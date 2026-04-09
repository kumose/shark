// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <shark/cpp/code_block.h>
#include <turbo/strings/str_format.h>

namespace shark {

    std::vector<std::string> NamespaceBlock::gen_lines() const noexcept {
        std::vector<std::string> lines;
        lines.push_back(turbo::str_format("namespace %s {", name));
        lines.emplace_back("");
        for (auto & it : blocks) {
            auto g = it->gen_lines();
            for (auto & sit : g) {
                lines.push_back(turbo::str_format("%s%s", kIndent, sit));
            }
        }
        lines.emplace_back("");
        lines.push_back(turbo::str_format("}  // namespace %s", name));
        lines.push_back(turbo::str_format(""));
        return lines;
    }

    std::vector<std::string> MessageBlock::gen_lines() const noexcept {
        std::vector<std::string> lines;
        lines.push_back(turbo::str_format("class %s {", name));
        if (!publics_def.empty()) {
            lines.emplace_back("public:");
            lines.emplace_back(turbo::str_format("%s/// public declares", kIndent));
            for (auto & it : publics_def) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    lines.push_back(turbo::str_format("%s%s", kIndent, sit));
                }
            }
            lines.emplace_back("");
        }
        if (!private_def.empty()) {
            lines.emplace_back("private:");
            lines.emplace_back(turbo::str_format("%s/// private declares", kIndent));
            for (auto & it : private_def) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    lines.push_back(turbo::str_format("%s%s", kIndent, sit));
                }
            }
            lines.emplace_back("");
        }
        if (!publics_func.empty()) {
            lines.emplace_back("public:");
            lines.emplace_back(turbo::str_format("%s/// public functions declares", kIndent));
            for (auto & it : publics_func) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    lines.push_back(turbo::str_format("%s%s", kIndent, sit));
                }
            }
            lines.emplace_back("");
        }
        if (!privates_func.empty()) {
            lines.emplace_back("private:");
            lines.emplace_back(turbo::str_format("%s/// private functions declares", kIndent));
            for (auto & it : privates_func) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    lines.push_back(turbo::str_format("%s%s", kIndent, sit));
                }
            }
            lines.emplace_back("");
        }
        if (!publics_var.empty()) {
            lines.emplace_back("public:");
            lines.emplace_back(turbo::str_format("%s/// public variable declares", kIndent));
            std::vector<std::string> pvars;
            for (auto & it : publics_var) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    pvars.push_back(sit);
                }
            }
            auto apv = align_variable(pvars);
            for (auto & sit : apv) {
                lines.push_back(turbo::str_format("%s%s", kIndent, sit));
            }
            lines.emplace_back("");
        }
        if (!privates_var.empty()) {
            lines.emplace_back("private:");
            lines.emplace_back(turbo::str_format("%s/// private variable declares", kIndent));
            std::vector<std::string> pvars;
            for (auto & it : privates_var) {
                auto g = it->gen_lines();
                for (auto & sit : g) {
                    pvars.push_back(sit);
                }
            }
            auto apv = align_variable(pvars);
            for (auto & sit : apv) {
                lines.push_back(turbo::str_format("%s%s", kIndent, sit));
            }
            lines.emplace_back("");
        }
        lines.push_back(turbo::str_format("};"));
        lines.emplace_back("");
        return lines;
    }

    static bool is_comment(const std::string &line) {
        auto trimd = turbo::trim_left(line);

        if (turbo::starts_with(trimd, "//") || turbo::starts_with(trimd, "/*")) {
            return true;
        }

        return false;
    }
    std::vector<std::string> MessageBlock::align_variable(const std::vector<std::string> &var) {
        std::vector<std::string> result;
        result.resize(var.size());
        std::vector<std::vector<std::string>> mid;
        mid.resize(var.size());
        size_t max_len = 0;
        for (size_t i = 0; i < var.size(); i++) {
            if (is_comment(var[i])) {
                result[i] = var[i];
                continue;
            }
            mid[i] = turbo::str_split(var[i], " ", turbo::SkipEmpty{});
            if (!mid[i].empty() && mid[i][0].size() > max_len) {
                max_len = mid[i][0].size();
            }
        }

        for (size_t i = 0; i < var.size(); i++) {
            if (mid[i].empty()) {
                continue;
            }
            std::string ow;
            if (mid[i].size() == 2) {
                ow = mid[i][0];
            } else if (mid[i].size() >= 3) {
                ow = turbo::str_format("%s %s", mid[i][0], mid[i][1]);
            }
            auto w = turbo::str_format("%s%s    ", ow, std::string(max_len - ow.size(), ' '));
            mid[i][0] = w;
            if (mid[i].size() >= 3) {
                mid[i][1].clear();
            }
            result[i]  = turbo::str_join(mid[i], "");
        }
        return result;
    }
}  // namespace shark
