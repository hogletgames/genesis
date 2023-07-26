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

#include <genesis/core/memory.h>
#include <genesis/gui/widgets/widget_node.h>

#include <deque>

namespace GE::GUI {

class GE_API WidgetNodeGuard
{
public:
    explicit WidgetNodeGuard(WidgetNode* node)
        : m_node{node}
    {
        if (m_node != nullptr) {
            m_node->begin();
        }
    }

    ~WidgetNodeGuard()
    {
        if (m_node != nullptr) {
            m_node->end();
        }
    }

    template<typename T, typename... Args>
    auto call(Args&&... args) -> std::enable_if_t<std::is_void_v<decltype(T::call(args...))>>
    {
        if (isNodeCallable()) {
            T::call(std::forward<Args>(args)...);
        }
    }

    template<typename T, typename... Args>
    auto call(Args&&... args)
        -> std::enable_if_t<std::is_same_v<decltype(T::call(args...)), bool>, bool>
    {
        if (isNodeCallable()) {
            return T::call(std::forward<Args>(args)...);
        }

        return false;
    }

    template<typename Func, typename... Args>
    void call(Func&& f, Args&&... args)
    {
        if (isNodeCallable()) {
            std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
        }
    }

    WidgetNodeGuard subNode(WidgetNode* node)
    {
        if (isNodeCallable()) {
            return WidgetNodeGuard{node};
        }

        return WidgetNodeGuard{nullptr};
    }

private:
    bool isNodeCallable() const { return m_node != nullptr && m_node->isOpened(); }

    WidgetNode* m_node{nullptr};
};

} // namespace GE::GUI
