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

#include <genesis/core/memory.h>
#include <genesis/gui/window/iwindow.h>

namespace GE::GUI {

class GE_API WindowMap
{
public:
    using WindowPtr = Scoped<IWindow>;
    using WindowsMap = std::unordered_map<std::string_view, WindowPtr>;
    using Iterator = WindowsMap::iterator;
    using ConstIterator = WindowsMap::const_iterator;

    void onUpdate(Timestamp ts);
    void onEvent(Event* event);
    void onRender();

    void insert(WindowPtr window);
    template<typename... Args>
    void insertWindows(Args&&... windows);
    void remove(std::string_view name);
    IWindow* get(std::string_view name);
    const IWindow* get(std::string_view name) const;

    void openWindow(std::string_view name);
    void closeWindow(std::string_view name);

    Iterator begin() { return m_windows.begin(); }
    Iterator end() { return m_windows.end(); }
    ConstIterator cbegin() const { return m_windows.cbegin(); }
    ConstIterator cend() const { return m_windows.cend(); }

private:
    WindowsMap m_windows;
};

template<typename... Args>
void WindowMap::insertWindows(Args&&... windows)
{
    (insert(std::move(windows)), ...);
}

} // namespace GE::GUI
