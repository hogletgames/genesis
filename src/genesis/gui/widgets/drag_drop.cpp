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
#include "private/flag_utils.h"
#include "widgets/text.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

#include <imgui.h>

#include <array>
#include <utility>

namespace GE::GUI {
namespace {

ImGuiDragDropFlags fromPayloadFlags(DragDropPayload::Flags flags)
{
    constexpr std::array<std::pair<DragDropPayload::Flag, ImGuiDragDropFlags_>, 3>
        To_IMGUI_DRAG_DROP_FLAGS = {
            std::make_pair(DragDropPayload::BEFORE_DELIVERY,
                           ImGuiDragDropFlags_AcceptBeforeDelivery),
            std::make_pair(DragDropPayload::NO_DRAW_DEFAULT_RECT,
                           ImGuiDragDropFlags_AcceptNoDrawDefaultRect),
            std::make_pair(DragDropPayload::NO_PREVIEW_TOOLTIP,
                           ImGuiDragDropFlags_AcceptNoPreviewTooltip),
        };

    return toImGuiFlags<ImGuiDragDropFlags>(To_IMGUI_DRAG_DROP_FLAGS, flags);
}

ImGuiDragDropFlags fromSourceFlags(DragDropSource::Flags flags)
{
    constexpr std::array<std::pair<DragDropSource::Flag, ImGuiDragDropFlags_>, 6>
        TO_IMGUI_DRAG_DROP_FLAGS = {
            std::make_pair(DragDropSource::NO_PREVIEW_TOOLTIP,
                           ImGuiDragDropFlags_SourceNoPreviewTooltip),
            std::make_pair(DragDropSource::NO_DISABLE_HOVER,
                           ImGuiDragDropFlags_SourceNoDisableHover),
            std::make_pair(DragDropSource::NO_HOLD_TO_OPENOTHERS,
                           ImGuiDragDropFlags_SourceNoHoldToOpenOthers),
            std::make_pair(DragDropSource::ALLOW_NULL_ID, ImGuiDragDropFlags_SourceAllowNullID),
            std::make_pair(DragDropSource::EXTERN, ImGuiDragDropFlags_SourceExtern),
            std::make_pair(DragDropSource::AUTO_EXPIRE_PAYLOAD,
                           ImGuiDragDropFlags_SourceAutoExpirePayload),
        };

    return toImGuiFlags<ImGuiDragDropFlags>(TO_IMGUI_DRAG_DROP_FLAGS, flags);
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

DragDropSource::DragDropSource(const DragDropPayload& payload,
                               std::string_view       dragging_text,
                               Flags                  flags)
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
