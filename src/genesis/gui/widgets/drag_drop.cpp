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

#include "drag_drop.h"

#include "widgets/text.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

#include <imgui.h>

namespace GE::GUI {
namespace {

ImGuiDragDropFlags fromPayloadFlags(DragDropPayload::Flags flags)
{
    ImGuiDragDropFlags imgui_flags{ImGuiDragDropFlags_None};

    if (checkBits<DragDropPayload::Flags>(flags, DragDropPayload::BEFORE_DELIVERY)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_AcceptBeforeDelivery);
    }
    if (checkBits<DragDropPayload::Flags>(flags, DragDropPayload::NO_DRAW_DEFAULT_RECT)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
    }
    if (checkBits<DragDropPayload::Flags>(flags, DragDropPayload::NO_PREVIEW_TOOLTIP)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_AcceptNoPreviewTooltip);
    }

    return imgui_flags;
}

ImGuiDragDropFlags fromSourceFlags(DragDropSource::Flags flags)
{
    ImGuiDragDropFlags imgui_flags{ImGuiDragDropFlags_None};

    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::NO_PREVIEW_TOOLTIP)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceNoPreviewTooltip);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::NO_PREVIEW_TOOLTIP)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceNoPreviewTooltip);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::NO_DISABLE_HOVER)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceNoDisableHover);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::NO_HOLD_TO_OPENOTHERS)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceNoHoldToOpenOthers);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::ALLOW_NULL_ID)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceAllowNullID);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::EXTERN)) {
        imgui_flags = setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceExtern);
    }
    if (checkBits<DragDropSource::Flags>(flags, DragDropSource::AUTO_EXPIRE_PAYLOAD)) {
        imgui_flags =
            setBits<ImGuiDragDropFlags>(imgui_flags, ImGuiDragDropFlags_SourceAutoExpirePayload);
    }

    return imgui_flags;
}

} // namespace

DragDropPayload::DragDropPayload(std::string type, const void* data, size_t size, Flags flags)
    : m_flags{flags}
    , m_payload_type{std::move(type)}
    , m_payload_data{data}
    , m_paylaod_size{size}
{}

void DragDropPayload::setPayload() const
{
    ImGui::SetDragDropPayload(m_payload_type.c_str(), m_payload_data, m_paylaod_size);
}

bool DragDropPayload::acceptPaylaod()
{
    m_accepted_payload.clear();

    if (const auto* paylaod =
            ImGui::AcceptDragDropPayload(m_payload_type.c_str(), fromPayloadFlags(m_flags));
        paylaod != nullptr) {
        m_accepted_payload.resize(paylaod->DataSize);
        std::memcpy(m_accepted_payload.data(), paylaod->Data, paylaod->DataSize);
    }

    return !m_accepted_payload.empty();
}

bool DragDropPayload::validateAcceptedPayload(size_t requested_size) const
{
    if (m_accepted_payload.size() != requested_size) {
        GE_CORE_ERR("Incorrect requested paylaod size, expected={}, got={}",
                    m_accepted_payload.size(), requested_size);
        return false;
    }

    return true;
}

DragDropSource::DragDropSource(const DragDropPayload& payload, std::string_view dragging_text,
                               Flags flags)
{
    setBeginFunc([&payload, dragging_text, flags] {
        if (ImGui::BeginDragDropSource(fromSourceFlags(flags))) {
            payload.setPayload();
            Text::call(dragging_text);
            return true;
        }

        return false;
    });

    setEndFunc(&ImGui::EndDragDropSource);
}

DragDropTarget::DragDropTarget(DragDropPayload* payload)
{
    setBeginFunc([payload] {
        if (ImGui::BeginDragDropTarget()) {
            payload->acceptPaylaod();
            return true;
        }

        return false;
    });

    setEndFunc(&ImGui::EndDragDropTarget);
}

} // namespace GE::GUI
