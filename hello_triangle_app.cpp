#include "hello_triangle_app.h"
#include <iostream>
#include <cstring>
#include <utility>

const std::string HelloTriangleApp::appName{ "Vulkan Tutorial" };

void HelloTriangleApp::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApp::createInstance() {
	if (enableValidationLayers && !checkValidationLayersSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	vk::ApplicationInfo appInfo;
	vk::InstanceCreateInfo createInfo;

	appInfo.setPNext(nullptr);
	appInfo.pApplicationName = HelloTriangleApp::appName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &appInfo;


	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

//	if (enableValidationLayers) {
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.setPpEnabledLayerNames(validationLayers.data());

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
//	} else {
//		createInfo.enabledLayerCount = 0;
//	}

	this->instance = vk::createInstance(createInfo); //C++ bindings have exceptions enabled by defaults so no need to check the value anymore.
}


bool HelloTriangleApp::checkValidationLayersSupport() {
	//uint32_t layerCount;
	auto availableLayers = vk::enumerateInstanceLayerProperties();

	/*vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<vk::LayerProperties> availableLayers(layerCount);
	vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());*/

	for (const char *layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
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

//	if (enableValidationLayers)
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void HelloTriangleApp::initVulkan() {
	createInstance();
	setupDebugCallback();
	pickPhysicalDevice();
}

void HelloTriangleApp::pickPhysicalDevice() {
	auto devices = this->instance.enumeratePhysicalDevices();
}

/*VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}*/

void HelloTriangleApp::setupDebugCallback() {
//	if (!enableValidationLayers) return;

	vk::DebugUtilsMessengerCreateInfoEXT createInfoEXT{};
	createInfoEXT.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
									vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
									vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

	createInfoEXT.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
								| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
								| vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;

	createInfoEXT.setPNext(nullptr);
	createInfoEXT.setPfnUserCallback(debugCallback);
	createInfoEXT.setPUserData(nullptr);

	auto CreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(this->instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));

	if (CreateDebugUtilsMessengerEXT != nullptr) {
		const VkDebugUtilsMessengerCreateInfoEXT tmp(createInfoEXT);
		if (CreateDebugUtilsMessengerEXT(instance, &tmp, nullptr, &this->callback) != VkResult::VK_SUCCESS)
			throw std::runtime_error("failed to set up debug callback!");
	} else {
		throw std::runtime_error("Cannot find DebugUtilsMessengerEXT function");
	}
}

void HelloTriangleApp::mainLoop() {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::cleanup() {
//	if (enableValidationLayers) {
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(this->instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		func(this->instance, this->callback, nullptr);
	}
//	}

//	this->instance.destroy();

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

HelloTriangleApp::~HelloTriangleApp() {
	this->cleanup();
}

vk::Bool32 HelloTriangleApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
										   [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
										   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
										   [[maybe_unused]] void *pUserData) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) { // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
	return VK_TRUE;
}