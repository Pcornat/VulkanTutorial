#ifndef VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
#define VULKANTUTORIAL_HELLO_TRIANGLE_APP_H

#ifdef WIN32

#define GLFW_DLL

#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	[[gnu::always_inline]] inline bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};


/**
 * @class HelloTriangleApp
 */
class HelloTriangleApp {
private:
	GLFWwindow *window = nullptr;
	vk::DynamicLoader dl;
	vk::UniqueInstance instance;
//	vk::UniqueHandle<vk::Instance, vk::DispatchLoaderDynamic> instance;
	vk::UniqueDebugUtilsMessengerEXT callback;
//	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> callback;
	vk::UniqueSurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
//	vk::UniqueHandle<vk::Device, vk::DispatchLoaderDynamic> device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;

	std::vector<std::string> validationLayers{ "VK_LAYER_KHRONOS_validation" };
	std::vector<std::string> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::string windowName = "Hello";
	static const std::string appName;
	uint32_t largeur = 800;
	uint32_t hauteur = 600;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	void initVulkan();

	static vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

	void setupDebugCallback();

	bool checkValidationLayersSupport();

	std::vector<const char *> getRequiredExtensions();

	void createInstance();

	void pickPhysicalDevice();

	bool isDeviceSuitable(const vk::PhysicalDevice &device);

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device);

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device);

	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device);

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

	void createLogicalDevice();

	void createSurface();

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
