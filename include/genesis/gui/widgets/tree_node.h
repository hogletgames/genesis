/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

class GE_API TreeNode: public Widget
{
public:
    enum Flag : int
    {
        NONE = 0,
        SELECTED = bit(0),
        FRAMED = bit(1),
        ALLOW_ITEM_OVERLAP = bit(2),
        NO_TREE_PUSH_ON_OPEN = bit(3),
        NO_AUTO_OPEN_ON_LOG = bit(4),
        DEFAULT_OPEN = bit(5),
        OPEN_ON_DOUBLE_CLICK = bit(6),
        OPEN_ON_ARROW = bit(7),
        LEAF = bit(8),
        BULLET = bit(9),
        FRAME_PADDING = bit(10),
        SPAN_AVAIL_WIDTH = bit(11),
        SPAN_FULL_WIDTH = bit(12),
        NAV_LEFT_JUMPS_BACK_HERE = bit(13),
        COLLAPSING_HEADER = FRAMED | NO_TREE_PUSH_ON_OPEN | NO_AUTO_OPEN_ON_LOG
    };

    using Flags = int;

    TreeNode() = default;
    explicit TreeNode(std::string_view label, Flags flags = NONE);
};

} // namespace GE::GUI
