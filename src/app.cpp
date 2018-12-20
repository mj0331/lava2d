#include "../include/app.h"

#include <iostream>
#include <vector>
#include <fstream>

namespace l2d{
    //Init static members, such as the logger and singleton instance
    l2d::log app::app_log = l2d::log("./log.txt");
    l2d::app* app::app_instance = nullptr;
    app::app(int width, int height, std::string title, l2d::version version)
    {
        this->width = width;
        this->height = height;
        this->title = title;
        this->app_version = version;
        this->l2d_version = {0, 0, 1};
        this->signal_shutdown = false;
        this->last_frame_time = std::chrono::high_resolution_clock::now();

        if(app_instance == nullptr) {
            app_instance = this;
        }
        
        app::app_log.info("{} v{}.{}.{}", title, version.major, version.minor, version.patch);
        app::app_log.info("Lava2D v{}.{}.{}", l2d_version.major, l2d_version.minor, l2d_version.patch);
        
        app::app_log.info("Initialising GLFW...");
        if(!init_glfw()) {
            app::app_log.critical("Failed to initialise GLFW!");
            exit(1);
        }
        app::app_log.info("GLFW3 initialised.");

        app::app_log.info("Initialising Vulkan...");
        if(!init_vk()) {
            app::app_log.critical("Failed to initialise Vulkan!");
            exit(2);
        }
        app::app_log.info("Vulkan initialised.");

        app::app_log.info("Creating framebuffer...");
        create_framebuffer();
    }

    app::~app()
    {
        app::app_log.warn("Cleaning up VEZ...");
        vezDestroyDevice(device);
        vezDestroyInstance(instance);
        app::app_log.warn("Shutting down GLFW...");
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    void app::get_window_size(int & w, int & h) {
        w = width;
        h = height;
    }

    bool app::init_glfw()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(  this->width,
                                    this->height,
                                    this->title.c_str(), 
                                    nullptr, 
                                    nullptr);

        if (!window)
            return false;

        app::app_log.info("Setting event handlers...");
        glfwSetMouseButtonCallback(window, glfw_mouse_button_event_handler);
        glfwSetKeyCallback(window, glfw_keyboard_event_handler);
        glfwSetCursorPosCallback(window, glfw_cursor_position_event_handler);

        return true;
    }

    bool app::init_vk()
    {
        uint32_t extension_count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);

        app::app_log.info("Loading instance extensions required by GLFW...");
        for(unsigned int i = 0; i < extension_count; i++) 
        {
            app::app_log.info("Loading {}", extensions[i]);
        }

        // Create the V-EZ instance.
        VezApplicationInfo appInfo = {};
        appInfo.pApplicationName = title.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.patch);
        appInfo.pEngineName = "Lava2D";
        appInfo.applicationVersion = VK_MAKE_VERSION(l2d_version.major, l2d_version.minor, l2d_version.patch);

        VezInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = extension_count;
        instanceCreateInfo.ppEnabledExtensionNames = extensions;

        VkResult result = vezCreateInstance(&instanceCreateInfo, &instance);
        if (result != VK_SUCCESS)
            return false;
        app::app_log.info("Vulkan instance created");

        // Create a surface to render to.
        result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (result != VK_SUCCESS)
            return false;
        app::app_log.info("Window surface created");

        // Enumerate and select the first discrete GPU physical device.
        app::app_log.info("Finding Vulkan compatible devices and picking best one for rendering...");
        uint32_t physicalDeviceCount;
        vezEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vezEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        for (auto pd : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vezGetPhysicalDeviceProperties(pd, &properties);

            app::app_log.info("Found {} Driver version {}", properties.deviceName, properties.driverVersion);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                physicalDevice = pd;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
            return false;
        
        //Query selected device for supported extensions and list them
        uint32_t supported_extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, nullptr);
        std::vector<VkExtensionProperties> supported_extensions(supported_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, supported_extensions.data());
        
        app::app_log.info("Supported extensions on the device:");
        for(const auto& extension : supported_extensions) {
            app::app_log.info("\t{}", extension.extensionName);
        }

        // Create a logical device connection to the physical device and load the swapchain extension
        std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VezDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = device_extensions.data();
        result = vezCreateDevice(physicalDevice, &deviceCreateInfo, &device);
        if (result != VK_SUCCESS)
            return false;

        // Create the swapchain.
        VezSwapchainCreateInfo swapchainCreateInfo = {};
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        result = vezCreateSwapchain(device, &swapchainCreateInfo, &swapchain);
        if (result != VK_SUCCESS)
            return false;

        return true;
    }

    app* app::get_instance() {
        return app::app_instance == nullptr ? new app() : app::app_instance; 
    }

    void app::run()
    {
        while(!signal_shutdown && !glfwWindowShouldClose(window)) {
            //Poll for window and input events
            glfwPollEvents();

            float dt = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - last_frame_time).count() / 1000.0f;
            update(dt);
        }
    }

    void app::glfw_keyboard_event_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(action == GLFW_PRESS)
        {
            app::get_instance()->on_key_down(key);
        }
        else if(action == GLFW_RELEASE)
        {
            app::get_instance()->on_key_up(key);
        }
    }

    void app::glfw_cursor_position_event_handler(GLFWwindow* window, double x, double y)
    {
        app::get_instance()->on_mouse_move(static_cast<int>(x), static_cast<int>(y));
    }

    void app::glfw_mouse_button_event_handler(GLFWwindow* window, int button, int action, int mods)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        if (action == GLFW_PRESS)
        {
            app::get_instance()->on_mouse_down(button, static_cast<int>(x), static_cast<int>(y));
        }
        else if (action == GLFW_RELEASE)
        {
            app::get_instance()->on_mouse_up(button, static_cast<int>(x), static_cast<int>(y));
        }
    }

    void app::set_root_scene(l2d::scene * scene) { 
        app::app_log.info("Setting root scene of app to {}", scene->get_id());
        root_scene = scene; 
    }

    void app::create_framebuffer()
    {
        // Free previous allocations.
        if (framebuffer.handle)
        {
            vezDestroyFramebuffer(device, framebuffer.handle);
            vezDestroyImageView(device, framebuffer.colorImageView);
            vezDestroyImageView(device, framebuffer.depthStencilImageView);
            vezDestroyImage(device, framebuffer.colorImage);
            vezDestroyImage(device, framebuffer.depthStencilImage);
        }

        // Get the current window dimension.
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Get the swapchain's current surface format.
        VkSurfaceFormatKHR swapchainFormat = {};
        vezGetSwapchainSurfaceFormat(swapchain, &swapchainFormat);

        // Create the color image for the m_framebuffer.
        VezImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = swapchainFormat.format;
        imageCreateInfo.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = sample_count_flag;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        auto result = vezCreateImage(device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &framebuffer.colorImage);
        if (result != VK_SUCCESS)
        {
            std::cout << " vkCreateImage failed (" << result << ")\n";
            return;
        }

        // Create the image view for binding the texture as a resource.
        VezImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.image = framebuffer.colorImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = imageCreateInfo.format;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        result = vezCreateImageView(device, &imageViewCreateInfo, &framebuffer.colorImageView);
        if (result != VK_SUCCESS)
        {
            std::cout << " vkCreateImageView failed (" << result << ")\n";
            return;
        }

        // Create the depth image for the m_framebuffer.
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = VK_FORMAT_D32_SFLOAT;
        imageCreateInfo.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = sample_count_flag;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        result = vezCreateImage(device, VEZ_MEMORY_GPU_ONLY, &imageCreateInfo, &framebuffer.depthStencilImage);
        if (result != VK_SUCCESS)
        {
            std::cout << " vkCreateImage failed (" << result << ")\n";
            return;
        }

        // Create the image view for binding the texture as a resource.
        imageViewCreateInfo.image = framebuffer.depthStencilImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = imageCreateInfo.format;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        result = vezCreateImageView(device, &imageViewCreateInfo, &framebuffer.depthStencilImageView);
        if (result != VK_SUCCESS)
        {
            std::cout << " vkCreateImageView failed (" << result << ")\n";
            return;
        }

        // Create the m_framebuffer.
        std::array<VkImageView, 2> attachments = { framebuffer.colorImageView, framebuffer.depthStencilImageView };
        VezFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;
        framebufferCreateInfo.layers = 1;
        result = vezCreateFramebuffer(device, &framebufferCreateInfo, &framebuffer.handle);
        if (result != VK_SUCCESS)
        {
            std::cout << "vkCreateFramebuffer failed (" << result << ")\n";
            return;
        }
    }


    VkShaderModule app::create_shader_module(const std::string& filename, const std::string& entryPoint, VkShaderStageFlagBits stage)
    {
        // Load the GLSL shader code from disk.
        std::ifstream filestream(filename.c_str(), std::ios::in | std::ios::binary);
        if (!filestream.good())
        {
            std::cout << "Failed to open " << filename << "\n";
            return VK_NULL_HANDLE;
        }

        std::string code = std::string((std::istreambuf_iterator<char>(filestream)), std::istreambuf_iterator<char>());
        filestream.close();

        // Create the shader module.
        VezShaderModuleCreateInfo createInfo = {};
        createInfo.stage = stage;
        createInfo.codeSize = static_cast<uint32_t>(code.size());
        if (filename.find(".spv") != std::string::npos)
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.c_str());
        else
            createInfo.pGLSLSource = code.c_str();
        createInfo.pEntryPoint = entryPoint.c_str();

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        auto result = vezCreateShaderModule(device, &createInfo, &shaderModule);
        if (result != VK_SUCCESS && shaderModule != VK_NULL_HANDLE)
        {
            // If shader module creation failed but error is from GLSL compilation, get the error log.
            uint32_t infoLogSize = 0;
            vezGetShaderModuleInfoLog(shaderModule, &infoLogSize, nullptr);

            std::string infoLog(infoLogSize, '\0');
            vezGetShaderModuleInfoLog(shaderModule, &infoLogSize, &infoLog[0]);

            vezDestroyShaderModule(device, shaderModule);

            std::cout << infoLog << "\n";
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }

    bool app::create_pipeline(const std::vector<pipeline_shader_info>& pipelineShaderInfo, VezPipeline* pPipeline, std::vector<VkShaderModule>* shaderModules)
    {
        // Create shader modules.
        app::app_log.info("PIPELINE: New shader pipeline being created...");
        std::vector<VezPipelineShaderStageCreateInfo> shaderStageCreateInfo(pipelineShaderInfo.size());
        for (auto i = 0U; i < pipelineShaderInfo.size(); ++i)
        {
            auto filename = std::get<0>(pipelineShaderInfo[i]);
            auto stage = std::get<1>(pipelineShaderInfo[i]);
            auto shaderModule = create_shader_module(filename, "main", stage);
            if (shaderModule == VK_NULL_HANDLE)
            {
                app::app_log.error("CreateShaderModule failed!");
                return false;
            }

            app::app_log.info("\tSHADER: {}", filename);

            shaderStageCreateInfo[i].module = shaderModule;
            shaderStageCreateInfo[i].pEntryPoint = "main";
            shaderStageCreateInfo[i].pSpecializationInfo = nullptr;

            shaderModules->push_back(shaderModule);
        }

        // Determine if this is a compute only pipeline.
        bool isComputePipeline = (pipelineShaderInfo.size() == 1 && std::get<1>(pipelineShaderInfo[0]) == VK_SHADER_STAGE_COMPUTE_BIT);

        // Create the graphics pipeline or compute pipeline.
        if (isComputePipeline)
        {
            VezComputePipelineCreateInfo pipelineCreateInfo = {};
            pipelineCreateInfo.pStage = shaderStageCreateInfo.data();
            if (vezCreateComputePipeline(device, &pipelineCreateInfo, pPipeline) != VK_SUCCESS)
            {
                app::app_log.error("vkCreateComputePipeline failed!");
                return false;
            }
        }
        else
        {
            VezGraphicsPipelineCreateInfo pipelineCreateInfo = {};
            pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfo.size());
            pipelineCreateInfo.pStages = shaderStageCreateInfo.data();
            if (vezCreateGraphicsPipeline(device, &pipelineCreateInfo, pPipeline) != VK_SUCCESS)
            {
                app::app_log.error("vkCreateGraphicsPipeline failed!");
                return false;
            }
        }

        // Return success.
        return true;
    }
}