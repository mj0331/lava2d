#if !defined(L2D_APP_H)
#define L2D_APP_H

#define GLFW_INCLUDE_VULKAN

#if(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <VEZ.h>

#include <string>

#include "version.h"
#include "logging.h"

namespace l2d{
    
    class app{
        private:
            GLFWwindow* window;
            VkInstance instance;
            VkPhysicalDevice physicalDevice;
            VkSurfaceKHR surface;
            VkDevice device;
            VezSwapchain swapchain;

            int width;
            int height;
            std::string title;
            l2d::version app_version;
            l2d::version l2d_version;

            bool init_glfw();
            bool init_vk();
        public:
            app(int width = 960, int height = 540, std::string title = "App", l2d::version version = {0, 0, 0});
            ~app();

            static l2d::log app_log;
    };
}

#endif // L2D_APP_H
