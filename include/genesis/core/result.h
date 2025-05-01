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

struct error_message_fmt_t {
    std::string_view error;
    std::string_view file_name;
    uint32_t         line{};

    error_message_fmt_t(std::string_view            error,
                        const std::source_location& location = std::source_location::current())
        : error{error}
        , file_name{location.file_name()}
        , line{location.line()}
    {}

    error_message_fmt_t(const char*                 error,
                        const std::source_location& location = std::source_location::current())
        : error{error}
        , file_name{location.file_name()}
        , line{location.line()}
    {}
};

struct error_result_t {
    std::string            error;
    std::string_view       file_name;
    uint32_t               line{};
    Scoped<error_result_t> nested_error{};

    Scoped<error_result_t> clone() const
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

template<typename T>
class GE_API Result
{
public:
    template<typename U>
    Result(U&& value)
        : m_value{std::forward<U>(value)}
    {}

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

    operator bool() const { return std::holds_alternative<T>(m_value); }
    std::string errorMessage() const { return makeErrorMessage(0); }

    template<typename... Args>
    static Result makeFailure(const error_message_fmt_t& error_fmt, Args&&... args)
    {
        return {fmt::format(fmt::runtime(error_fmt.error), std::forward<Args>(args)...),
                error_fmt.file_name, error_fmt.line};
    }

    template<typename U, typename... Args>
    static Result makeFailure(const Result<U>&           nested_error,
                              const error_message_fmt_t& error_fmt,
                              Args&&... args)
    {
        return {nested_error,
                fmt::format(fmt::runtime(error_fmt.error), std::forward<Args>(args)...),
                error_fmt.file_name, error_fmt.line};
    }

private:
    template<typename>
    friend class Result;

    template<typename U>
    using OptionalResult = std::optional<Result<U>>;
    using Value = std::variant<T, error_result_t>;

    Result(std::string_view error, std::string_view file, uint32_t line)
        : m_value{error_result_t{std::string{error}, FS::baseNameView(file), line}}
    {}

    template<typename U>
    Result(const Result<U>& nestedResult,
           std::string_view error,
           std::string_view file,
           uint32_t         line)
        : Result{error, file, line}
    {
        GE_CORE_ASSERT(nestedResult.isErrorResult(), "Nested result must be an error result");
        errorResult().nested_error = nestedResult.errorResult().clone();
    }

    bool isErrorResult() const { return std::holds_alternative<error_result_t>(m_value); }

    error_result_t& errorResult()
    {
        GE_CORE_ASSERT(isErrorResult(), "Result is not an error result");
        return std::get<error_result_t>(m_value);
    }

    const error_result_t& errorResult() const
    {
        GE_CORE_ASSERT(isErrorResult(), "Result is not an error result");
        return std::get<error_result_t>(m_value);
    }

    std::string makeErrorMessage(int error_message_index) const
    {
        if (*this) {
            return {};
        }

        return errorResult().errorMessage(error_message_index);
    }

    Value m_value;
};

} // namespace GE
