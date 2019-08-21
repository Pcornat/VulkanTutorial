#ifndef VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
#define VULKANTUTORIAL_HELLO_TRIANGLE_APP_H

#ifdef WIN32

#define GLFW_DLL

#endif

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	[[gnu::always_inline]] inline bool isComplete() {
		return graphicsFamily.has_value();
	}
};

/**
 * @class HelloTriangleApp
 */
class HelloTriangleApp {
private:
	GLFWwindow *window = nullptr;
	vk::UniqueInstance instance;
	vk::DebugUtilsMessengerEXT callback;
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;

	std::vector<std::string> validationLayers{ "VK_LAYER_KHRONOS_validation" };
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

	static void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfoEXT);

	void setupDebugCallback();

	bool checkValidationLayersSupport();

	std::vector<const char *> getRequiredExtensions();

	void createInstance();

	void pickPhysicalDevice();

	static bool isDeviceSuitable(const vk::PhysicalDevice &device);

	static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device);

	void createLogicalDevice();

	void initWindow();

	void mainLoop();

	void cleanup();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				  void *pUserData);

public:
	HelloTriangleApp() = default;

	HelloTriangleApp(std::string windowName, uint32_t l, uint32_t h);

	/**
	 * \brief To add a validation layer before the run method.
	 * \param validationLayers_
	 * \todo Need to think how to correctly copy…
	 */
	void addValidationLayer(const std::string &validationLayers_);

	virtual ~HelloTriangleApp() = default;

	void run();

};


#endif //VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
