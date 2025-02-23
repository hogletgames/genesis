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

#pragma once

#include <genesis/core/export.h>

#include <string_view>

extern "C" {
typedef struct _MonoDomain MonoDomain;
}

namespace GE::Script {

class Assembly;

class GE_API Domain
{
public:
    enum CreationPolicy : uint8_t
    {
        CREATION_POLICY_DO_NOT_SET_AS_CURRENT,
        CREATION_POLICY_SET_AS_CURRENT,
    };

    explicit Domain(std::string_view name,
                    CreationPolicy creation_policy = CREATION_POLICY_SET_AS_CURRENT);
    ~Domain();

    Domain(Domain&& other) noexcept = default;
    Domain& operator=(Domain&& other) noexcept = default;

    Domain(const Domain& other) = delete;
    Domain& operator=(const Domain& other) noexcept = delete;

    bool recreate(std::string_view name,
                  CreationPolicy creation_policy = CREATION_POLICY_SET_AS_CURRENT);

    bool isCurrent() const;
    void setAsCurrent() const;

    bool isValid() const { return m_domain != nullptr; }
    MonoDomain* nativeHandle() const { return m_domain; }

    static Domain rootDomain();
    static Domain currentDomain();

private:
    enum UnloadDomainPolicy : uint8_t
    {
        UNLOAD_POLICY_UNLOAD,
        UNLOAD_POLICY_DO_NOT_UNLOAD,
    };

    explicit Domain(MonoDomain* domain,
                    UnloadDomainPolicy unload_policy = UNLOAD_POLICY_DO_NOT_UNLOAD);

    void unload();

    MonoDomain* m_domain{nullptr};
    UnloadDomainPolicy m_unload_policy{UNLOAD_POLICY_UNLOAD};
};

} // namespace GE::Script
