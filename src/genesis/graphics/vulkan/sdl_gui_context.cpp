/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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

#include "sdl_gui_context.h"
#include "command_buffer.h"
#include "device.h"
#include "image.h"
#include "instance.h"
#include "renderers/window_renderer.h"
#include "single_command.h"
#include "swap_chain.h"
#include "texture.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"
#include "genesis/graphics/graphics.h"
#include "genesis/window/key_codes.h"

#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>

namespace {

void mapKeys()
{
    auto &io = ImGui::GetIO();
    auto cast_key = [](GE::KeyCode key) { return static_cast<int>(key); };

    io.KeyMap[ImGuiKey_Tab] = cast_key(GE::KeyCode::TAB);
    io.KeyMap[ImGuiKey_LeftArrow] = cast_key(GE::KeyCode::LEFT);
    io.KeyMap[ImGuiKey_RightArrow] = cast_key(GE::KeyCode::RIGHT);
    io.KeyMap[ImGuiKey_UpArrow] = cast_key(GE::KeyCode::UP);
    io.KeyMap[ImGuiKey_DownArrow] = cast_key(GE::KeyCode::DOWN);
    io.KeyMap[ImGuiKey_PageUp] = cast_key(GE::KeyCode::PAGE_UP);
    io.KeyMap[ImGuiKey_PageDown] = cast_key(GE::KeyCode::PAGE_DOWN);
    io.KeyMap[ImGuiKey_Home] = cast_key(GE::KeyCode::HOME);
    io.KeyMap[ImGuiKey_End] = cast_key(GE::KeyCode::END);
    io.KeyMap[ImGuiKey_Insert] = cast_key(GE::KeyCode::INSERT);
    io.KeyMap[ImGuiKey_Delete] = cast_key(GE::KeyCode::DELETE);
    io.KeyMap[ImGuiKey_Backspace] = cast_key(GE::KeyCode::BACKSPACE);
    io.KeyMap[ImGuiKey_Space] = cast_key(GE::KeyCode::SPACE);
    io.KeyMap[ImGuiKey_Enter] = cast_key(GE::KeyCode::RETURN);
    io.KeyMap[ImGuiKey_Escape] = cast_key(GE::KeyCode::ESCAPE);
    io.KeyMap[ImGuiKey_KeyPadEnter] = cast_key(GE::KeyCode::KP_ENTER);
    io.KeyMap[ImGuiKey_A] = cast_key(GE::KeyCode::A);
    io.KeyMap[ImGuiKey_C] = cast_key(GE::KeyCode::C);
    io.KeyMap[ImGuiKey_V] = cast_key(GE::KeyCode::V);
    io.KeyMap[ImGuiKey_X] = cast_key(GE::KeyCode::X);
    io.KeyMap[ImGuiKey_Y] = cast_key(GE::KeyCode::Y);
    io.KeyMap[ImGuiKey_Z] = cast_key(GE::KeyCode::Z);
}

} // namespace

namespace GE::Vulkan::SDL {

GUIContext::GUIContext(void *window, Shared<Device> device,
                       WindowRenderer *window_renderer)
    : m_device{std::move(device)}
{
    GE_CORE_INFO("Initializing Vulkan SDL GUI");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (isDockingEnabled()) {
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    auto *sdl_window = reinterpret_cast<SDL_Window *>(window);

    if (!ImGui_ImplSDL2_InitForVulkan(sdl_window)) {
        throw Vulkan::Exception{"Failed to initialize SDL2 for Vulkan"};
    }

    auto *swap_chain = window_renderer->swapChain();
    VkRenderPass render_pass = window_renderer->renderPass(Renderer::CLEAR_ALL);

    createDescriptorPool();

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = Instance::instance();
    init_info.PhysicalDevice = m_device->physicalDevice();
    init_info.Device = m_device->device();
    init_info.QueueFamily = m_device->queueIndices().graphics_family.value();
    init_info.Queue = m_device->graphicsQueue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = m_descriptor_pool;
    init_info.Allocator = nullptr;
    init_info.MinImageCount = swap_chain->minImageCount();
    init_info.ImageCount = swap_chain->imageCount();
    init_info.CheckVkResultFn = nullptr;

    if (!ImGui_ImplVulkan_Init(&init_info, render_pass)) {
        throw Vulkan::Exception{"Failed to initialize GUI Vulkan backend"};
    }

    // Load fonts
    {
        SingleCommand cmd(m_device);
        ImGui_ImplVulkan_CreateFontsTexture(cmd.getCmdBuffer());
    }

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    mapKeys();
}

GUIContext::~GUIContext()
{
    GE_CORE_INFO("Destroying Vulkan SDL GUI");

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    destroyVulkanHandles();
}

void GUIContext::begin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();

    if (isDockingEnabled()) {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }
}

void GUIContext::end()
{
    ImGui::Render();
    Graphics::command()->draw(this);

    if (isViewportEnabled()) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void GUIContext::draw(GPUCommandQueue *queue)
{
    queue->enqueue([](void *cmd) {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer(cmd));
    });
}

void GUIContext::createDescriptorPool()
{
    constexpr size_t descriptor_count{1000};

    static constexpr std::array<VkDescriptorPoolSize, 11> pool_sizes = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptor_count},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptor_count},
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = pool_sizes.size() * descriptor_count;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();

    if (vkCreateDescriptorPool(m_device->device(), &pool_info, nullptr,
                               &m_descriptor_pool) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Descriptor Pool"};
    }
}

void GUIContext::destroyVulkanHandles()
{
    vkDestroyDescriptorPool(m_device->device(), m_descriptor_pool, nullptr);
    m_descriptor_pool = VK_NULL_HANDLE;
}

bool GUIContext::isDockingEnabled() const
{
    return (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) != 0;
}

bool GUIContext::isViewportEnabled() const
{
    return (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0;
}

VkDescriptorSet createGuiTextureID(const Vulkan::Texture &texture)
{
    static constexpr VkImageLayout image_layout{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    return reinterpret_cast<VkDescriptorSet>(::ImGui_ImplVulkan_AddTexture(
        texture.sampler(), texture.image()->view(), image_layout));
}

void destroyGuiTextureID(VkDescriptorSet texture_id)
{
    if (texture_id != VK_NULL_HANDLE) {
        ::ImGui_ImplVulkan_DestroyTexture(reinterpret_cast<ImTextureID>(texture_id));
    }
};

} // namespace GE::Vulkan::SDL
