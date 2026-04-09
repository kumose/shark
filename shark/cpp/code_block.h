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

#pragma once

#include <vector>
#include <string>
#include <shark/cpp/block_writer.h>

namespace shark {

    class NamespaceBlock : public BlockWriter{
    public:
        std::vector<std::string> gen_lines() const noexcept override;

        std::vector<BlockWriterPtr> blocks;
        std::string name;
    };


    class MessageBlock : public BlockWriter {
    public:
        std::vector<std::string> gen_lines() const noexcept override;
        std::vector<BlockWriterPtr> publics_func;
        std::vector<BlockWriterPtr> publics_var;
        std::vector<BlockWriterPtr> publics_def;

        std::vector<BlockWriterPtr> private_def;
        std::vector<BlockWriterPtr> privates_var;
        std::vector<BlockWriterPtr> privates_func;
        std::string name;
        NamespaceBlock *namespace_block{nullptr};

       static std::vector<std::string> align_variable(const std::vector<std::string> &var);
    };
}  // namespace shark
