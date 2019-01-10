#ifndef VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
#define VULKANTUTORIAL_HELLO_TRIANGLE_APP_H

#ifdef WIN32

#define GLFW_DLL

#endif

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <string>

/**
 * @class HelloTriangleApp
 */
class HelloTriangleApp {
private:
	GLFWwindow* window = nullptr;
	vk::Instance instance;
	VkDebugUtilsMessengerEXT callback;

	const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
	};
	const std::string windowName = "Hello";
	static const std::string appName;
	uint32_t l = 800;
	uint32_t h = 600;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	void initVulkan();

	void setupDebugCallback();

	bool checkValidationLayersSupport();

	std::vector<const char*> getRequiredExtensions();

	void createInstance();

	void initWindow();

	void mainLoop();

	void cleanup();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				  void* pUserData);

public:
	HelloTriangleApp() = default;

	HelloTriangleApp(const std::string& windowName, uint32_t l, uint32_t h);

	virtual ~HelloTriangleApp();

	void run();

};


#endif //VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
