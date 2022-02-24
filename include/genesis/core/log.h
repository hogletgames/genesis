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

#ifndef GENESIS_CORE_LOG_H_
#define GENESIS_CORE_LOG_H_

#include <genesis/core/enum.h>
#include <genesis/core/export.h>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <memory>

#define GE_CORE_CRIT(...)  ::GE::Log::core()->crit(__FILE__, __LINE__, __VA_ARGS__)
#define GE_CORE_ERR(...)   ::GE::Log::core()->error(__FILE__, __LINE__, __VA_ARGS__)
#define GE_CORE_INFO(...)  ::GE::Log::core()->info(__FILE__, __LINE__, __VA_ARGS__)
#define GE_CORE_WARN(...)  ::GE::Log::core()->warn(__FILE__, __LINE__, __VA_ARGS__)
#define GE_CORE_DBG(...)   ::GE::Log::core()->debug(__FILE__, __LINE__, __VA_ARGS__)
#define GE_CORE_TRACE(...) ::GE::Log::core()->trace(__FILE__, __LINE__, __VA_ARGS__)

#define GE_CRIT(...)  ::GE::Log::client()->crit(__FILE__, __LINE__, __VA_ARGS__)
#define GE_ERR(...)   ::GE::Log::client()->error(__FILE__, __LINE__, __VA_ARGS__)
#define GE_WARN(...)  ::GE::Log::client()->warn(__FILE__, __LINE__, __VA_ARGS__)
#define GE_INFO(...)  ::GE::Log::client()->info(__FILE__, __LINE__, __VA_ARGS__)
#define GE_DBG(...)   ::GE::Log::client()->debug(__FILE__, __LINE__, __VA_ARGS__)
#define GE_TRACE(...) ::GE::Log::client()->trace(__FILE__, __LINE__, __VA_ARGS__)

namespace GE {

class GE_API Logger
{
public:
    enum class Level : uint8_t
    {
        UNKNOWN = 0,
        CRITICAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG,
        TRACE,
    };

    bool initialize(std::string name);
    void shutdown();

    void setLevel(Level level);
    Level getLevel() const { return m_level; }

    template<typename... Args>
    void crit(Args&&... args)
    {
        log(spdlog::level::critical, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(Args&&... args)
    {
        log(spdlog::level::err, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(Args&&... args)
    {
        log(spdlog::level::warn, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(Args&&... args)
    {
        log(spdlog::level::info, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(Args&&... args)
    {
        log(spdlog::level::debug, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void trace(Args&&... args)
    {
        log(spdlog::level::trace, std::forward<Args>(args)...);
    }

private:
    template<typename... Args>
    void log(spdlog::level::level_enum level, const char* file, int line, Args&&... args)
    {
        if (m_logger) {
            m_logger->log({file, line, nullptr}, level, std::forward<Args>(args)...);
        }
    }

    std::string m_logger_name;
    Level m_level{Level::INFO};
    std::shared_ptr<spdlog::logger> m_logger;
};

class GE_API Log
{
public:
    struct settings_t {
        Logger::Level core_log_level{LOG_LEVEL_DEFAULT};
        Logger::Level client_log_level{LOG_LEVEL_DEFAULT};

        static constexpr Logger::Level LOG_LEVEL_DEFAULT{Logger::Level::INFO};
    };

    static bool initialize(const settings_t& settings);
    static void shutdown();

    static Logger* core() { return &get()->m_core_logger; }
    static Logger* client() { return &get()->m_client_logger; }

private:
    Log() = default;

    static Log* get()
    {
        static Log instance;
        return &instance;
    }

    Logger m_core_logger;
    Logger m_client_logger;
};

Logger::Level toLoggerLevel(const std::string& level_str);

} // namespace GE

#endif // GENESIS_CORE_LOG_H_
