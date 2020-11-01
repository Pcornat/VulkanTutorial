#include "hello_triangle_app.h"
#include <iostream>
#include <cstring>
#include <utility>
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

const char *const HelloTriangleApp::appName{ "Vulkan Tutorial" };

void HelloTriangleApp::run() {
	auto vkGetInstanceProcAddr = this->dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

[[maybe_unused]] void HelloTriangleApp::addValidationLayer(const std::string &validationLayers_) {
	this->validationLayers.emplace_back(validationLayers_.c_str());
}

vk::DebugUtilsMessengerCreateInfoEXT HelloTriangleApp::populateDebugMessengerCreateInfo() {
	return vk::DebugUtilsMessengerCreateInfoEXT{{},
												vk::DebugUtilsMessageSeverityFlagsEXT{
														vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
														| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
														| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose },
												vk::DebugUtilsMessageTypeFlagsEXT{
														vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
														| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
														| vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral },
												debugCallback,
												nullptr };
}

void HelloTriangleApp::createInstance() {
	if (enableValidationLayers && !checkValidationLayersSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	const vk::ApplicationInfo appInfo{
			HelloTriangleApp::appName,
			VK_MAKE_VERSION(1, 0, 0),
			"No Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_2
	};
	vk::InstanceCreateInfo createInfo{{}, &appInfo };
	std::vector<const char *> extensions, layers_names; // Needed because it can cause problem if it is declared inside the condition.
	layers_names.reserve(validationLayers.size());

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	/*auto extensionsProperties = vk::enumerateInstanceExtensionProperties();
	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensionsProperties) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}*/
	const auto debugCreateInfo = populateDebugMessengerCreateInfo();
	if constexpr (enableValidationLayers) {
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
	const auto availableLayers = vk::enumerateInstanceLayerProperties();

	for (const auto &layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (layerName == layerProperties.layerName) {
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
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
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
		drawFrame();
	}
}

void HelloTriangleApp::drawFrame() {
	this->device->waitForFences(*inFlightFences[currentFrame], true, std::numeric_limits<std::uint32_t>::max());
	this->device->resetFences(*inFlightFences[currentFrame]);
	const auto result = this->device->acquireNextImageKHR(*swapChain,
														  std::numeric_limits<std::uint32_t>::max(),
														  *imageAvailableSemaphores[currentFrame],
														  vk::Fence{});
	if (result.result == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		return;
	} else if (result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR) {
		throw std::runtime_error("échec de la présentation d'une image à la swap chain!");
	}

	const std::uint32_t imageIndex = result.value;
	if (imagesInFlight[imageIndex]) {
		device->waitForFences(imagesInFlight[imageIndex], true, std::numeric_limits<std::uint32_t>::max());
	}
	imagesInFlight[imageIndex] = *inFlightFences[currentFrame];

	{
		const vk::PipelineStageFlags waitStages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
		const vk::SubmitInfo submitInfo{
				1,
				&imageAvailableSemaphores[currentFrame].get(),
				&waitStages,
				1,
				&commandBuffers[imageIndex].get(),
				1,
				&renderFinishedSemaphores[currentFrame].get() };

		device->resetFences(*inFlightFences[currentFrame]);

		graphicsQueue.submit(submitInfo, *inFlightFences[currentFrame]);
	}

	try {
		const vk::PresentInfoKHR presentInfo{ 1, &renderFinishedSemaphores[currentFrame].get(), 1, &swapChain.get(), &imageIndex };
		const auto resultQueue = presentQueue.presentKHR(presentInfo);
		if (resultQueue == vk::Result::eSuboptimalKHR || framebufferResized) {
			framebufferResized = false;
			recreateSwapChain();
		} else if (resultQueue != vk::Result::eSuccess) {
			throw std::runtime_error("échec de la présentation d'une image!");
		}
	} catch (const vk::OutOfDateKHRError &error) {
		framebufferResized = false;
		recreateSwapChain();
	} catch (const std::exception &exception) {
		std::cerr << "exception not handled\n";
		__throw_exception_again;
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApp::cleanup() {
	device->waitIdle();
	glfwDestroyWindow(this->window);

	glfwTerminate();
}

void HelloTriangleApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Tell glfw to NOT create OpenGL context.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(this->largeur, this->hauteur, this->windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow *wwindow, [[maybe_unused]] int width, [[maybe_unused]] int height) {
		auto *__restrict app = reinterpret_cast<HelloTriangleApp *>(glfwGetWindowUserPointer(wwindow));
		app->framebufferResized = true;
	});
}

HelloTriangleApp::HelloTriangleApp(std::string windowName, const uint32_t l, const uint32_t h) :
		windowName(std::move(windowName)), largeur(l), hauteur(h) {}

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
		requiredExtensions.erase(std::string(extension.extensionName));
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
		glfwGetFramebufferSize(window, reinterpret_cast<int *>(&largeur), reinterpret_cast<int *>(&hauteur));
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
	this->tmpVal.emplace_back(extent);
	this->tmpVal.emplace_back(surfaceFormat.format);

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
	oldSwpChain = !this->swapChain ? nullptr : this->swapChain.release();
	createInfo.oldSwapchain = oldSwpChain;
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

static std::vector<char> readFile(const std::string &filename) {
	std::ifstream file;
	file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);
	file.open(filename, std::ios::ate | std::ios::binary);
	std::size_t fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	return buffer;
}

void HelloTriangleApp::createGraphicsPipeline() {
	const auto vertShaderCode = readFile("shaders/vert.spv");
	const auto fragShaderCode = readFile("shaders/frag.spv");

	const auto vertShaderModule = createShaderModule(vertShaderCode);
	const auto fragShaderModule = createShaderModule(fragShaderCode);

	const vk::PipelineShaderStageCreateInfo shaderStages[] = {
			vk::PipelineShaderStageCreateInfo{
					{},
					vk::ShaderStageFlagBits::eVertex,
					*vertShaderModule,
					"main" },
			vk::PipelineShaderStageCreateInfo{
					{},
					vk::ShaderStageFlagBits::eFragment,
					*fragShaderModule,
					"main" }
	};
	const vk::PipelineVertexInputStateCreateInfo vertexInputInfo{{}, 0, nullptr, 0, nullptr };
	const vk::PipelineInputAssemblyStateCreateInfo inputAssembly{{}, vk::PrimitiveTopology::eTriangleList, false };
	const vk::Viewport viewport{
			0.0f,
			0.0f,
			static_cast<float>(std::any_cast<vk::Extent2D>(tmpVal[0]).width),
			static_cast<float>(std::any_cast<vk::Extent2D>(tmpVal[0]).height),
			0.0f,
			0.1f };
	const vk::Rect2D scissor{{ 0, 0 }, std::any_cast<vk::Extent2D>(tmpVal[0]) };

	const vk::PipelineViewportStateCreateInfo viewportState{{},
															1,
															&viewport,
															1,
															&scissor };
	const vk::PipelineRasterizationStateCreateInfo rasterizer{{},
															  false,
															  false,
															  vk::PolygonMode::eFill,
															  vk::CullModeFlagBits::eBack,
															  vk::FrontFace::eClockwise,
															  false,
															  0.0f,
															  0.0f,
															  0.0f,
															  1.0f };
	const vk::PipelineMultisampleStateCreateInfo multisampling{{},
															   vk::SampleCountFlagBits::e1,
															   false,
															   1.0f,
															   nullptr,
															   false,
															   false };
	const vk::PipelineColorBlendAttachmentState colorBlendAttachment{
			false,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
	const vk::PipelineColorBlendStateCreateInfo colorBlending{{}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment, { 0.0f, 0.0f, 0.0f, 0.0f }};
	const vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
	const vk::PipelineDynamicStateCreateInfo dynamicState{{}, 2, dynamicStates };
	const vk::PipelineLayoutCreateInfo pipelineLayoutInfo{{}, 0, nullptr, 0, nullptr };
	this->pipelineLayout = this->device->createPipelineLayoutUnique(pipelineLayoutInfo);

	const vk::GraphicsPipelineCreateInfo pipelineInfo{
			{}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
			nullptr, &colorBlending,
			nullptr, *pipelineLayout, *renderPass, 0 };
	this->pipeline = this->device->createGraphicsPipelineUnique({}, pipelineInfo).value;
}

vk::UniqueShaderModule HelloTriangleApp::createShaderModule(const std::vector<char> &code) {
	return this->device->createShaderModuleUnique(vk::ShaderModuleCreateInfo{
			{},
			code.size(),
			reinterpret_cast<const std::uint32_t *>(code.data())
	});
}

void HelloTriangleApp::createRenderPass() {
	const vk::AttachmentDescription colorAttachment{{},
													std::any_cast<vk::Format>(tmpVal[1]),
													vk::SampleCountFlagBits::e1,
													vk::AttachmentLoadOp::eClear,
													vk::AttachmentStoreOp::eStore,
													vk::AttachmentLoadOp::eDontCare,
													vk::AttachmentStoreOp::eDontCare,
													vk::ImageLayout::eUndefined,
													vk::ImageLayout::ePresentSrcKHR };
	constexpr vk::AttachmentReference colorAttachmentRef{ 0, vk::ImageLayout::eColorAttachmentOptimal };
	const vk::SubpassDescription subpass{{}, vk::PipelineBindPoint::eGraphics, {}, {}, 1, &colorAttachmentRef };
	constexpr vk::SubpassDependency dependency{
			VK_SUBPASS_EXTERNAL,
			0,
			{ vk::PipelineStageFlagBits::eColorAttachmentOutput },
			{ vk::PipelineStageFlagBits::eColorAttachmentOutput },
			{ vk::AccessFlagBits::eMemoryRead },
			{ vk::AccessFlagBits::eColorAttachmentWrite }
	};
	const vk::RenderPassCreateInfo renderPassInfo{{}, 1, &colorAttachment, 1, &subpass, 1, &dependency };
	this->renderPass = this->device->createRenderPassUnique(renderPassInfo);
}

void HelloTriangleApp::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());
	const auto extent = std::any_cast<vk::Extent2D>(tmpVal[0]);
	for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
		const vk::FramebufferCreateInfo framebufferInfo{{}, *renderPass, 1, &swapChainImageViews[i].get(), extent.width, extent.height, 1 };
		swapChainFramebuffers[i] = this->device->createFramebufferUnique(framebufferInfo);
	}
}

void HelloTriangleApp::createCommandPool() {
	const auto queueFamilyIndices = findQueueFamilies(this->physicalDevice);
	const vk::CommandPoolCreateInfo poolInfo{{}, queueFamilyIndices.graphicsFamily.value() };
	this->commandPool = this->device->createCommandPoolUnique(poolInfo);
}

void HelloTriangleApp::createCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());
	const vk::CommandBufferAllocateInfo allocateInfo{
			*commandPool,
			vk::CommandBufferLevel::ePrimary,
			static_cast<uint32_t>(commandBuffers.size()) };
	this->commandBuffers = this->device->allocateCommandBuffersUnique(allocateInfo);
	for (size_t i = 0; i < commandBuffers.size(); ++i) {
		{
			const vk::CommandBufferBeginInfo beginInfo;
			commandBuffers[i]->begin(beginInfo);
		}
		{
			const vk::ClearValue clearColor{ std::array{ 0.f, 0.f, 0.f, 1.f }};
			const vk::RenderPassBeginInfo renderPassInfo{
					*renderPass, *swapChainFramebuffers[i],
					{{ 0, 0 }, std::any_cast<vk::Extent2D>(tmpVal[0]) },
					1, &clearColor };
			commandBuffers[i]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
		}
		commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
		commandBuffers[i]->draw(3, 1, 0, 0);
		commandBuffers[i]->endRenderPass();
		commandBuffers[i]->end();
	}
}

void HelloTriangleApp::createSyncObjects() {
	constexpr vk::FenceCreateInfo fenceInfo{{ vk::FenceCreateFlagBits::eSignaled }};
	imagesInFlight.resize(swapChainImages.size());
	for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		this->imageAvailableSemaphores[i] = this->device->createSemaphoreUnique({});
		this->renderFinishedSemaphores[i] = this->device->createSemaphoreUnique({});
		this->inFlightFences[i] = this->device->createFenceUnique(fenceInfo);
	}
}

void HelloTriangleApp::recreateSwapChain() {
	glfwGetFramebufferSize(window, reinterpret_cast<int *>(&largeur), reinterpret_cast<int *>(&hauteur));
	while (largeur == 0 || hauteur == 0) {
		glfwGetFramebufferSize(window, reinterpret_cast<int *>(&largeur), reinterpret_cast<int *>(&hauteur));
		glfwWaitEvents();
	}

	this->device->waitIdle();

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void HelloTriangleApp::cleanupSwapChain() {
	for (auto &framebuffer : swapChainFramebuffers) {
//		device->destroyFramebuffer(framebuffer.release());
		framebuffer.reset(); // Équivalent du dessus
	}

	for (auto &command_buffer : commandBuffers) {
		command_buffer.reset();
	}

	pipeline.reset();
	pipelineLayout.reset();
	renderPass.reset();

	for (auto &image_view : swapChainImageViews) {
		image_view.reset();
	}

	this->device->destroy(oldSwpChain, {});
}