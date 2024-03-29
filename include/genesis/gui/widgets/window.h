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

#include <genesis/gui/widgets/widget_node.h>
#include <genesis/math/types.h>

#include <boost/signals2/signal.hpp>

#include <string_view>

namespace GE::GUI {

class GE_API Window: public WidgetNode
{
public:
    using Flags = int;

    using SizeSignal = boost::signals2::signal<void(const Vec2&)>;
    using AvailableRegionSignal = boost::signals2::signal<void(const Vec2&)>;
    using IsFocusedSignal = boost::signals2::signal<void(bool)>;
    using IsHoveredSignal = boost::signals2::signal<void(bool)>;

    explicit Window(std::string_view title, bool* is_open = nullptr, Flags flags = 0);

    void emitSignals() override;

    Vec2 position() const;
    Vec2 size() const;
    Vec2 availableRegion() const;
    float aspectRatio() const;

    bool isFocused() const;
    bool isHovered() const;

    SizeSignal* sizeSignal() { return &m_size_signal; }
    AvailableRegionSignal* availableRegionSignal() { return &m_available_region_signal; }
    IsFocusedSignal* isFocusedSignal() { return &m_is_focused_signal; }
    IsHoveredSignal* isHoveredSignal() { return &m_is_hovered_signal; }

private:
    SizeSignal m_size_signal;
    AvailableRegionSignal m_available_region_signal;
    IsFocusedSignal m_is_focused_signal;
    IsHoveredSignal m_is_hovered_signal;
};

} // namespace GE::GUI
