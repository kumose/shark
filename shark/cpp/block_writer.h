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

#include <memory>
#include <string_view>
#include <string>
#include <turbo/strings/str_split.h>
#include <turbo/strings/string_builder.h>

namespace shark {

    class BlockWriter {
    public:

        static constexpr  std::string_view kIndent = "    ";
        static constexpr  std::string_view kHardLine = "$HARDLINE";

        virtual ~BlockWriter() = default;

        virtual std::vector<std::string> gen_lines() const noexcept = 0;
    };

    using  BlockWriterPtr = std::unique_ptr<BlockWriter>;

    class PlainBlockWriter : public BlockWriter {
    public:
        
    PlainBlockWriter(std::string str) : _str(str){}

        ~PlainBlockWriter() override = default;

        std::vector<std::string> gen_lines()  const noexcept override{
            std::vector<std::string> lines = turbo::str_split(_str, '\n');
            return lines;
        }

    private:
        std::string _str;
    };

    class LinesBlockWriter : public BlockWriter {
    public:

        LinesBlockWriter(std::vector<std::string> &&l) :lines(l){}

        ~LinesBlockWriter() override = default;

        std::vector<std::string> gen_lines()  const noexcept override{
            return lines;
        }

    private:
        std::vector<std::string>  lines;
    };
    class LinesBuilder;

    class LinseBuilderStream {
    public:
        LinseBuilderStream(LinesBuilder *b, const std::string &in) : builder(b) {
            _str<<in;
        }
        ~LinseBuilderStream();

        template <typename T>
        LinseBuilderStream& operator<<(T v) {
            _str<<v;
            return *this;
        }

        LinesBuilder *builder;
        turbo::StringBuilder _str;
    };

    class LinesBuilder {
    public:
        LinesBuilder()  = default;

        void push_back(std::string line) {
            lines.push_back(line);
        }

        LinseBuilderStream stream() {
            return LinseBuilderStream(this ,std::string(_indent * 4, ' '));
        }

        BlockWriterPtr build() {
            return std::make_unique<LinesBlockWriter>(std::move(lines));
        }

        void indent() {
            _indent++;
        }
        void deindent() {
            _indent--;
        }
        std::vector<std::string> lines;

        int _indent{0};

    };

   inline  LinseBuilderStream::~LinseBuilderStream() {
        builder->push_back(_str.str());
    }
}  // namespace shark

