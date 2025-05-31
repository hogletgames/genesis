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

#pragma once

#include <genesis/core/bit.h>
#include <genesis/gui/widgets/widget_node.h>

#include <string>

namespace GE::GUI {

class DragDropSource;
class DragDropTarget;

class GE_API DragDropPayload
{
public:
    using Flags = int;

    enum Flag : uint8_t
    {
        NONE = 0,
        BEFORE_DELIVERY = bit<uint8_t>(10),
        NO_DRAW_DEFAULT_RECT = bit<uint8_t>(11),
        NO_PREVIEW_TOOLTIP = bit<uint8_t>(12),
        PEEK_ONLY = BEFORE_DELIVERY | NO_DRAW_DEFAULT_RECT,
    };

    template<typename T>
    DragDropPayload(const std::string& type, const T* object, Flags flags = NONE)
        : DragDropPayload(type, object, sizeof(T), flags)
    {}

    DragDropPayload(std::string type, const void* data, size_t size, Flags flags);

    std::string_view type() const { return m_payload_type; }
    bool accepted() const { return !m_accepted_payload.empty(); }

    template<typename T>
    const T* get()
    {
        return validateAcceptedPayload(sizeof(T))
                   ? reinterpret_cast<const T*>(m_accepted_payload.data())
                   : nullptr;
    }

private:
    friend DragDropSource;
    friend DragDropTarget;

    void setPayload() const;
    bool acceptPaylaod();
    bool validateAcceptedPayload(size_t requested_size) const;

    Flags       m_flags{NONE};
    std::string m_payload_type;
    const void* m_payload_data{nullptr};
    size_t      m_paylaod_size{0};

    std::vector<uint8_t> m_accepted_payload;
};

class GE_API DragDropSource: public Widget
{
public:
    using Flags = int;

    enum Flag : uint8_t
    {
        NONE = 0,
        NO_PREVIEW_TOOLTIP = bit<uint8_t>(0),
        NO_DISABLE_HOVER = bit<uint8_t>(1),
        NO_HOLD_TO_OPENOTHERS = bit<uint8_t>(2),
        ALLOW_NULL_ID = bit<uint8_t>(3),
        EXTERN = bit<uint8_t>(4),
        AUTO_EXPIRE_PAYLOAD = bit<uint8_t>(5),
    };

    DragDropSource(const DragDropPayload& payload,
                   std::string_view       dragging_text,
                   Flags                  flags = NONE);
};

class GE_API DragDropTarget: public Widget
{
public:
    explicit DragDropTarget(DragDropPayload* payload);
};

} // namespace GE::GUI
