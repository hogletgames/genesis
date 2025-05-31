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

#include <genesis/core/export.h>

#include <boost/signals2/signal.hpp>

#include <string>
#include <vector>

namespace GE::GUI {

class GE_API FileDialog
{
public:
    using SingleResultSignal = boost::signals2::signal<void(std::string_view)>;
    using MultipleResultsSignal = boost::signals2::signal<void(const std::vector<std::string>&)>;
    using CancelSignal = boost::signals2::signal<void()>;
    using ErrorSignal = boost::signals2::signal<void(std::string_view)>;

    enum Type : uint8_t
    {
        SINGLE_FILE,
        MULTIPLE_FILES,
        SELECT_FOLDER,
        SAVE_FILE,
    };

    void showSingleFile(std::string_view default_path, std::string_view filter_list = {});
    void showMultipleFiles(std::string_view default_path, std::string_view filter_list = {});
    void showSelectFolder(std::string_view default_path);
    void showSaveFile(std::string_view default_path, std::string_view filter_list = {});

    SingleResultSignal* singleResultSignal() { return &m_single_result_file; }
    MultipleResultsSignal* multipleResultsSignal() { return &m_multiple_files_signal; }
    CancelSignal* cancelSignal() { return &m_cancel_signal; }
    ErrorSignal* errorSignal() { return &m_error_signal; }

private:
    void error(std::string_view error);

    SingleResultSignal    m_single_result_file;
    MultipleResultsSignal m_multiple_files_signal;
    CancelSignal          m_cancel_signal;
    ErrorSignal           m_error_signal;
};

GE_API std::string openSingleFile(std::string_view filters);
GE_API std::vector<std::string> openMultipleFile(std::string_view filters);
GE_API std::string openFolder(std::string_view filters);
GE_API std::string saveFile(std::string_view filters);

} // namespace GE::GUI
