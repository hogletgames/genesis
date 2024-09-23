/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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

#include <genesis/math/types.h>

namespace GE::Vulkan {

template<typename T>
struct data_type_traits;

template<>
struct data_type_traits<bool> {
    static constexpr size_t SIZE{4};
};

template<>
struct data_type_traits<int32_t> {
    static constexpr size_t SIZE{4};
};

template<>
struct data_type_traits<uint32_t> {
    static constexpr size_t SIZE{4};
};

template<>
struct data_type_traits<float> {
    static constexpr size_t SIZE{4};
};

template<>
struct data_type_traits<double> {
    static constexpr size_t SIZE{8};
};

template<>
struct data_type_traits<Vec2> {
    static constexpr size_t SIZE{data_type_traits<Vec2::value_type>::SIZE * Vec2::length()};
};

template<>
struct data_type_traits<Vec3> {
    static constexpr size_t SIZE{data_type_traits<Vec3::value_type>::SIZE * Vec3::length()};
};

template<>
struct data_type_traits<Vec4> {
    static constexpr size_t SIZE{data_type_traits<Vec4::value_type>::SIZE * Vec4::length()};
};

template<>
struct data_type_traits<Mat4> {
    static constexpr size_t SIZE{data_type_traits<Mat4::value_type>::SIZE * Mat4::length() *
                                 Mat4::length()};
};

template<typename T>
static constexpr size_t DATA_TYPE_SIZE{data_type_traits<T>::SIZE};

} // namespace GE::Vulkan
