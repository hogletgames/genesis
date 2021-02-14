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

#include "log.h"
#include "asserts.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>

namespace {

using spdlogLevel = spdlog::level::level_enum;

constexpr auto CORE_LOGGER_NAME = "CORE";
constexpr auto CLIENT_LOGGER_NAME = "APP";
constexpr auto LOG_FTM_PATTERN = "[%H:%M:%S.%e] [%l] %n: %v%$";

constexpr auto LOGLVL_TRACE_STR = "Trace";
constexpr auto LOGLVL_DBG_STR = "Debug";
constexpr auto LOGLVL_INFO_STR = "Info";
constexpr auto LOGLVL_WARN_STR = "Warning";
constexpr auto LOGLVL_ERR_STR = "Error";
constexpr auto LOGLVL_CRIT_STR = "Critical";
constexpr auto LOGLVL_UNKNOWN_STR = "Unknown";

spdlogLevel toSpdlogLvl(GE::Logger::Level level)
{
    std::unordered_map<GE::Logger::Level, spdlogLevel> level_to_spdlog = {
        {GE::Logger::Level::CRITICAL, spdlogLevel::critical},
        {GE::Logger::Level::ERROR, spdlogLevel::err},
        {GE::Logger::Level::WARNING, spdlogLevel::warn},
        {GE::Logger::Level::INFO, spdlogLevel::info},
        {GE::Logger::Level::DEBUG, spdlogLevel::debug},
        {GE::Logger::Level::TRACE, spdlogLevel::trace}};

    return GE::toType(level_to_spdlog, level, spdlogLevel::critical);
}

} // namespace

namespace GE {

Logger::~Logger()
{
    if (m_logger) {
        shutdown();
    }
}

bool Logger::initialize(std::string name)
{
    try {
        m_logger = spdlog::stdout_color_mt(name);
    } catch (const spdlog::spdlog_ex& e) {
        std::cerr << "Failed to create logger '" << name << "': " << e.what()
                  << std::endl;
        return false;
    }

    m_logger_name = std::move(name);
    m_logger->set_pattern(LOG_FTM_PATTERN);

    return true;
}

void Logger::shutdown()
{
    spdlog::drop(m_logger_name);
    m_logger.reset();
    m_logger_name.clear();
}

void Logger::setLevel(Level level)
{
    m_level = level;
    m_logger->set_level(toSpdlogLvl(level));
    m_level = level;
}

bool Log::initialize()
{
    auto& core_logger = get()->m_core_logger;
    auto& client_logger = get()->m_client_logger;

    if (!core_logger.initialize(CORE_LOGGER_NAME) ||
        !client_logger.initialize(CLIENT_LOGGER_NAME)) {
        shutdown();
        return false;
    }

    core_logger.setLevel(Logger::Level::TRACE);
    client_logger.setLevel(Logger::Level::TRACE);

    GE_CORE_DBG("Log system has been initialized");
    return true;
}

void Log::shutdown()
{
    GE_CORE_DBG("Shutdown log system");
    get()->m_client_logger.shutdown();
    get()->m_core_logger.shutdown();
}

std::string toString(Logger::Level level)
{
    std::unordered_map<Logger::Level, std::string> lvl_to_str = {
        {Logger::Level::UNKNOWN, LOGLVL_UNKNOWN_STR},
        {Logger::Level::CRITICAL, LOGLVL_CRIT_STR},
        {Logger::Level::ERROR, LOGLVL_ERR_STR},
        {Logger::Level::WARNING, LOGLVL_WARN_STR},
        {Logger::Level::INFO, LOGLVL_INFO_STR},
        {Logger::Level::DEBUG, LOGLVL_DBG_STR},
        {Logger::Level::TRACE, LOGLVL_TRACE_STR}};

    return toType(lvl_to_str, level, std::string(LOGLVL_UNKNOWN_STR));
}

Logger::Level toLogLvl([[maybe_unused]] const std::string& level)
{
    std::unordered_map<std::string, Logger::Level> str_to_lvl = {
        {LOGLVL_UNKNOWN_STR, Logger::Level::UNKNOWN},
        {LOGLVL_CRIT_STR, Logger::Level::CRITICAL},
        {LOGLVL_ERR_STR, Logger::Level::ERROR},
        {LOGLVL_WARN_STR, Logger::Level::WARNING},
        {LOGLVL_INFO_STR, Logger::Level::INFO},
        {LOGLVL_DBG_STR, Logger::Level::DEBUG},
        {LOGLVL_TRACE_STR, Logger::Level::TRACE}};

    return toType(str_to_lvl, level, Logger::Level::UNKNOWN);
}

} // namespace GE
