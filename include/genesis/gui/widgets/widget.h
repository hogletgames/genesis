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

#include <genesis/core/interface.h>

#include <functional>

namespace GE::GUI {

class GE_API Widget: public Interface
{
public:
    void begin()
    {
        if (m_begin != nullptr) {
            m_is_opened = m_begin();
        }
    }

    void end()
    {
        if (m_is_opened || m_force_end) {
            m_end();
        }
    }

    bool isOpened() const { return m_is_opened; }

protected:
    using BeginFunc = std::function<bool()>;
    using EndFunc = std::function<void()>;

    template<typename Func, typename... Args>
    void setBeginFunc(Func&& f, Args&&... args)
    {
        m_begin = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
    }

    template<typename Func, typename... Args>
    void setEndFunc(Func&& f, Args&&... args)
    {
        m_end = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
    }

    void setForceEnd() { m_force_end = true; }

private:
    BeginFunc m_begin{nullptr};
    EndFunc m_end{nullptr};

    bool m_is_opened{false};
    bool m_force_end{false};
};

} // namespace GE::GUI
