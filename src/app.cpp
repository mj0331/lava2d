#include "../include/app.h"

#include <iostream>
#include <vector>

namespace l2d{
    app::app(int width, int height, std::string title, l2d::version version)
    {
        this->width = width;
        this->height = height;
        this->title = title;
        this->app_version = version;
        this->l2d_version = {0, 0, 1};
        
        std::cout << title << " v" << version.major << "." << version.minor << "." << version.patch <<"\n";
        std::cout << "Lava2D" << " v" << l2d_version.major << "." << l2d_version.minor << "." << l2d_version.patch <<"\n";

        if(!init_glfw()) {
            std::cout << "Failed to initialise GLFW!\n";
            exit(1);
        }

        if(!init_vk()) {
            std::cout << "Failed to initialise Vulkan!\n";
            exit(2);
        }
    }

    app::~app()
    {
        vezDestroyDevice(device);
        vezDestroyInstance(instance);
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

        return true;
    }

    bool app::init_vk()
    {
        uint32_t extension_count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);

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

        // Create a surface to render to.
        result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (result != VK_SUCCESS)
            return false;

        // Enumerate and select the first discrete GPU physical device.
        uint32_t physicalDeviceCount;
        vezEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vezEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        for (auto pd : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vezGetPhysicalDeviceProperties(pd, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                physicalDevice = pd;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
            return false;

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
}