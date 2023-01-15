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

#include <genesis/core/export.h>

#include <chrono>

namespace GE {

template<typename ToDuration, typename FromDuration>
constexpr ToDuration durationCast(FromDuration duration)
{
    return std::chrono::duration_cast<ToDuration>(duration);
}

class GE_API Timestamp
{
public:
    using Sec = std::chrono::duration<double>;
    using Milli = std::chrono::duration<double, std::milli>;
    using Micro = std::chrono::duration<double, std::micro>;
    using Nano = std::chrono::duration<double, std::nano>;

    constexpr Timestamp(double time_sec = 0.0) // NOLINT
        : m_duration{Sec{time_sec}}
    {}

    constexpr Timestamp(Sec duration) // NOLINT
        : m_duration{duration}
    {}

    constexpr Timestamp(Milli duration) // NOLINT
        : m_duration{duration}
    {}

    constexpr Timestamp(Micro duration) // NOLINT
        : m_duration{duration}
    {}

    constexpr Timestamp(Nano duration) // NOLINT
        : m_duration{duration}
    {}

    static Timestamp now()
    {
        using Clock = std::chrono::steady_clock;
        return durationCast<Nano>(Clock::now().time_since_epoch());
    }

    constexpr operator double() const { return sec(); } // NOLINT

    constexpr Timestamp operator-(Timestamp rhs) const { return m_duration - rhs.m_duration; }

    constexpr Timestamp operator+(Timestamp rhs) const { return m_duration + rhs.m_duration; }

    constexpr Timestamp operator-=(Timestamp rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    constexpr Timestamp operator+=(Timestamp rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    constexpr double sec() const { return durationCast<Sec>(m_duration).count(); }
    constexpr double ms() const { return durationCast<Milli>(m_duration).count(); }
    constexpr double us() const { return durationCast<Micro>(m_duration).count(); }
    constexpr double ns() const { return m_duration.count(); }
    constexpr Nano duration() const { return m_duration; }

private:
    Nano m_duration{};
};

} // namespace GE
