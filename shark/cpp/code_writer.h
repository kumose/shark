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

#include <shark/cpp/code_block.h>
#include <shark/cpp/km_option.h>
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <memory>
#include <vector>

namespace shark {

    std::string get_file_prefix(const google::protobuf::FileDescriptor *file_descriptor);

    struct CodeRegion {
        std::vector<BlockWriterPtr> header_blocks;
        std::vector<BlockWriterPtr> include_blocks;
        std::vector<BlockWriterPtr> namespace_blocks;
    };

    class CodeWriter {
    public:
        static CodeWriter & get_instance() {
            static  CodeWriter ins;
            return ins;
        }

        ~CodeWriter() = default;

        bool init(const google::protobuf::FileDescriptor *file_descriptor,
                   google::protobuf::compiler::GeneratorContext *generator_context);

        void flush();

        void add_header_header(BlockWriterPtr ptr) {
            _header_region.header_blocks.push_back(std::move(ptr));
        }

        void add_header_include(BlockWriterPtr ptr){

            _header_region.include_blocks.push_back(std::move(ptr));
        }


        void add_header_namespace(std::unique_ptr<NamespaceBlock> ptr) {
            _header_region.namespace_blocks.push_back(std::move(ptr));
        }

        void add_source_header(BlockWriterPtr ptr) {
            _source_region.header_blocks.push_back(std::move(ptr));
        }

        void add_source_include(BlockWriterPtr ptr) {
            _source_region.include_blocks.push_back(std::move(ptr));
        }

        void add_source_namespace(std::unique_ptr<NamespaceBlock> ptr) {
            _source_region.namespace_blocks.push_back(std::move(ptr));
        }

        const KmgenOption& option() const{
            return _option;
        }
    private:
        CodeWriter() {}
        std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> open_with_extension(
            const std::string &ext) {
            return std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream>(
                _generator_context->Open(_filename_prefix + ext));
        }

        std::string flush_header() const;
        std::string flush_source() const;
    private:
        CodeRegion _header_region;
        CodeRegion _source_region;
        
        google::protobuf::compiler::GeneratorContext *_generator_context{nullptr};
        const google::protobuf::FileDescriptor *_file{nullptr};
        std::string _filename_prefix;
        KmgenOption _option;
    };

}  // namespace shark
