/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "genesis/core.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <shaderc/shaderc.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include <vulkan/vulkan.h>

#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_map>

using NamesVector = std::vector<const char*>;
using ExtPropVector = std::vector<VkExtensionProperties>;
using LayerPropVector = std::vector<VkLayerProperties>;
using PhysDeviceVector = std::vector<VkPhysicalDevice>;
using SurfFormatVector = std::vector<VkSurfaceFormatKHR>;
using PresentModeVactor = std::vector<VkPresentModeKHR>;
using ImageVector = std::vector<VkImage>;
using ImageViewVector = std::vector<VkImageView>;
using ShaderCodeVector = std::vector<uint32_t>;
using FramebufferVector = std::vector<VkFramebuffer>;
using CmdBuffVector = std::vector<VkCommandBuffer>;
using SemaphoreVector = std::vector<VkSemaphore>;
using FenceVector = std::vector<VkFence>;
using BufferVector = std::vector<VkBuffer>;
using DeviceMemVector = std::vector<VkDeviceMemory>;
using DescSetsVector = std::vector<VkDescriptorSet>;
using FormatVector = std::vector<VkFormat>;

namespace {

constexpr GE::Logger::Level LOG_LEVEL{GE::Logger::Level::TRACE};

constexpr int WINDOW_HEIGHT{800};
constexpr int WINDOW_WIDTH{600};

constexpr auto APP_NAME = "SDL2 Vulkan Example";
constexpr auto ENGINE_NAME = "No Engine";

constexpr int VULKAN_LOG_INFO{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};

constexpr auto SHADER_VERT_PATH = "examples/sdl2-vulkan/shaders/shader.vert";
constexpr auto SHADER_FRAG_PATH = "examples/sdl2-vulkan/shaders/shader.frag";
constexpr auto SHADER_ENTRYPOINT = "main";

constexpr auto VIKING_ROOM_MODEL_PATH = "examples/sdl2-vulkan/models/viking_room.obj";
constexpr auto VIKING_ROOM_TEXTURE_PATH = "examples/sdl2-vulkan/textures/viking_room.png";

constexpr size_t MAX_FRAMES_IN_FLIGHT{2};

constexpr int ALIGN_16{16};

struct swap_chain_support_details_t {
    VkSurfaceCapabilitiesKHR capabilities{};
    SurfFormatVector formats;
    PresentModeVactor present_mode;
};

struct Vertex {
    glm::vec3 pos{0.0f, 0.0f, 0.0f};
    glm::vec3 color{0.0f, 0.0f, 0.0f};
    glm::vec2 tex_coord{0.0f, 0.0f};

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);

        return attribute_descriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && tex_coord == other.tex_coord;
    }
};

struct ubo_mvp_t {
    alignas(ALIGN_16) glm::mat4 model;
    alignas(ALIGN_16) glm::mat4 view;
    alignas(ALIGN_16) glm::mat4 proj;
};

void printVkExtensions(const NamesVector& extensions)
{
    GE_DBG("SDL2 Vulkan Extensions ({}):", extensions.size());

    for (const char* extension : extensions) {
        GE_DBG("\t{}", extension);
    }
}

void printVkExtProperties(const ExtPropVector& extensions)
{
    GE_DBG("Vulkan Extensions ({}):", extensions.size());

    for (const auto& extension : extensions) {
        GE_DBG("\tName: {}, Version: {}", extension.extensionName, extension.specVersion);
    }
}

void printVkLayers(const LayerPropVector& layers)
{
    GE_DBG("Vulkan Layers ({}):", layers.size());

    for (const auto& layer : layers) {
        GE_DBG("\tName: {}, Version: {}, Description: '{}'", layer.layerName, layer.specVersion,
               layer.description);
    }
}

void printVkPhysicalDevices(const PhysDeviceVector& devices)
{
    GE_DBG("Physical devices ({})", devices.size());

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties device_properties{};
        vkGetPhysicalDeviceProperties(device, &device_properties);
        GE_DBG("\tName: {}", device_properties.deviceName);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data)
{
    const char* type_str = "Unknown";

    if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type_str = "Performance";
    } else if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type_str = "Validation";
    } else if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type_str = "General";
    }

    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        GE_ERR("[Vk {}]: {}", type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        GE_WARN("[Vk {}]: {}", type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        GE_INFO("[Vk {}]: {}", type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        GE_DBG("[Vk {}]: {}", type_str, callback_data->pMessage);
    } else {
        GE_CRIT("[Vk {}]: {}", type_str, callback_data->pMessage);
    }

    return VK_FALSE;
}

template<typename FuncPFN, typename... Args>
VkResult loadInstanceFuncAndCall(const char* func_name, VkInstance instance, Args&&... args)
{
    auto* void_func = vkGetInstanceProcAddr(instance, func_name);

    if (void_func == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    auto* func = reinterpret_cast<FuncPFN>(void_func);
    using FuncRet = std::invoke_result_t<FuncPFN, VkInstance, Args...>;

    if constexpr (std::is_void_v<FuncRet>) {
        func(instance, std::forward<Args>(args)...);
    } else {
        return func(instance, std::forward<Args>(args)...);
    }

    return VK_SUCCESS;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                      const VkAllocationCallbacks* allocator,
                                      VkDebugUtilsMessengerEXT* debug_messenger)
{
    return loadInstanceFuncAndCall<PFN_vkCreateDebugUtilsMessengerEXT>(
        "vkCreateDebugUtilsMessengerEXT", instance, create_info, allocator, debug_messenger);
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger,
                                   const VkAllocationCallbacks* allocator)
{
    loadInstanceFuncAndCall<PFN_vkDestroyDebugUtilsMessengerEXT>(
        "vkDestroyDebugUtilsMessengerEXT", instance, debug_messenger, allocator);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const SurfFormatVector& formats)
{
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const PresentModeVactor& present_modes)
{
    for (const auto& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

ShaderCodeVector compileShader(const std::string& filename, shaderc_shader_kind shader_kind)
{
    std::ifstream fin{filename};

    if (!fin) {
        GE_ERR("No such file: '{}'", filename);
        return {};
    }

    std::string source(std::istreambuf_iterator<char>{fin}, std::istreambuf_iterator<char>{});
    shaderc::Compiler compiler;
    auto result = compiler.CompileGlslToSpv(source, shader_kind, filename.c_str());

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        GE_ERR("Failed to compile '{}'", filename);
        GE_ERR("{}", result.GetErrorMessage());
        return {};
    }

    return {result.begin(), result.end()};
}

ExtPropVector getInstanceExtensions()
{
    uint32_t extension_count{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    ExtPropVector extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    return extensions;
}

ExtPropVector getDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t extensions_count{0};
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, nullptr);

    ExtPropVector extensions(extensions_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensions_count, extensions.data());

    return extensions;
}

bool hasStencilComponent(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_S8_UINT: return true;
        default: return false;
    }
}

struct queue_family_indices_t {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
};

} // namespace

namespace std {

template<>
struct hash<Vertex> {
    size_t operator()(const Vertex& vertex) const
    {
        size_t hash_pos = std::hash<glm::vec3>{}(vertex.pos);
        size_t hash_color = std::hash<glm::vec3>{}(vertex.color);
        size_t hash_tex_coord = std::hash<glm::vec2>{}(vertex.tex_coord);
        return ((hash_pos ^ hash_color << 1) >> 1) ^ hash_tex_coord << 1;
    }
};

} // namespace std

namespace {

/**
 * HelloTriangleApplication is based on https://github.com/Overv/VulkanTutorial
 */
class HelloTriangleApplication
{
public:
    HelloTriangleApplication();
    void run();

private:
    void initializeWindow();
    void initializeVulkan();
    void mainLoop();

    void pollEvents();
    void handleWindowEvents(SDL_WindowEvent event);

    void drawFrame();
    void recreateSwapChain();
    void updateUniformBuffer(uint32_t current_image);

    VkCommandBuffer beginSingleTimeCommand();
    void endSingleTimeCommand(VkCommandBuffer command_buffer);

    void cleanup();
    void cleanupSwapChain();

    void createVkInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createColorResources();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void loadModel();
    void createVertexBuffers();
    void createIndexBuffers();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    bool checkValidationLayerSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
    queue_family_indices_t findQueueFamilies(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
    VkSampleCountFlagBits getMaxUsableSampleCount();

    std::pair<VkBuffer, VkDeviceMemory> createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                                     VkMemoryPropertyFlags properties);
    void copyBuffer(VkBuffer dst, VkBuffer src, VkDeviceSize size);

    std::pair<VkImage, VkDeviceMemory> createImage(glm::uvec2 size, uint32_t mip_levels,
                                                   VkSampleCountFlagBits num_samples,
                                                   VkFormat format, VkImageTiling tiling,
                                                   VkImageUsageFlags usage,
                                                   VkMemoryPropertyFlags properties);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags,
                                uint32_t mip_levels);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout,
                               VkImageLayout new_layout, uint32_t mip_levels);
    void copyBufferToImage(VkImage image, VkBuffer buffer, glm::uvec2 size);
    void createMipmaps(VkImage image, VkFormat image_format, const glm::uvec2& size,
                       uint32_t mip_levels);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkShaderModule createShaderModule(const ShaderCodeVector& code);

    VkDebugUtilsMessengerCreateInfoEXT getDebugMsgrCreateInfo() const;
    NamesVector getRequiredExtensions();
    swap_chain_support_details_t querySwapChainSupport(VkPhysicalDevice device);
    VkFormat findSupportedFormat(const FormatVector& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    bool m_is_closed{false};

    SDL_Window* m_window{nullptr};
    VkInstance m_vk_instance{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    VkQueue m_graphics_queue{VK_NULL_HANDLE};
    VkQueue m_present_queue{VK_NULL_HANDLE};

    VkDebugUtilsMessageSeverityFlagsEXT m_debug_severity{VULKAN_LOG_INFO};
    VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};

    VkSwapchainKHR m_swap_chain{VK_NULL_HANDLE};
    ImageVector m_swap_chain_images;
    ImageViewVector m_swap_chain_image_views;
    FramebufferVector m_swap_chain_framebuffers;
    VkFormat m_swap_chain_image_format{};
    VkExtent2D m_swap_chain_extent{};

    VkRenderPass m_render_pass{VK_NULL_HANDLE};
    VkDescriptorSetLayout m_descriptor_set_layout{VK_NULL_HANDLE};
    VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
    VkPipeline m_graphics_pipeline{VK_NULL_HANDLE};

    VkCommandPool m_command_pool{VK_NULL_HANDLE};
    CmdBuffVector m_command_buffers{VK_NULL_HANDLE};

    SemaphoreVector m_image_available_semaphores;
    SemaphoreVector m_render_finished_semaphores;
    FenceVector m_in_flight_fences;
    FenceVector m_images_in_flight;
    size_t m_current_frame{0};

    VkBuffer m_vertex_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_vertex_buffer_memory{VK_NULL_HANDLE};
    VkBuffer m_index_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_index_buffer_memory{VK_NULL_HANDLE};
    BufferVector m_uniform_buffers;
    DeviceMemVector m_uniform_buffers_memory;
    VkDescriptorPool m_descriptor_pool{VK_NULL_HANDLE};
    DescSetsVector m_descriptor_sets;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    bool m_framebuffer_resized{false};

    uint32_t m_mip_levels{};
    VkImage m_texture_image{VK_NULL_HANDLE};
    VkDeviceMemory m_texture_image_memory{VK_NULL_HANDLE};
    VkImageView m_texture_image_view{VK_NULL_HANDLE};
    VkSampler m_texture_sampler{VK_NULL_HANDLE};
    VkSampleCountFlagBits m_msaa_samples{VK_SAMPLE_COUNT_1_BIT};

    VkImage m_color_image{VK_NULL_HANDLE};
    VkDeviceMemory m_color_image_memory{VK_NULL_HANDLE};
    VkImageView m_color_image_view{VK_NULL_HANDLE};

    VkImage m_depth_image{VK_NULL_HANDLE};
    VkDeviceMemory m_depth_image_memory{VK_NULL_HANDLE};
    VkImageView m_depth_image_view{VK_NULL_HANDLE};

    NamesVector m_validation_layers;
    NamesVector m_device_extensions;
};

HelloTriangleApplication::HelloTriangleApplication()
{
    m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
    m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

void HelloTriangleApplication::run()
{
    initializeWindow();
    initializeVulkan();
    mainLoop();
    cleanup();
}

void HelloTriangleApplication::initializeWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error{SDL_GetError()};
    }

    auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;

    m_window = SDL_CreateWindow("SDL2 Vulkan Example", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WINDOW_HEIGHT, WINDOW_WIDTH, flags);

    if (m_window == nullptr) {
        throw std::runtime_error{SDL_GetError()};
    }
}

void HelloTriangleApplication::initializeVulkan()
{
    createVkInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createColorResources();
    createDepthResources();
    createFramebuffers();
    createCommandPool();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    loadModel();
    createVertexBuffers();
    createIndexBuffers();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void HelloTriangleApplication::mainLoop()
{
    while (!m_is_closed) {
        pollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_device);
}

void HelloTriangleApplication::pollEvents()
{
    SDL_Event event{};
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT: m_is_closed = true; break;
        case SDL_WINDOWEVENT: handleWindowEvents(event.window); break;
        default: break;
    }
}

void HelloTriangleApplication::handleWindowEvents(SDL_WindowEvent event)
{
    switch (event.event) {
        case SDL_WINDOWEVENT_CLOSE: m_is_closed = true; break;
        case SDL_WINDOWEVENT_RESIZED: m_framebuffer_resized = true; break;
        default: break;
    }
}

void HelloTriangleApplication::drawFrame()
{
    static constexpr uint64_t timeout_none = std::numeric_limits<uint64_t>::max();
    uint32_t image_idx{};

    vkWaitForFences(m_device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, timeout_none);

    auto result = vkAcquireNextImageKHR(m_device, m_swap_chain, timeout_none,
                                        m_image_available_semaphores[m_current_frame],
                                        VK_NULL_HANDLE, &image_idx);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error{"Failed to aquire swap chain image!"};
    }

    if (m_images_in_flight[image_idx] != VK_NULL_HANDLE) {
        vkWaitForFences(m_device, 1, &m_images_in_flight[image_idx], VK_TRUE, timeout_none);
    }

    m_images_in_flight[image_idx] = m_in_flight_fences[m_current_frame];

    updateUniformBuffer(image_idx);

    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitDstStageMask = &wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[image_idx];
    submit_info.pSignalSemaphores = &m_render_finished_semaphores[m_current_frame];
    submit_info.signalSemaphoreCount = 1;

    vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);

    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]) !=
        VK_SUCCESS) {
        throw std::runtime_error{"Failed to submit Draw Command Buffer!"};
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pWaitSemaphores = &m_render_finished_semaphores[m_current_frame];
    present_info.waitSemaphoreCount = 1;
    present_info.pSwapchains = &m_swap_chain;
    present_info.swapchainCount = 1;
    present_info.pImageIndices = &image_idx;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(m_present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_framebuffer_resized) {
        m_framebuffer_resized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error{"Failed to present Swap Chain Image!"};
    }

    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::recreateSwapChain()
{
    int width{};
    int height{};

    SDL_GetWindowSize(m_window, &width, &height);

    if (width == 0 || height == 0) {
        return;
    }

    vkDeviceWaitIdle(m_device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createColorResources();
    createDepthResources();
    createFramebuffers();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t current_image)
{
    static auto start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();

    using Sec = std::chrono::seconds::period;
    float time = std::chrono::duration<float, Sec>(current_time - start_time).count();

    float rotation_speed = time * glm::radians(90.0f);
    float aspect_ratio = static_cast<float>(m_swap_chain_extent.width) / m_swap_chain_extent.height;

    ubo_mvp_t ubo{};
    ubo.model = glm::rotate(glm::mat4{1.0f}, rotation_speed, {0.0f, 0.0f, 1.0f});
    ubo.view = glm::lookAt(glm::vec3{2.0f, 2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                           glm::vec3{0.0f, 0.0f, 1.0f});
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10.0f);

    // Flip Y
    ubo.proj[1][1] *= -1;

    static constexpr size_t ubo_size = sizeof(ubo_mvp_t);
    void* data{nullptr};
    vkMapMemory(m_device, m_uniform_buffers_memory[current_image], 0, ubo_size, 0, &data);
    std::memcpy(data, &ubo, ubo_size);
    vkUnmapMemory(m_device, m_uniform_buffers_memory[current_image]);
}

VkCommandBuffer HelloTriangleApplication::beginSingleTimeCommand()
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    return command_buffer;
}

void HelloTriangleApplication::endSingleTimeCommand(VkCommandBuffer command_buffer)
{
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.commandBufferCount = 1;

    vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphics_queue);

    vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
}

void HelloTriangleApplication::cleanup()
{
    // Vulkan
    cleanupSwapChain();

    vkDestroySampler(m_device, m_texture_sampler, nullptr);
    vkDestroyImageView(m_device, m_texture_image_view, nullptr);
    vkDestroyImage(m_device, m_texture_image, nullptr);
    vkFreeMemory(m_device, m_texture_image_memory, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, nullptr);
    vkDestroyBuffer(m_device, m_index_buffer, nullptr);
    vkFreeMemory(m_device, m_index_buffer_memory, nullptr);
    vkDestroyBuffer(m_device, m_vertex_buffer, nullptr);
    vkFreeMemory(m_device, m_vertex_buffer_memory, nullptr);

    for (size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(m_device, m_in_flight_fences[i], nullptr);
        vkDestroySemaphore(m_device, m_image_available_semaphores[i], nullptr);
        vkDestroySemaphore(m_device, m_render_finished_semaphores[i], nullptr);
    }

    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
    destroyDebugUtilsMessengerEXT(m_vk_instance, m_debug_messenger, nullptr);
    vkDestroyInstance(m_vk_instance, nullptr);

    // Window
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void HelloTriangleApplication::cleanupSwapChain()
{
    vkDestroyImageView(m_device, m_color_image_view, nullptr);
    vkDestroyImage(m_device, m_color_image, nullptr);
    vkFreeMemory(m_device, m_color_image_memory, nullptr);

    vkDestroyImageView(m_device, m_depth_image_view, nullptr);
    vkDestroyImage(m_device, m_depth_image, nullptr);
    vkFreeMemory(m_device, m_depth_image_memory, nullptr);

    for (const auto& framebuffer : m_swap_chain_framebuffers) {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(m_device, m_command_pool, m_command_buffers.size(),
                         m_command_buffers.data());

    vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);

    for (const auto& image_view : m_swap_chain_image_views) {
        vkDestroyImageView(m_device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);

    for (size_t i{0}; i < m_swap_chain_images.size(); i++) {
        vkDestroyBuffer(m_device, m_uniform_buffers[i], nullptr);
        vkFreeMemory(m_device, m_uniform_buffers_memory[i], nullptr);
    }

    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
}

void HelloTriangleApplication::createVkInstance()
{
    if (!checkValidationLayerSupport()) {
        throw std::runtime_error{"Validation layers are not available!"};
    }

    auto sdl_extensions = getRequiredExtensions();
    printVkExtensions(sdl_extensions);

    auto vk_extensions = getInstanceExtensions();
    printVkExtProperties(vk_extensions);

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = APP_NAME;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    auto debug_create_info = getDebugMsgrCreateInfo();
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(sdl_extensions.size());
    create_info.ppEnabledExtensionNames = sdl_extensions.data();
    create_info.ppEnabledLayerNames = m_validation_layers.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
    create_info.pNext = &debug_create_info;

#ifdef GE_PLATFORM_APPLE
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif // GE_PLATFORM_APPLE

    if (vkCreateInstance(&create_info, nullptr, &m_vk_instance) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Vulkan Instance!"};
    }
}

void HelloTriangleApplication::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT create_info = getDebugMsgrCreateInfo();

    if (createDebugUtilsMessengerEXT(m_vk_instance, &create_info, nullptr, &m_debug_messenger) !=
        VK_SUCCESS) {
        throw std::runtime_error{"Failed to set up debug messenger!"};
    }
}

void HelloTriangleApplication::createSurface()
{
    if (SDL_Vulkan_CreateSurface(m_window, m_vk_instance, &m_surface) == SDL_FALSE) {
        throw std::runtime_error{SDL_GetError()};
    }
}

void HelloTriangleApplication::pickPhysicalDevice()
{
    uint32_t device_count{0};
    vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);

    if (device_count == 0) {
        throw std::runtime_error{"Failed to find GPUs with Vulkan support!"};
    }

    PhysDeviceVector devices(device_count);
    vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());
    printVkPhysicalDevices(devices);

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            m_physical_device = device;
            m_msaa_samples = getMaxUsableSampleCount();

            VkPhysicalDeviceProperties device_properties{};
            vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
            GE_INFO("'{}' has been chosen as GPU, MSAA: {}", device_properties.deviceName,
                    GE::toString(m_msaa_samples));

            break;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error{"Failed tot find suitable GPU!"};
    }
}

void HelloTriangleApplication::createLogicalDevice()
{
    auto indices = findQueueFamilies(m_physical_device);
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(),
                                                indices.present_family.value()};

    float queue_priorities{1.0f};

    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priorities;

        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pEnabledFeatures = &device_features;
    create_info.ppEnabledExtensionNames = m_device_extensions.data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Logical Device!"};
    }

    vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
}

void HelloTriangleApplication::createSwapChain()
{
    auto swap_chain_support = querySwapChainSupport(m_physical_device);
    auto surface_format = chooseSwapSurfaceFormat(swap_chain_support.formats);
    auto present_mode = chooseSwapPresentMode(swap_chain_support.present_mode);
    auto extent = chooseSwapExtent(swap_chain_support.capabilities);
    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount) {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    auto indices = findQueueFamilies(m_physical_device);
    std::array<uint32_t, 2> queue_family_indices = {indices.graphics_family.value(),
                                                    indices.present_family.value()};

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.pQueueFamilyIndices = queue_family_indices.data();
        create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Swap Chain!"};
    }

    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
    m_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

    m_swap_chain_image_format = surface_format.format;
    m_swap_chain_extent = extent;
}

void HelloTriangleApplication::createImageViews()
{
    m_swap_chain_image_views.resize(m_swap_chain_images.size());

    for (size_t i{0}; i < m_swap_chain_images.size(); i++) {
        m_swap_chain_image_views[i] = createImageView(
            m_swap_chain_images[i], m_swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

void HelloTriangleApplication::createRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_swap_chain_image_format;
    color_attachment.samples = m_msaa_samples;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = findDepthFormat();
    depth_attachment.samples = m_msaa_samples;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription color_attachment_resolve{};
    color_attachment_resolve.format = m_swap_chain_image_format;
    color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_resolve_ref{};
    color_attachment_resolve_ref.attachment = 2;
    color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.colorAttachmentCount = 1;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;
    subpass.pResolveAttachments = &color_attachment_resolve_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = {color_attachment, depth_attachment,
                                                          color_attachment_resolve};

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.subpassCount = 1;
    render_pass_info.pDependencies = &dependency;
    render_pass_info.dependencyCount = 1;

    if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Render Pass!"};
    }
}

void HelloTriangleApplication::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding mvp_layout_binding{};
    mvp_layout_binding.binding = 0;
    mvp_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    mvp_layout_binding.descriptorCount = 1;
    mvp_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    mvp_layout_binding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sampler_layout_bindings{};
    sampler_layout_bindings.binding = 1;
    sampler_layout_bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_bindings.descriptorCount = 1;
    sampler_layout_bindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    sampler_layout_bindings.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {mvp_layout_binding,
                                                            sampler_layout_bindings};

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.pBindings = bindings.data();
    layout_info.bindingCount = bindings.size();

    if (vkCreateDescriptorSetLayout(m_device, &layout_info, nullptr, &m_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Descriptor Set Layout!"};
    }
}

void HelloTriangleApplication::createGraphicsPipeline()
{
    auto vert_shader_code = compileShader(SHADER_VERT_PATH, shaderc_glsl_vertex_shader);
    auto frag_shader_code = compileShader(SHADER_FRAG_PATH, shaderc_glsl_fragment_shader);

    if (vert_shader_code.empty() || frag_shader_code.empty()) {
        throw std::runtime_error{"Failed to read shaders!"};
    }

    VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
    VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = SHADER_ENTRYPOINT;

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = frag_shader_module;
    frag_shader_stage_info.pName = SHADER_ENTRYPOINT;

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {vert_shader_stage_info,
                                                                    frag_shader_stage_info};
    auto binding_description = Vertex::getBindingDescription();
    auto attribute_descriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state{};
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.pVertexBindingDescriptions = &binding_description;
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexAttributeDescriptions = attribute_descriptions.data();
    vertex_input_state.vertexAttributeDescriptionCount = attribute_descriptions.size();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state{};
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swap_chain_extent.width);
    viewport.height = static_cast<float>(m_swap_chain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {};
    scissor.extent = m_swap_chain_extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pViewports = &viewport;
    viewport_state.viewportCount = 1;
    viewport_state.pScissors = &scissor;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization_state{};
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.lineWidth = 1.0f;
    rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisample_state{};
    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.sampleShadingEnable = VK_TRUE;
    multisample_state.minSampleShading = 0.2f;
    multisample_state.rasterizationSamples = m_msaa_samples;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.pAttachments = &color_blend_attachment;
    color_blend_state.attachmentCount = 1;
    color_blend_state.blendConstants[0] = 0.0f;
    color_blend_state.blendConstants[1] = 0.0f;
    color_blend_state.blendConstants[2] = 0.0f;
    color_blend_state.blendConstants[3] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depth_stenil_state{};
    depth_stenil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stenil_state.depthTestEnable = VK_TRUE;
    depth_stenil_state.depthWriteEnable = VK_TRUE;
    depth_stenil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stenil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stenil_state.stencilTestEnable = VK_FALSE;

    // std::array<VkDynamicState, 2> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
    //                                                 VK_DYNAMIC_STATE_LINE_WIDTH};

    // VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    // dynamic_state_info.sType =
    // VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // dynamic_state_info.pDynamicStates = dynamic_states.data();
    // dynamic_state_info.dynamicStateCount = dynamic_states.size();

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pSetLayouts = &m_descriptor_set_layout;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Pipeline Layout!"};
    }

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pVertexInputState = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &input_assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pMultisampleState = &multisample_state;
    pipeline_info.pDepthStencilState = &depth_stenil_state;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = nullptr;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = m_render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                  &m_graphics_pipeline) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Graphics Pipeline!"};
    }

    vkDestroyShaderModule(m_device, vert_shader_module, nullptr);
    vkDestroyShaderModule(m_device, frag_shader_module, nullptr);
}

void HelloTriangleApplication::createColorResources()
{
    VkFormat color_format = m_swap_chain_image_format;
    VkImageUsageFlags usage =
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::tie(m_color_image, m_color_image_memory) = createImage(
        {m_swap_chain_extent.width, m_swap_chain_extent.height}, 1, m_msaa_samples, color_format,
        VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_color_image_view = createImageView(m_color_image, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void HelloTriangleApplication::createDepthResources()
{
    VkFormat depth_format = findDepthFormat();

    std::tie(m_depth_image, m_depth_image_memory) = createImage(
        {m_swap_chain_extent.width, m_swap_chain_extent.height}, 1, m_msaa_samples, depth_format,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_depth_image_view = createImageView(m_depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

void HelloTriangleApplication::createFramebuffers()
{
    m_swap_chain_framebuffers.resize(m_swap_chain_image_views.size());

    for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            m_color_image_view,
            m_depth_image_view,
            m_swap_chain_image_views[i],
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_render_pass;
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.attachmentCount = attachments.size();
        framebuffer_info.width = m_swap_chain_extent.width;
        framebuffer_info.height = m_swap_chain_extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebuffer_info, nullptr,
                                &m_swap_chain_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to create Framebuffer!"};
        }
    }
}

void HelloTriangleApplication::createCommandPool()
{
    queue_family_indices_t queue_family_indices = findQueueFamilies(m_physical_device);

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    create_info.flags = 0;

    if (vkCreateCommandPool(m_device, &create_info, nullptr, &m_command_pool) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Command Pool!"};
    }
}

void HelloTriangleApplication::createTextureImage()
{
    int width{0};
    int height{0};
    int channels{0};
    stbi_uc* pixels =
        stbi_load(VIKING_ROOM_TEXTURE_PATH, &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize image_size = width * height * 4;

    if (pixels == nullptr) {
        throw std::runtime_error{"Failed to load texture image!"};
    }

    m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    VkBufferUsageFlags stagig_buffer_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags staging_buffer_props =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    auto [staging_buffer, staging_buffer_memory] =
        createBuffer(image_size, stagig_buffer_usage, staging_buffer_props);

    void* data{nullptr};
    vkMapMemory(m_device, staging_buffer_memory, 0, image_size, 0, &data);
    std::memcpy(data, pixels, image_size);
    vkUnmapMemory(m_device, staging_buffer_memory);

    stbi_image_free(pixels);

    VkFormat format{VK_FORMAT_R8G8B8A8_SRGB};
    VkImageTiling tiling{VK_IMAGE_TILING_OPTIMAL};
    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VkMemoryPropertyFlags mem_props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    std::tie(m_texture_image, m_texture_image_memory) =
        createImage({width, height}, m_mip_levels, VK_SAMPLE_COUNT_1_BIT, format, tiling,
                    image_usage, mem_props);

    transitionImageLayout(m_texture_image, format, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mip_levels);
    copyBufferToImage(m_texture_image, staging_buffer, {width, height});

    vkDestroyBuffer(m_device, staging_buffer, nullptr);
    vkFreeMemory(m_device, staging_buffer_memory, nullptr);

    createMipmaps(m_texture_image, format, {width, height}, m_mip_levels);
}

void HelloTriangleApplication::createTextureImageView()
{
    m_texture_image_view = createImageView(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                                           VK_IMAGE_ASPECT_COLOR_BIT, m_mip_levels);
}

void HelloTriangleApplication::createTextureSampler()
{
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16.0f;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = m_mip_levels;

    if (vkCreateSampler(m_device, &sampler_info, nullptr, &m_texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Texture Sampler!"};
    }
}

void HelloTriangleApplication::loadModel()
{
    tinyobj::attrib_t attrib{};
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, VIKING_ROOM_MODEL_PATH)) {
        throw std::runtime_error{warn + err};
    }

    std::unordered_map<Vertex, uint32_t> unique_vertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {attrib.vertices[(3 * index.vertex_index) + 0],
                          attrib.vertices[(3 * index.vertex_index) + 1],
                          attrib.vertices[(3 * index.vertex_index) + 2]};
            vertex.tex_coord = {attrib.texcoords[(2 * index.texcoord_index) + 0],
                                1.0 - attrib.texcoords[(2 * index.texcoord_index) + 1]};
            vertex.color = {1.0f, 1.0f, 1.0f};

            if (!unique_vertices.contains(vertex)) {
                unique_vertices[vertex] = m_vertices.size();
                m_vertices.push_back(vertex);
            }

            m_indices.push_back(unique_vertices[vertex]);
        }
    }
}

void HelloTriangleApplication::createVertexBuffers()
{
    VkDeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();
    VkMemoryPropertyFlags staging_buff_props =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    auto [staging_buffer, staging_buffer_memory] =
        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buff_props);

    void* data{nullptr};
    vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    std::memcpy(data, m_vertices.data(), buffer_size);
    vkUnmapMemory(m_device, staging_buffer_memory);

    VkBufferUsageFlags vert_buff_usage =
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    std::tie(m_vertex_buffer, m_vertex_buffer_memory) =
        createBuffer(buffer_size, vert_buff_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copyBuffer(m_vertex_buffer, staging_buffer, buffer_size);

    vkDestroyBuffer(m_device, staging_buffer, nullptr);
    vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void HelloTriangleApplication::createIndexBuffers()
{
    VkDeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();
    VkMemoryPropertyFlags staging_buff_props =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    auto [staging_buffer, staging_buffer_memory] =
        createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buff_props);

    void* data{nullptr};
    vkMapMemory(m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
    std::memcpy(data, m_indices.data(), buffer_size);
    vkUnmapMemory(m_device, staging_buffer_memory);

    VkBufferUsageFlags vert_buff_usage =
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    std::tie(m_index_buffer, m_index_buffer_memory) =
        createBuffer(buffer_size, vert_buff_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copyBuffer(m_index_buffer, staging_buffer, buffer_size);

    vkDestroyBuffer(m_device, staging_buffer, nullptr);
    vkFreeMemory(m_device, staging_buffer_memory, nullptr);
}

void HelloTriangleApplication::createUniformBuffers()
{
    VkDeviceSize buffer_size = sizeof(ubo_mvp_t);

    m_uniform_buffers.resize(m_swap_chain_images.size());
    m_uniform_buffers_memory.resize(m_swap_chain_images.size());

    VkMemoryPropertyFlags properties =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i{0}; i < m_swap_chain_images.size(); i++) {
        std::tie(m_uniform_buffers[i], m_uniform_buffers_memory[i]) =
            createBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, properties);
    }
}

void HelloTriangleApplication::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> pool_sizes{};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = m_swap_chain_images.size();
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = m_swap_chain_images.size();

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.maxSets = m_swap_chain_images.size();

    if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Descriptor Pool"};
    }
}

void HelloTriangleApplication::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(m_swap_chain_images.size(), m_descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_descriptor_pool;
    alloc_info.descriptorSetCount = m_swap_chain_images.size();
    alloc_info.pSetLayouts = layouts.data();

    m_descriptor_sets.resize(m_swap_chain_images.size());

    if (vkAllocateDescriptorSets(m_device, &alloc_info, m_descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to allocate Descriptor Sets"};
    }

    for (size_t i{0}; i < m_swap_chain_images.size(); i++) {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = m_uniform_buffers[i];
        buffer_info.offset = 0;
        buffer_info.range = sizeof(ubo_mvp_t);

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = m_texture_image_view;
        image_info.sampler = m_texture_sampler;

        std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = m_descriptor_sets[i];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &buffer_info;
        descriptor_writes[0].pImageInfo = nullptr;
        descriptor_writes[0].pTexelBufferView = nullptr;

        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = m_descriptor_sets[i];
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pBufferInfo = nullptr;
        descriptor_writes[1].pImageInfo = &image_info;
        descriptor_writes[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_device, descriptor_writes.size(), descriptor_writes.data(), 0,
                               nullptr);
    }
}

void HelloTriangleApplication::createCommandBuffers()
{
    m_command_buffers.resize(m_swap_chain_framebuffers.size());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = m_command_buffers.size();

    if (vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to allocate Command Buffers!"};
    }

    for (size_t i{0}; i < m_command_buffers.size(); i++) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to begin recording Command Buffer!"};
        }

        std::array<VkClearValue, 2> clear_values{};
        std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        std::ranges::copy(clear_color, clear_values[0].color.float32);
        clear_values[1].depthStencil = {.depth = 1.0f, .stencil = 0};

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_render_pass;
        render_pass_info.framebuffer = m_swap_chain_framebuffers[i];
        render_pass_info.renderArea.offset = {};
        render_pass_info.renderArea.extent = m_swap_chain_extent;
        render_pass_info.pClearValues = clear_values.data();
        render_pass_info.clearValueCount = clear_values.size();

        std::array<VkBuffer, 1> vertex_buffers = {m_vertex_buffer};
        std::array<VkDeviceSize, 1> offsets = {0};

        vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_graphics_pipeline);
        vkCmdBindVertexBuffers(m_command_buffers[i], 0, 1, vertex_buffers.data(), offsets.data());
        vkCmdBindIndexBuffer(m_command_buffers[i], m_index_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline_layout, 0, 1, &m_descriptor_sets[i], 0, nullptr);
        vkCmdDrawIndexed(m_command_buffers[i], m_indices.size(), 1, 0, 0, 0);

        vkCmdEndRenderPass(m_command_buffers[i]);

        if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to record Command Buffer"};
        }
    }
}

void HelloTriangleApplication::createSyncObjects()
{
    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    m_images_in_flight.resize(m_swap_chain_images.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr,
                              &m_image_available_semaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to create an image available semaphore!"};
        }

        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr,
                              &m_render_finished_semaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to create a render finished semaphore!"};
        }

        if (vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS) {
            throw std::runtime_error{"Failed to create an in flight fence!"};
        }
    }
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
    uint32_t layer_count{0};
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    LayerPropVector layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
    printVkLayers(layers);

    std::set<std::string> required_layers{m_validation_layers.begin(), m_validation_layers.end()};

    for (const auto& layer : layers) {
        required_layers.erase(layer.layerName);
    }

    return required_layers.empty();
}

bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device)
{
    auto indices = findQueueFamilies(device);
    bool is_ext_supported = checkDeviceExtensionsSupport(device);
    bool is_swap_chain_valid{false};

    if (is_ext_supported) {
        auto swap_chain_support = querySwapChainSupport(device);
        is_swap_chain_valid =
            !swap_chain_support.formats.empty() && !swap_chain_support.present_mode.empty();
    }

    VkPhysicalDeviceFeatures supported_features{};
    vkGetPhysicalDeviceFeatures(device, &supported_features);
    bool has_anisotropy = supported_features.samplerAnisotropy == VK_TRUE;

    return indices.isComplete() && is_ext_supported && is_swap_chain_valid && has_anisotropy;
}

bool HelloTriangleApplication::checkDeviceExtensionsSupport(VkPhysicalDevice device)
{
    auto extension_properties = getDeviceExtensions(device);
    std::set<std::string> required_extensions{m_device_extensions.begin(),
                                              m_device_extensions.end()};

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

queue_family_indices_t HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
    queue_family_indices_t indices{};

    uint32_t queue_family_count{0};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i{0}; i < queue_families.size(); i++) {
        if ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            indices.graphics_family = i;
        }

        VkBool32 is_present_supported{VK_FALSE};
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &is_present_supported);

        if (is_present_supported == VK_TRUE) {
            indices.present_family = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

uint32_t HelloTriangleApplication::findMemoryType(uint32_t type_filter,
                                                  VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties{};
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);

    for (uint32_t i{0}; i < mem_properties.memoryTypeCount; i++) {
        uint32_t mem_type = 1 << i;
        bool is_type_suitable = (type_filter & mem_type) != 0;
        bool is_properties_suitable =
            (mem_properties.memoryTypes[i].propertyFlags & properties) != 0;

        if (is_type_suitable && is_properties_suitable) {
            return i;
        }
    }

    throw std::runtime_error{"Failed to find suitable memory type!"};
}

VkSampleCountFlagBits HelloTriangleApplication::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physical_device_properties{};
    vkGetPhysicalDeviceProperties(m_physical_device, &physical_device_properties);

    auto device_counts = physical_device_properties.limits.framebufferColorSampleCounts &
                         physical_device_properties.limits.framebufferDepthSampleCounts;

    static constexpr std::array<VkSampleCountFlagBits, 6> expected_sample_counts = {
        VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,  VK_SAMPLE_COUNT_4_BIT,  VK_SAMPLE_COUNT_2_BIT};

    for (auto expected_count : expected_sample_counts) {
        if ((device_counts & expected_count) != 0) {
            return expected_count;
        }
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

std::pair<VkBuffer, VkDeviceMemory>
HelloTriangleApplication::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                       VkMemoryPropertyFlags properties)
{
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory buffer_memory{VK_NULL_HANDLE};

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Vertex Buffer"};
    }

    VkMemoryRequirements mem_requirements{};
    vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);

    uint32_t mem_type_idx = findMemoryType(mem_requirements.memoryTypeBits, properties);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mem_type_idx;

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to allocate Vertex Buffer Memory!"};
    }

    vkBindBufferMemory(m_device, buffer, buffer_memory, 0);

    return {buffer, buffer_memory};
}

void HelloTriangleApplication::copyBuffer(VkBuffer dst, VkBuffer src, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = beginSingleTimeCommand();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy_region);

    endSingleTimeCommand(command_buffer);
}

std::pair<VkImage, VkDeviceMemory> HelloTriangleApplication::createImage(
    glm::uvec2 size, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkFormat format,
    VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkImage image{VK_NULL_HANDLE};
    VkDeviceMemory image_memory{VK_NULL_HANDLE};

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = size.x;
    image_info.extent.height = size.y;
    image_info.extent.depth = 1;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = num_samples;
    image_info.flags = 0;

    if (vkCreateImage(m_device, &image_info, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Image!"};
    }

    VkMemoryRequirements mem_requirements{};
    vkGetImageMemoryRequirements(m_device, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to allocate Image Memory!"};
    }

    vkBindImageMemory(m_device, image, image_memory, 0);

    return {image, image_memory};
}

VkImageView HelloTriangleApplication::createImageView(VkImage image, VkFormat format,
                                                      VkImageAspectFlags aspect_flags,
                                                      uint32_t mip_levels)
{
    VkImageView image_view{VK_NULL_HANDLE};

    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = aspect_flags;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = mip_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Texture Image View!"};
    }

    return image_view;
}

void HelloTriangleApplication::transitionImageLayout(VkImage image, VkFormat format,
                                                     VkImageLayout old_layout,
                                                     VkImageLayout new_layout, uint32_t mip_levels)
{
    VkCommandBuffer command_buffer = beginSingleTimeCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags src_stage{};
    VkPipelineStageFlags dst_stages{};

    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stages = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stages = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
               new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stages = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::runtime_error{"Unsupported Layout Transition!"};
    }

    vkCmdPipelineBarrier(command_buffer, src_stage, dst_stages, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);

    endSingleTimeCommand(command_buffer);
}

void HelloTriangleApplication::copyBufferToImage(VkImage image, VkBuffer buffer, glm::uvec2 size)
{
    VkCommandBuffer command_buffer = beginSingleTimeCommand();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {};
    region.imageExtent = {.width = size.x, .height = size.y, .depth = 1};

    vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &region);

    endSingleTimeCommand(command_buffer);
}

void HelloTriangleApplication::createMipmaps(VkImage image, VkFormat image_format,
                                             const glm::uvec2& size, uint32_t mip_levels)
{
    VkFormatProperties format_properties{};
    vkGetPhysicalDeviceFormatProperties(m_physical_device, image_format, &format_properties);

    bool is_linear_filter_supported = (format_properties.optimalTilingFeatures &
                                       VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0;

    if (!is_linear_filter_supported) {
        throw std::runtime_error{"Texture Image Format doesn't support linear blitting!"};
    }

    VkCommandBuffer command_buffer = beginSingleTimeCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mip_width = size.x;
    int32_t mip_height = size.y;

    for (uint32_t i{1}; i < mip_levels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &barrier);

        int32_t blit_dest_offset_x = mip_width > 1 ? mip_width / 2 : 1;
        int32_t blit_dest_offset_y = mip_height > 1 ? mip_height / 2 : 1;

        VkImageBlit blit{};
        blit.srcOffsets[0] = {};
        blit.srcOffsets[1] = {.x = mip_width, .y = mip_height, .z = 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {};
        blit.dstOffsets[1] = {.x = blit_dest_offset_x, .y = blit_dest_offset_y, .z = 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                             &barrier);

        mip_width = mip_width > 1 ? mip_width / 2 : mip_width;
        mip_height = mip_height > 1 ? mip_height / 2 : mip_height;
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);

    endSingleTimeCommand(command_buffer);
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width{};
    int height{};

    SDL_GetWindowSize(m_window, &width, &height);

    VkExtent2D extent{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    extent.width = std::max(capabilities.minImageExtent.width,
                            std::min(capabilities.maxImageExtent.width, extent.width));
    extent.height = std::max(capabilities.minImageExtent.height,
                             std::min(capabilities.maxImageExtent.height, extent.height));

    return extent;
}

VkShaderModule HelloTriangleApplication::createShaderModule(const ShaderCodeVector& code)
{
    VkShaderModule shader_module{VK_NULL_HANDLE};
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode = code.data();
    create_info.codeSize = code.size() * sizeof(uint32_t);

    if (vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Shader Module!"};
    }

    return shader_module;
}

VkDebugUtilsMessengerCreateInfoEXT HelloTriangleApplication::getDebugMsgrCreateInfo() const
{
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = m_debug_severity;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;
    create_info.pUserData = nullptr;

    return create_info;
}

NamesVector HelloTriangleApplication::getRequiredExtensions()
{
    uint32_t extensions_count{0};
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, nullptr);

    NamesVector extension(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, extension.data());

    extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#ifdef GE_PLATFORM_APPLE
    extension.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif // GE_PLATFORM_APPLE

    return extension;
}

swap_chain_support_details_t
HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device)
{
    swap_chain_support_details_t details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t format_count{0};
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);

    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count,
                                             details.formats.data());
    }

    uint32_t present_mode_count{0};
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) {
        details.present_mode.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count,
                                                  details.present_mode.data());
    }

    return details;
}

VkFormat HelloTriangleApplication::findSupportedFormat(const FormatVector& candidates,
                                                       VkImageTiling tiling,
                                                       VkFormatFeatureFlags features)
{
    for (auto format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

        bool has_linear_features = (props.linearTilingFeatures & features) == features;
        bool has_optimal_features = (props.optimalTilingFeatures & features) == features;

        if (tiling == VK_IMAGE_TILING_LINEAR && has_linear_features) {
            return format;
        }

        if (tiling == VK_IMAGE_TILING_OPTIMAL && has_optimal_features) {
            return format;
        }
    }

    throw std::runtime_error{"Failed to find supported format!"};
}

VkFormat HelloTriangleApplication::findDepthFormat()
{
    FormatVector candidates = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                               VK_FORMAT_D24_UNORM_S8_UINT};

    return findSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace

int main()
{
    GE::Log::settings_t log_settings{};
    log_settings.core_log_level = LOG_LEVEL;
    log_settings.client_log_level = LOG_LEVEL;

    GE::Log::initialize(log_settings);

    try {
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) {
        GE_ERR("{}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
