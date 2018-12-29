#ifndef VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
#define VULKANTUTORIAL_HELLO_TRIANGLE_APP_H


#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <string>

/**
 * @class HelloTriangleApp
 */
class HelloTriangleApp {
private:
	GLFWwindow* window = nullptr;
	vk::Instance instance;


	const std::string windowName = "Hello";
	static const std::string appName;
	uint32_t l = 800;
	uint32_t h = 600;

	void initVulkan();

	void createInstance();

	void initWindow();

	void mainLoop();

	void cleanup();

public:
	HelloTriangleApp() = default;

	HelloTriangleApp(const std::string& windowName, uint32_t l, uint32_t h);

	virtual ~HelloTriangleApp();

	void run();

};


#endif //VULKANTUTORIAL_HELLO_TRIANGLE_APP_H
