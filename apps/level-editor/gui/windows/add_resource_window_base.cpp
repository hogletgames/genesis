/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include "add_resource_window_base.h"
#include "level_editor_context.h"

#include "genesis/assets/package.h"
#include "genesis/gui/widgets.h"

#include <algorithm>

using namespace GE::GUI;
using namespace GE::Assets;

namespace LE {
namespace {

std::vector<std::string_view> getPackageNames(LevelEditorContext* ctx)
{
    auto                          packages = ctx->assets()->allPackages();
    std::vector<std::string_view> package_names(packages.size());
    std::ranges::transform(packages, package_names.begin(),
                           [](const auto* package) { return std::string_view{package->name()}; });

    return package_names;
}

} // namespace

AddResourceWindowBase::AddResourceWindowBase(std::string_view name, LevelEditorContext* ctx)
    : WindowBase{name}
    , m_ctx{ctx}
{}

void AddResourceWindowBase::renderPackageCombobox(WidgetNode* node)
{
    auto package_names = getPackageNames(m_ctx);

    if (m_package_name.empty() && !package_names.empty()) {
        m_package_name = package_names.front();
    }

    ComboBox packages_combobox{"Package", package_names, m_package_name};
    node->subNode(&packages_combobox);
    if (packages_combobox.selectedItem() != m_package_name) {
        m_package_name = packages_combobox.selectedItem();
    }
}

} // namespace LE
