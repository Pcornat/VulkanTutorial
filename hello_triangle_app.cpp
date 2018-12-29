//
// Created by Florent on 29/12/2018.
//

#include "hello_triangle_app.h"

const std::string HelloTriangleApp::appName = "Vulkan Tutorial";

void HelloTriangleApp::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApp::createInstance() {
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
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	vk::createInstance(createInfo); //C++ bindings have exceptions enabled by defaults so no need to check the value anymore.

}

void HelloTriangleApp::initVulkan() {
	createInstance();
}

void HelloTriangleApp::mainLoop() {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::cleanup() {
	glfwDestroyWindow(this->window);

	glfwTerminate();
}

void HelloTriangleApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Tell glfw to NOT create OpenGL context.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //No resizable window because it's a special case so not for now.

	window = glfwCreateWindow(this->l, this->h, this->windowName.c_str(), nullptr, nullptr);


}

HelloTriangleApp::HelloTriangleApp(const std::string& windowName, uint32_t l, uint32_t h) : window(nullptr), windowName(windowName), l(l), h(h) {}

HelloTriangleApp::~HelloTriangleApp() {
	this->cleanup();
}