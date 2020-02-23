#include "hello_triangle_app.h"
#include <iostream>
#include <cstring>
#include <utility>
#include <sstream>
#include <set>
#include <algorithm>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

const std::string HelloTriangleApp::appName{ "Vulkan Tutorial" };

void HelloTriangleApp::run() {
	auto vkGetInstanceProcAddr = this->dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApp::addValidationLayer(const std::string &validationLayers_) {
	this->validationLayers.emplace_back(validationLayers_.c_str());
}

vk::DebugUtilsMessengerCreateInfoEXT HelloTriangleApp::populateDebugMessengerCreateInfo() {
	vk::DebugUtilsMessengerCreateInfoEXT createInfoEXT;
	createInfoEXT.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

	createInfoEXT.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
								| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
								| vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;

	createInfoEXT.setPNext(nullptr);
	createInfoEXT.setPfnUserCallback(debugCallback);
	createInfoEXT.setPUserData(nullptr);
	return createInfoEXT;
}

void HelloTriangleApp::createInstance() {
	if (enableValidationLayers && !checkValidationLayersSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	vk::ApplicationInfo appInfo;
	vk::InstanceCreateInfo createInfo{};
	std::vector<const char *> extensions, layers_names; // Needed because it can cause problem if it is declared inside the condition.
	layers_names.reserve(validationLayers.size());

	appInfo.setPNext(nullptr);
	appInfo.pApplicationName = HelloTriangleApp::appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext = nullptr;


	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions = nullptr;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	/*auto extensionsProperties = vk::enumerateInstanceExtensionProperties();
	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensionsProperties) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}*/
	const auto debugCreateInfo = populateDebugMessengerCreateInfo();
	if (enableValidationLayers) {
		for (const auto &item : validationLayers) {
			layers_names.push_back(item.c_str());
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = layers_names.data();

		extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		createInfo.pNext = &debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	this->instance = vk::createInstanceUnique(createInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->instance);
	//C++ bindings have exceptions enabled by defaults so no need to check the value anymore.
}

bool HelloTriangleApp::checkValidationLayersSupport() {
	//uint32_t layerCount;
	auto availableLayers = vk::enumerateInstanceLayerProperties();

	/*vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<vk::LayerProperties> availableLayers(layerCount);
	vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());*/

	for (const auto &layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (/* strcmp(layerName, layerProperties.layerName) == 0 */ layerName == layerProperties.layerName) {
				//LayerName is a char[256] useless to use std::string, it would add more compute time.
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char *> HelloTriangleApp::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void HelloTriangleApp::initVulkan() {
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
}

void HelloTriangleApp::pickPhysicalDevice() {
	auto devices = this->instance->enumeratePhysicalDevices(); // devices = std::vector<vk::PhysicalDevice>
	if (devices.empty()) {
		throw std::runtime_error("Failed to find GPU with Vulkan support.");
	} else if (devices.size() != 1) {
		std::stringstream message("More than one GPU. This situation is not dealt with yet.\n Exception at ");
		message << __FILE__ << ", line : " << __LINE__ << '\n';
		throw std::runtime_error(message.str());
	}

	for (const auto &device : devices) {
		if (HelloTriangleApp::isDeviceSuitable(device)) {
			this->physicalDevice = device;
			break;
		}
	}

	if (!this->physicalDevice)
		throw std::runtime_error("Failed to find a suitable GPU.");

}

void HelloTriangleApp::setupDebugCallback() {
	if (!enableValidationLayers) return;
	{
		this->callback = this->instance->createDebugUtilsMessengerEXTUnique(populateDebugMessengerCreateInfo());
	}
}

void HelloTriangleApp::mainLoop() {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::cleanup() {
//	if (enableValidationLayers) {
//		const vk::DispatchLoaderDynamic dldi(*this->instance, vkGetInstanceProcAddr);
//		this->instance->destroyDebugUtilsMessengerEXT(this->callback, nullptr, dldi);
//	}

//	this->instance->destroySurfaceKHR(this->surface);

	glfwDestroyWindow(this->window);

	glfwTerminate();
}

void HelloTriangleApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Tell glfw to NOT create OpenGL context.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //No resizable window because it's a special case so not for now.

	window = glfwCreateWindow(this->largeur, this->hauteur, this->windowName.c_str(), nullptr, nullptr);
}

HelloTriangleApp::HelloTriangleApp(std::string windowName, uint32_t l, uint32_t h) : windowName(std::move(windowName)), largeur(l), hauteur(h) {}

VKAPI_ATTR vk::Bool32 VKAPI_CALL HelloTriangleApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																 [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
																 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
																 [[maybe_unused]] void *pUserData) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
	return VK_TRUE;
}

bool HelloTriangleApp::isDeviceSuitable(const vk::PhysicalDevice &device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool HelloTriangleApp::checkDeviceExtensionSupport(const vk::PhysicalDevice &device) {
	auto availableExtensions = device.enumerateDeviceExtensionProperties();
	std::set<std::string> requiredExtensions(this->deviceExtensions.cbegin(), this->deviceExtensions.cend());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices HelloTriangleApp::findQueueFamilies(const vk::PhysicalDevice &device) {
	QueueFamilyIndices indices;
	const auto queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		vk::Bool32 presentSupport = false;
		presentSupport = device.getSurfaceSupportKHR(i, *this->surface);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void HelloTriangleApp::createLogicalDevice() {
	float queuePriority = 1.0f;
	vk::PhysicalDeviceFeatures deviceFeatures;
	vk::DeviceCreateInfo createInfo;

	QueueFamilyIndices indices = findQueueFamilies(this->physicalDevice);
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	std::vector<const char *> layer_names, extensions;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilies.size());

	layer_names.reserve(validationLayers.size());
	extensions.reserve(this->deviceExtensions.size());

	for (decltype(uniqueQueueFamilies)::value_type queueFamily : uniqueQueueFamilies) {
//		vk::DeviceQueueCreateInfo queueCreateInfo;
//		queueCreateInfo.queueFamilyIndex = queueFamily;
//		queueCreateInfo.queueCount = 1;
//		queueCreateInfo.pQueuePriorities = &queuePriority;
//		queueCreateInfos.push_back(queueCreateInfo);
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{{}, queueFamily, 1, &queuePriority });
	}

	for (const auto &extension_name : this->deviceExtensions) {
		extensions.push_back(extension_name.c_str());
	}

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(this->deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		for (const auto &layer_name : validationLayers) {
			layer_names.push_back(layer_name.c_str());
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = layer_names.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	this->device = this->physicalDevice.createDeviceUnique(createInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->device);

	this->graphicsQueue = this->device->getQueue(indices.graphicsFamily.value(), 0);
	this->presentQueue = this->device->getQueue(indices.presentFamily.value(), 0);
}

void HelloTriangleApp::createSurface() {
	VkSurfaceKHR psurf = nullptr;
	if (glfwCreateWindowSurface(*this->instance, this->window, nullptr, &psurf) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface !");
	}
	this->surface = vk::UniqueSurfaceKHR(psurf, *this->instance);
}

SwapChainSupportDetails HelloTriangleApp::querySwapChainSupport(const vk::PhysicalDevice &device) {
	SwapChainSupportDetails details;
	device.getSurfaceCapabilitiesKHR(*this->surface, &details.capabilities);
	details.formats = device.getSurfaceFormatsKHR(*this->surface);
	details.presentModes = device.getSurfacePresentModesKHR(*this->surface);
	return details;
}

vk::SurfaceFormatKHR HelloTriangleApp::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
	for (const auto &availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

vk::PresentModeKHR HelloTriangleApp::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
	for (const auto &mode : availablePresentModes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			return mode;
		}
	}
	return vk::PresentModeKHR::eFifoRelaxed;
}

vk::Extent2D HelloTriangleApp::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		vk::Extent2D actualExtent{ largeur, hauteur };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

void HelloTriangleApp::createSwapChain() {
	const auto swapChainSupport = querySwapChainSupport(this->physicalDevice);
	const auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	const auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	const auto extent = chooseSwapExtent(swapChainSupport.capabilities);

	std::uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	vk::SwapchainCreateInfoKHR createInfo{{},
										  *this->surface,
										  imageCount,
										  surfaceFormat.format,
										  surfaceFormat.colorSpace,
										  extent,
										  1,
										  vk::ImageUsageFlagBits::eColorAttachment };
	const auto indices = findQueueFamilies(physicalDevice);
	const std::uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0; // Optionnel
		createInfo.pQueueFamilyIndices = nullptr; // Optionnel
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = true;
	createInfo.oldSwapchain = nullptr;
	this->swapChain = this->device->createSwapchainKHRUnique(createInfo);
	this->swapChainImages = this->device->getSwapchainImagesKHR(*this->swapChain);
}

void HelloTriangleApp::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());
	{
		const auto swapChainSupport = querySwapChainSupport(this->physicalDevice);
		const auto format = chooseSwapSurfaceFormat(swapChainSupport.formats).format;
		for (size_t i = 0; i < swapChainImages.size(); ++i) {
			swapChainImageViews[i] =
					this->device->createImageViewUnique(vk::ImageViewCreateInfo{{},
																				swapChainImages[i],
																				vk::ImageViewType::e2D,
																				format,
																				vk::ComponentMapping{},
																				vk::ImageSubresourceRange{
																						vk::ImageAspectFlagBits::eColor,
																						0,
																						1,
																						0,
																						1
																				}
					});
		}
	}
}