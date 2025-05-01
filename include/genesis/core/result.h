/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Dmitry Shilnenkov
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

#include <genesis/core/asserts.h>
#include <genesis/core/export.h>
#include <genesis/core/format.h>
#include <genesis/core/memory.h>
#include <genesis/filesystem/file.h>

#include <optional>
#include <source_location>
#include <string>
#include <variant>

namespace GE {

template<typename T>
class GE_API Result
{
public:
    Result() = default;

    Result& operator=(Result&& other)
    {
        m_value = std::move(other.m_value);
        return *this;
    }

    T& value()
    {
        GE_CORE_ASSERT(*this, "Result is not successful");
        return std::get<T>(m_value);
    }

    const T& value() const
    {
        GE_CORE_ASSERT(*this, "Result is not successful");
        return std::get<T>(m_value);
    }

    std::string errorMessage() const { return makeErrorMessage(0); }

    operator bool() const { return std::holds_alternative<T>(m_value); }

private:
    friend class Success;
    friend class Failure;

    struct error_result_t {
        std::string                  error;
        const std::string_view       file_name;
        uint32_t                     line{};
        Scoped<const error_result_t> nested_error;

        Scoped<const error_result_t> clone() const
        {
            auto cloned_error = makeScoped<error_result_t>(error, file_name, line);
            if (nested_error) {
                cloned_error->nested_error = nested_error->clone();
            }
            return cloned_error;
        }

        std::string errorMessage(int message_index) const
        {
            auto message = GE_FMTSTR("#{} {}:{}: {}", message_index, file_name, line, error);
            if (nested_error) {
                message += '\n' + nested_error->errorMessage(message_index + 1);
            }
            return message;
        }
    };

    using Value = std::variant<T, error_result_t>;

    explicit Result(const T& value)
        : m_value{value}
    {}

    template<typename NestedResultType>
    Result(std::string_view                error,
           const Result<NestedResultType>& nestedResult,
           const std::source_location&     location)
        : m_value{error_result_t{std::string{error}, FS::baseNameView(location.file_name()),
                                 location.line()}}
    {
        GE_CORE_ASSERT(nestedResult.isErrorResult(), "Nested result must be an error result");
        errorResult().nested_error = nestedResult.errorResult().clone();
    }

    bool isErrorResult() const { return std::holds_alternative<error_result_t>(m_value); }

    error_result_t& errorResult() { return std::get<error_result_t>(m_value); }
    const error_result_t& errorResult() const { return std::get<error_result_t>(m_value); }

    std::string makeErrorMessage(int error_message_index) const
    {
        if (*this) {
            return {};
        }

        return errorResult().errorMessage(error_message_index);
    }

    Value m_value;
};

class Success
{
public:
    Success() = delete;

    template<typename T>
    static Result<T> make(const T& value)
    {
        return Result<T>(value);
    }
};

class Failure
{
public:
    Failure() = delete;

    template<typename ResultType, typename NestedResultType = int>
    static Result<ResultType>
    make(std::string_view                error,
         const Result<NestedResultType>& nestedResult = {},
         const std::source_location&     location = std::source_location::current())
    {
        return Result<ResultType>{error, nestedResult, location};
    }
};

} // namespace GE
