/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Dmitry Shilnenkov
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

#include "domain.h"

#include "genesis/core/defer.h"
#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>

namespace GE::Script {

Domain::Domain(std::string_view name, CreationPolicy creation_policy)
{
    recreate(name, creation_policy);
}

Domain::~Domain()
{
    unload();
}

bool Domain::isCurrent() const
{
    return mono_domain_get() == m_domain;
}

void Domain::setAsCurrent() const
{
    if (isValid()) {
        mono_domain_set(m_domain, true);
    }
}

bool Domain::recreate(std::string_view name, CreationPolicy creation_policy)
{
    if (isValid()) {
        unload();
    }

    if (m_domain = mono_domain_create_appdomain(const_cast<char*>(name.data()), nullptr);
        m_domain == nullptr) {
        GE_CORE_ERR("Failed to create domain '{}'", name);
        return false;
    }

    if (creation_policy == CREATION_POLICY_SET_AS_CURRENT) {
        setAsCurrent();
    }

    return true;
}

Domain Domain::rootDomain()
{
    return Domain{mono_get_root_domain(), UNLOAD_POLICY_DO_NOT_UNLOAD};
}

Domain Domain::currentDomain()
{
    return Domain{mono_domain_get(), UNLOAD_POLICY_DO_NOT_UNLOAD};
}

Domain::Domain(MonoDomain* domain, UnloadDomainPolicy unload_policy)
    : m_domain{domain}
    , m_unload_policy(unload_policy)
{}

void Domain::unload()
{
    GE_DEFER([this] { m_domain = nullptr; });

    if (!isValid() || m_unload_policy == UNLOAD_POLICY_DO_NOT_UNLOAD) {
        return;
    }

    if (isCurrent()) {
        mono_domain_set(mono_get_root_domain(), true);
    }

    mono_domain_unload(m_domain);
}

} // namespace GE::Script
