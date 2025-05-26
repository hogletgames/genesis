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

#include "file_dialog.h"

#include "genesis/core/defer.h"
#include "genesis/core/log.h"

#include <nfd.h>

namespace {

std::vector<std::string> toVector(const ::nfdpathset_t* path_set)
{
    std::vector<std::string> paths(::NFD_PathSet_GetCount(path_set));

    for (size_t i{0}; i < paths.size(); i++) {
        paths[i] = ::NFD_PathSet_GetPath(path_set, i);
    }

    return paths;
}

} // namespace

namespace GE::GUI {

void FileDialog::showSingleFile(std::string_view default_path, std::string_view filter_list)
{
    ::nfdchar_t* output_path{nullptr};
    Defer        output_path_defer{[output_path] { std::free(output_path); }};

    auto result = ::NFD_OpenDialog(filter_list.data(), default_path.data(), &output_path);

    switch (result) {
        case NFD_OKAY: m_single_result_file(output_path); break;
        case NFD_CANCEL: m_cancel_signal(); break;
        case NFD_ERROR: error(::NFD_GetError()); break;
        default: break;
    }
}

void FileDialog::showMultipleFiles(std::string_view default_path, std::string_view filter_list)
{
    ::nfdpathset_t path_set{};
    Defer          path_set_defer{[&path_set] { ::NFD_PathSet_Free(&path_set); }};

    auto result = ::NFD_OpenDialogMultiple(filter_list.data(), default_path.data(), &path_set);

    switch (result) {
        case NFD_OKAY: m_multiple_files_signal(toVector(&path_set)); break;
        case NFD_CANCEL: m_cancel_signal(); break;
        case NFD_ERROR: error(::NFD_GetError()); break;
        default: break;
    }
}

void FileDialog::showSelectFolder(std::string_view default_path)
{
    ::nfdchar_t* output_path{nullptr};
    Defer        output_path_defer{[output_path] { std::free(output_path); }};

    auto result = ::NFD_PickFolder(default_path.data(), &output_path);

    switch (result) {
        case NFD_OKAY: m_single_result_file(output_path); break;
        case NFD_CANCEL: m_cancel_signal(); break;
        case NFD_ERROR: error(::NFD_GetError()); break;
        default: break;
    }
}

void FileDialog::showSaveFile(std::string_view default_path, std::string_view filter_list)
{
    ::nfdchar_t* output_path{nullptr};
    Defer        output_path_defer{[output_path] { std::free(output_path); }};

    auto result = ::NFD_SaveDialog(filter_list.data(), default_path.data(), &output_path);

    switch (result) {
        case NFD_OKAY: m_single_result_file(output_path); break;
        case NFD_CANCEL: m_cancel_signal(); break;
        case NFD_ERROR: error(::NFD_GetError()); break;
        default: break;
    }
}

void FileDialog::error(std::string_view error)
{
    GE_CORE_WARN("File dialog: {}", error);
    m_error_signal(error);
}

std::string openSingleFile(std::string_view filters)
{
    std::string path;

    FileDialog dialog;
    dialog.singleResultSignal()->connect(
        [&path](std::string_view output_path) { path = std::string{output_path}; });
    dialog.showSingleFile(std::string_view{}, filters);

    return path;
}

std::vector<std::string> openMultipleFile(std::string_view filters)
{
    std::vector<std::string> paths;

    FileDialog dialog;
    dialog.multipleResultsSignal()->connect(
        [&paths](const std::vector<std::string>& output_paths) { paths = output_paths; });
    dialog.showSingleFile(std::string_view{}, filters);

    return paths;
}

std::string openFolder(std::string_view filters)
{
    std::string path;

    FileDialog dialog;
    dialog.singleResultSignal()->connect(
        [&path](std::string_view output_path) { path = std::string{output_path}; });
    dialog.showSingleFile(std::string_view{}, filters);

    return path;
}

std::string saveFile(std::string_view filters)
{
    std::string path;

    FileDialog dialog;
    dialog.singleResultSignal()->connect(
        [&path](std::string_view output_path) { path = std::string{output_path}; });
    dialog.showSaveFile(std::string_view{}, filters);

    return path;
}

} // namespace GE::GUI
