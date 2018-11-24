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
#include <chrono>

#include "version.h"
#include "logging.h"

namespace l2d{
	
	class app{
		private:
			static app* app_instance;

			GLFWwindow* window;
			VkInstance instance;
			VkPhysicalDevice physicalDevice;
			VkSurfaceKHR surface;
			VkDevice device;
			VezSwapchain swapchain;
			color_depth_framebuffer framebuffer;

			int width;
			int height;
			std::string title;
			l2d::version app_version;
			l2d::version l2d_version;

			bool signal_shutdown;
			std::chrono::steady_clock::time_point last_frame_time;

			bool init_glfw();
			bool init_vk();

			static void glfw_keyboard_event_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void glfw_cursor_position_event_handler(GLFWwindow* window, double x, double y);
			static void glfw_mouse_button_event_handler(GLFWwindow* window, int button, int action, int mods);
		protected:
			app(int width = 960, int height = 540, std::string title = "App", l2d::version version = {0, 0, 0});
			~app();

		public:
			static l2d::log app_log;

			static app* get_instance();

			/**
			 * Sends the shutdown signal to the app instance, so it shuts down the next update
			 * */
			inline void shutdown() { signal_shutdown = true; }

			/**
			 * The "root" update function. Override in client app class. 
			 **/
			virtual void update(float dt)
			{

			}

			/**
			 * Keyboard event handler for when a key is pressed down. Override in client to handle keyboard events
			 * */
			virtual void on_key_down(int key)
			{

			}

			/**
			 * Keyboard event handler for when a key is let go. Override in client to handle keyboard events
			 * */
			virtual void on_key_up(int key)
			{

			}

			/**
			 * Mouse movement event handler
			 * */
			virtual void on_mouse_move(int x, int y)
			{

			}

			/**
			 * Mouse button press event handler
			 * */
			virtual void on_mouse_down(int button, int x, int y)
			{

			}

			/**
			 * Mouse button release event handler
			 * */
			virtual void on_mouse_up(int button, int x, int y)
			{

			}

			/**
			 * Call this function to start the app loop
			 **/
			void run();

	};
}

#endif // L2D_APP_H
