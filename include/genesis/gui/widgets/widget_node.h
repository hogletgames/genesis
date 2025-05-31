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

#include <genesis/core/concepts.h>
#include <genesis/core/memory.h>
#include <genesis/gui/widgets/widget.h>

namespace GE::GUI {

template<typename T, typename ReturnValue, typename... Args>
concept WidgetCallReturns = requires(Args... args) {
    T::call(args...);
    { T::call(args...) } -> std::same_as<ReturnValue>;
};

class GE_API WidgetNode
{
public:
    explicit WidgetNode(Widget* widget)
        : m_widget{widget}
    {
        begin();
    }

    explicit WidgetNode(Scoped<Widget> widget)
        : m_internal_widget{std::move(widget)}
        , m_widget{m_internal_widget.get()}
    {
        begin();
    }

    ~WidgetNode()
    {
        if (m_widget != nullptr) {
            m_widget->end();
        }
    }

    template<typename T, typename... Args>
    void call(Args&&... args)
        requires WidgetCallReturns<T, void, Args...>
    {
        if (isOpened()) {
            T::call(std::forward<Args>(args)...);
        }
    }

    template<typename T, typename... Args>
    bool call(Args&&... args)
        requires WidgetCallReturns<T, bool, Args...>
    {
        if (isOpened()) {
            return T::call(std::forward<Args>(args)...);
        }

        return false;
    }

    template<typename Func, typename... Args>
    void call(Func&& f, Args&&... args)
        requires FuncReturns<Func, void, Args...>
    {
        if (isOpened()) {
            std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
        }
    }

    template<typename Func, typename... Args>
    bool call(Func&& f, Args&&... args)
        requires FuncReturns<Func, bool, Args...>
    {
        if (isOpened()) {
            return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
        }

        return false;
    }

    WidgetNode subNode(Widget* widget_node) const
    {
        if (isOpened()) {
            return WidgetNode{widget_node};
        }

        return WidgetNode{nullptr};
    }

    template<typename T, typename... Args>
    WidgetNode makeSubNode(Args&&... args)
    {
        if (isOpened()) {
            return create<T>(std::forward<Args>(args)...);
        }

        return WidgetNode{nullptr};
    }

    Widget* widget() { return m_widget; }
    bool isOpened() const { return m_widget != nullptr && m_widget->isOpened(); }

    template<typename T, typename... Args>
    static WidgetNode create(Args&&... args)
    {
        return WidgetNode{makeScoped<T>(std::forward<Args>(args)...)};
    }

private:
    void begin() const
    {
        if (m_widget != nullptr) {
            m_widget->begin();
        }
    }

    Scoped<Widget> m_internal_widget;
    Widget*        m_widget{nullptr};
};

} // namespace GE::GUI
