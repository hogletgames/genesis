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

#pragma once

#include <genesis/graphics/framebuffer.h>
#include <genesis/graphics/gpu_command_queue.h>
#include <genesis/graphics/graphics.h>
#include <genesis/graphics/graphics_context.h>
#include <genesis/graphics/graphics_factory.h>
#include <genesis/graphics/index_buffer.h>
#include <genesis/graphics/mesh.h>
#include <genesis/graphics/pipeline.h>
#include <genesis/graphics/pipeline_config.h>
#include <genesis/graphics/primitives_renderer.h>
#include <genesis/graphics/render_command.h>
#include <genesis/graphics/renderer.h>
#include <genesis/graphics/shader.h>
#include <genesis/graphics/shader_input_layout.h>
#include <genesis/graphics/shader_precompiler.h>
#include <genesis/graphics/shader_reflection.h>
#include <genesis/graphics/shader_resource_descriptors.h>
#include <genesis/graphics/texture.h>
#include <genesis/graphics/texture_loader.h>
#include <genesis/graphics/uniform_buffer.h>
#include <genesis/graphics/vertex.h>
#include <genesis/graphics/vertex_buffer.h>
