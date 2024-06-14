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

#include "log_panel.h"

#include "genesis/core/log.h"
#include "genesis/core/string_utils.h"
#include "genesis/core/utils.h"
#include "genesis/gui/widgets.h"

#include <boost/circular_buffer.hpp>
#include <spdlog/sinks/base_sink.h>

#include <array>
#include <vector>

using namespace GE::GUI;

namespace LE {
namespace {

constexpr size_t LOG_LINE_COUNT{10'000};

constexpr std::array<std::string_view, 7> LEVELS = {
    "Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off",
};

const std::unordered_map<spdlog::level::level_enum, std::string_view> LEVEL_TO_STR = {
    {spdlog::level::trace, "Trace"}, {spdlog::level::debug, "Debug"},
    {spdlog::level::info, "Info"},   {spdlog::level::warn, "Warning"},
    {spdlog::level::err, "Error"},   {spdlog::level::critical, "Critical"},
    {spdlog::level::off, "Off"},
};

std::string toString(spdlog::level::level_enum level)
{
    return std::string{GE::getValue(LEVEL_TO_STR, level)};
}

spdlog::level::level_enum toSpdlogLevel(std::string_view level)
{
    static const auto STR_TO_LEVEL = GE::swapKeyAndValue(LEVEL_TO_STR);
    return GE::getValue(STR_TO_LEVEL, level);
}

} // namespace

class LogPanel::LogSink: public spdlog::sinks::base_sink<std::mutex>
{
public:
    using LogBuffer = boost::circular_buffer<std::string>;

    explicit LogSink(size_t log_line_count)
        : m_lines{log_line_count}
    {}

    void clear()
    {
        std::lock_guard lock{mutex_};
        m_lines.clear();
    }

    std::vector<std::string> lines()
    {
        std::lock_guard lock{mutex_};
        return std::vector<std::string>{m_lines.begin(), m_lines.end()};
    }

private:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        formatter_->format(msg, formatted);
        m_lines.push_back(fmt::to_string(formatted));
    }

    void flush_() override{};

    mutable std::mutex m_lines_mtx;
    LogBuffer m_lines;
};

LogPanel::LogPanel()
    : WindowBase(NAME)
    , m_log_sink{std::make_shared<LogPanel::LogSink>(LOG_LINE_COUNT)}
{
    m_log_sink->set_pattern("[%H:%M:%S.%e] [%l] %v");
    m_log_sink->set_level(spdlog::level::info);

    GE::Log::core()->addSink(m_log_sink);
    GE::Log::client()->addSink(m_log_sink);
}

LogPanel::~LogPanel() = default;

void LogPanel::onRender()
{
    StyleVar padding{StyleVar::WINDOW_PADDING, {0.0f, 0.0f}};

    if (WidgetNode node{&m_window}; node.isOpened()) {
        drawControls(&node);
        drawLogs(&node);
    }
}

void LogPanel::drawControls(WidgetNode *node)
{
    if (node->call<Button>("Clear")) {
        m_log_sink->clear();
    }
    node->call<SameLine>();

    auto level_string = toString(m_log_sink->level());
    ComboBox level{"level", LEVELS, level_string};
    node->subNode(&level);

    if (level.selectedItem() != level_string) {
        m_log_sink->set_level(toSpdlogLevel(level.selectedItem()));
    }
}

void LogPanel::drawLogs(WidgetNode *node)
{
    auto logs = m_log_sink->lines();
    auto log_lines = GE::joinString(logs.begin(), logs.end());
    node->call<InputTextMultiline>(&log_lines, m_window.availableRegion());
}

} // namespace LE
