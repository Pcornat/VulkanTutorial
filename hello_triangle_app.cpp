#include "hello_triangle_app.h"
#include <iostream>
#include <cstring>
#include <utility>
#include <sstream>

const std::string HelloTriangleApp::appName{ "Vulkan Tutorial" };

void HelloTriangleApp::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApp::addValidationLayer(const std::string &validationLayers_) {
	this->validationLayers.push_back(validationLayers_.c_str());
}

void HelloTriangleApp::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT &createInfoEXT) {
	createInfoEXT = vk::DebugUtilsMessengerCreateInfoEXT();
	createInfoEXT.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
									| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

	createInfoEXT.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
								| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
								| vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;

	createInfoEXT.setPNext(nullptr);
	createInfoEXT.setPfnUserCallback(debugCallback);
	createInfoEXT.setPUserData(nullptr);
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
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	populateDebugMessengerCreateInfo(debugCreateInfo);
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

	this->instance = vk::createInstanceUnique(createInfo); //C++ bindings have exceptions enabled by defaults so no need to check the value anymore.
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
	pickPhysicalDevice();
	createLogicalDevice();
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

	vk::DebugUtilsMessengerCreateInfoEXT createInfoEXT{};
	populateDebugMessengerCreateInfo(createInfoEXT);
	vk::DispatchLoaderDynamic dldi(*this->instance);

	this->callback = this->instance->createDebugUtilsMessengerEXT(createInfoEXT, nullptr, dldi);

	/* auto CreateDebugUtilsMessengerEXT =
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(this->instance->getProcAddr("vkCreateDebugUtilsMessengerEXT"));

	if (CreateDebugUtilsMessengerEXT != nullptr) {
		const VkDebugUtilsMessengerCreateInfoEXT tmp(createInfoEXT);
		if (CreateDebugUtilsMessengerEXT(*instance, &tmp, nullptr, &this->callback) != VkResult::VK_SUCCESS)
			throw std::runtime_error("failed to set up debug callback!");
	} else {
		throw std::runtime_error("Cannot find DebugUtilsMessengerEXT function");
	} */
}

void HelloTriangleApp::mainLoop() {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::cleanup() {
	if (enableValidationLayers) {
		vk::DispatchLoaderDynamic dldi(*this->instance);
		this->instance->destroyDebugUtilsMessengerEXT(this->callback, nullptr, dldi);
	}

	glfwDestroyWindow(this->window);

	glfwTerminate();
}

void HelloTriangleApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Tell glfw to NOT create OpenGL context.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //No resizable window because it's a special case so not for now.

	window = glfwCreateWindow(this->l, this->h, this->windowName.c_str(), nullptr, nullptr);
}

HelloTriangleApp::HelloTriangleApp(std::string windowName, uint32_t l, uint32_t h) : windowName(std::move(windowName)), l(l), h(h) {}

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
	return HelloTriangleApp::findQueueFamilies(device).isComplete();
}

QueueFamilyIndices HelloTriangleApp::findQueueFamilies(const vk::PhysicalDevice &device) {
	QueueFamilyIndices indices;
	auto queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
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
	std::vector<const char *> layer_names;
	vk::DeviceQueueCreateInfo queueCreateInfo;
	vk::PhysicalDeviceFeatures deviceFeatures;
	vk::DeviceCreateInfo createInfo;

	layer_names.reserve(validationLayers.size());
	QueueFamilyIndices indices = findQueueFamilies(this->physicalDevice);

	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

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
	this->graphicsQueue = this->device->getQueue(indices.graphicsFamily.value(), 0);
}
