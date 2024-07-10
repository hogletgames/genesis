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

#pragma once

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>
#include <genesis/math/types.h>

namespace GE {

class GPUCommandQueue;
class Texture;
class UniformBuffer;

class Pipeline: public Interface
{
public:
    using NativeHandle = void*;

    virtual void bind(GPUCommandQueue* queue) = 0;
    virtual void bind(GPUCommandQueue* queue, const std::string& name,
                      const UniformBuffer& ubo) = 0;
    virtual void bind(GPUCommandQueue* queue, const std::string& name, const Texture& texture) = 0;

    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name, bool value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name, int32_t value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name, uint32_t value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name, float value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name, double value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name,
                              const Vec2& value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name,
                              const Vec3& value) = 0;
    virtual void pushConstant(GPUCommandQueue* queue, const std::string& name,
                              const Mat4& value) = 0;

    virtual NativeHandle nativeHandle() const = 0;
};

} // namespace GE
