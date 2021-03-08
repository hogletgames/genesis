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

#include "renderer.h"

#include "genesis/core/log.h"
#include "genesis/core/utils.h"

namespace GE {

bool Renderer::initialize(Renderer::API api)
{
    switch (api) {
        case Renderer::API::VULKAN: break;
        case Renderer::API::NONE:
        default: GE_CORE_ERR("Unknown Render API: {}", toString(api)); return false;
    }

    GE_CORE_INFO("Configuring '{}' as Renderer API", toString(api));

    get()->m_api = api;
    return true;
}

void Renderer::shutdown() {}

std::string toString(Renderer::API api)
{
    std::string default_value = "Unknown (" + std::to_string(static_cast<int>(api)) + ")";
    static const std::unordered_map<Renderer::API, std::string> api_to_str = {
        {Renderer::API::NONE, "None"}, {Renderer::API::VULKAN, "Vulkan"}};

    return toType(api_to_str, api, default_value);
}

} // namespace GE
