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

#include <genesis/core/bit.h>
#include <genesis/gui/widgets/widget.h>

#include <string_view>

namespace GE::GUI {

class GE_API ComboBox: public Widget
{
public:
    using Flags = int;
    using Items = std::vector<std::string_view>;

    enum Flag : uint8_t
    {
        NONE = 0,
        POPUP_ALIGN_LEFT = bit(0),
        HEIGHT_SMALL = bit(1),
        HEIGHT_REGULAR = bit(2),
        HEIGHT_LARGE = bit(3),
        HEIGHT_LARGEST = bit(4),
        NO_ARROW_BUTTON = bit(5),
        NO_PREVIEW = bit(5),
        HEIGHT_MASK = HEIGHT_SMALL | HEIGHT_REGULAR | HEIGHT_LARGE | HEIGHT_LARGEST,
    };

    template<typename Items>
    ComboBox(std::string_view name, const Items& items, std::string_view current_item,
             Flags flags = NONE)
        : ComboBox(name, {items.begin(), items.end()}, current_item, flags)
    {}

    ComboBox(std::string_view name, Items items, std::string_view current_item, Flags flags = NONE);

    std::string_view selectedItem() const { return m_selected_item; }

private:
    Items m_items;
    std::string_view m_current_item;
    std::string_view m_selected_item;
};

} // namespace GE::GUI
