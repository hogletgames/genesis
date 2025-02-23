/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include "assembly.h"
#include "class.h"
#include "domain.h"
#include "genesis/core/asserts.h"

#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>

namespace GE::Script {

Assembly::Assembly(const Domain& domain)
    : m_domain{domain.nativeHandle()}
{}

Assembly::~Assembly()
{
    reset();
}

bool Assembly::load(std::string_view assembly_path)
{
    GE_CORE_ASSERT(!isValid(), "Assembly has already been loaded");

    if (m_domain == nullptr) {
        GE_CORE_ERR("Trying to load assembly '{}' into invalid domain", assembly_path);
        return false;
    }

    if (m_assembly = mono_domain_assembly_open(m_domain, assembly_path.data());
        m_assembly == nullptr) {
        GE_CORE_ERR("Failed to load '{}' assembly", assembly_path);
        reset();
        return false;
    }

    if (m_image = mono_assembly_get_image(m_assembly); m_image == nullptr) {
        GE_CORE_ERR("Failed to load image for '{}' assembly", assembly_path);
        reset();
        return false;
    }

    GE_CORE_DBG("Loaded assembly: '{}'", assembly_path);
    return true;
}

Class Assembly::createClass(std::string_view class_namespace, std::string_view class_name) const
{
    if (auto* klass = mono_class_from_name(m_image, class_namespace.data(), class_name.data());
        klass != nullptr) {
        return Class{klass};
    }

    GE_CORE_ERR("Failed to get class from name: namespace '{}', class '{}'", class_namespace,
                class_name);
    return {};
}

void Assembly::reset()
{
    m_domain = nullptr;
    m_assembly = nullptr;
    m_image = nullptr;
}

} // namespace GE::Script
