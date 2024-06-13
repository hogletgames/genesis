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

namespace GE::GUI {

class GE_API WidgetNodeGuard
{
public:
    explicit WidgetNodeGuard(WidgetNode* widget)
        : m_widget{widget}
    {
        begin();
    }

    explicit WidgetNodeGuard(Scoped<WidgetNode> widget)
        : m_internal_widget{std::move(widget)}
        , m_widget{m_internal_widget.get()}
    {
        begin();
    }

    ~WidgetNodeGuard()
    {
        if (m_widget != nullptr) {
            m_widget->end();
        }
    }

    template<typename T, typename... Args>
    auto call(Args&&... args) -> std::enable_if_t<std::is_void_v<decltype(T::call(args...))>>
    {
        if (isOpened()) {
            T::call(std::forward<Args>(args)...);
        }
    }

    template<typename T, typename... Args>
    auto call(Args&&... args)
        -> std::enable_if_t<std::is_same_v<decltype(T::call(args...)), bool>, bool>
    {
        if (isOpened()) {
            return T::call(std::forward<Args>(args)...);
        }

        return false;
    }

    template<typename Func, typename... Args>
    void call(Func&& f, Args&&... args)
    {
        if (isOpened()) {
            std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
        }
    }

    WidgetNodeGuard subNode(WidgetNode* widget_node)
    {
        if (isOpened()) {
            return WidgetNodeGuard{widget_node};
        }

        return WidgetNodeGuard{nullptr};
    }

    template<typename T, typename... Args>
    WidgetNodeGuard makeSubNode(Args&&... args)
    {
        if (isOpened()) {
            return create<T>(std::forward<Args>(args)...);
        }

        return WidgetNodeGuard{nullptr};
    }

    WidgetNode* widget() { return m_widget; }
    bool isOpened() const { return m_widget != nullptr && m_widget->isOpened(); }

    template<typename T, typename... Args>
    static WidgetNodeGuard create(Args&&... args)
    {
        return WidgetNodeGuard{makeScoped<T>(std::forward<Args>(args)...)};
    }

private:
    void begin() const
    {
        if (m_widget != nullptr) {
            m_widget->begin();
        }
    }

    Scoped<WidgetNode> m_internal_widget;
    WidgetNode* m_widget{nullptr};
};

} // namespace GE::GUI
