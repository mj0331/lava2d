#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <VEZ.h>

GLFWwindow* window = nullptr;
VkInstance instance = VK_NULL_HANDLE;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkSurfaceKHR surface = VK_NULL_HANDLE;
VkDevice device = VK_NULL_HANDLE;
VezSwapchain swapchain = VK_NULL_HANDLE;

bool InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "MyApplication", nullptr, nullptr);
    if (!window)
        return false;

    return true;
}

bool InitVulkanEZ()
{
    uint32_t extension_count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);

    // Create the V-EZ instance.
    VezApplicationInfo appInfo = {};
    appInfo.pApplicationName = "MyApplication";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "MyEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

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

    // Create a logical device connection to the physical device.
    VezDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
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

int main(int argc, char** argv)
{
    if (!InitGLFW())
    {
        std::cout << "Failed to create GLFW window!\n";
        return -1;
    }

    if (!InitVulkanEZ())
    {
        std::cout << "Failed to initialize V-EZ!\n";
        return -1;
    }

    vezDestroyDevice(device);
    vezDestroyInstance(instance);
    return 0;
}