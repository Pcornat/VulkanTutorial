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
#include <any>

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
	static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;
	// Members
	GLFWwindow *window{ nullptr };
	vk::DynamicLoader dl;
	vk::UniqueInstance instance;
	vk::UniqueDebugUtilsMessengerEXT callback;
	vk::UniqueSurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::UniqueDevice device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::UniqueSwapchainKHR swapChain;
	vk::SwapchainKHR oldSwpChain;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::UniqueImageView> swapChainImageViews;
	vk::UniqueRenderPass renderPass;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniquePipeline pipeline;
	std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
	vk::UniqueCommandPool commandPool;
	std::vector<vk::UniqueCommandBuffer> commandBuffers;
	std::array<vk::UniqueSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores;
	std::array<vk::UniqueSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores;
	std::array<vk::UniqueFence, MAX_FRAMES_IN_FLIGHT> inFlightFences;
	std::vector<vk::Fence> imagesInFlight;
	std::size_t currentFrame{ 0 };
	bool framebufferResized{ false };

	std::vector<std::string> validationLayers{ "VK_LAYER_KHRONOS_validation" };
	std::vector<std::string> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::vector<std::any> tmpVal;

	const std::string windowName = "Hello";
	static const char *const appName;
	uint32_t largeur = 800;
	uint32_t hauteur = 600;

#ifdef NDEBUG
	static constexpr bool enableValidationLayers = false;
#else
	static constexpr bool enableValidationLayers = true;
#endif

	// Functions
	void initWindow();

	void initVulkan();

	void createInstance();

	static vk::DebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

	bool checkValidationLayersSupport();

	std::vector<const char *> getRequiredExtensions();

	void setupDebugCallback();

	void pickPhysicalDevice();

	bool isDeviceSuitable(const vk::PhysicalDevice &device);

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device);

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device);

	void createLogicalDevice();

	void createSurface();

	void createSwapChain();

	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &device);

	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

	[[nodiscard]] vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

	void createImageViews();

	void createGraphicsPipeline();

	vk::UniqueShaderModule createShaderModule(const std::vector<char> &code);

	void createRenderPass();

	void createFramebuffers();

	void createCommandPool();

	void createCommandBuffers();

	void createSyncObjects();

	void recreateSwapChain();

	void cleanupSwapChain();

	void mainLoop();

	void drawFrame();

	void cleanup();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
				  void *pUserData);

public:
	HelloTriangleApp() = default;

	HelloTriangleApp(std::string windowName, const uint32_t l, const uint32_t h);

	/**
	 * \brief To add a validation layer before the run method.
	 * \param validationLayers_
	 * \todo Need to think how to correctly copy…
	 */
	[[maybe_unused]] [[maybe_unused]] void addValidationLayer(const std::string &validationLayers_);

	virtual ~HelloTriangleApp() = default;

	void run();

};


#endif //VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
