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

#include <shark/cpp/code_writer.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/string_builder.h>
#include <turbo/strings/str_join.h>
#include <turbo/strings/match.h>
#include <memory>
#include <vector>


namespace shark {

    std::string get_file_prefix(const google::protobuf::FileDescriptor *file_descriptor) {
            constexpr std::string_view kSuffix = ".proto";
            auto prefix = file_descriptor->name();
            if (turbo::ends_with(prefix, kSuffix)) {
                prefix.erase(prefix.end() - kSuffix.size(), prefix.end());
            }
            return prefix;
        }

    bool CodeWriter::init(const google::protobuf::FileDescriptor *file_descriptor,
                  google::protobuf::compiler::GeneratorContext *generator_context) {
        _generator_context = generator_context;
        _file = file_descriptor;
        _filename_prefix = get_file_prefix(file_descriptor);
        return _option.init(file_descriptor);
    }

     void CodeWriter::flush() {
                    // Header.
            auto hdr = open_with_extension(".sk.h");
            google::protobuf::io::CodedOutputStream hdr_stream(hdr.get());
            hdr_stream.WriteString(flush_header());

            // Source file.
            auto src = open_with_extension(".sk.cc");
            google::protobuf::io::CodedOutputStream src_stream(src.get());
            src_stream.WriteString(flush_source());
     }

    std::string CodeWriter::flush_header() const {
        turbo::StringBuilder builder;
        for(auto &it : _header_region.header_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                builder<<vit<<"\n";
            }
        }

        builder<<"\n";
        std::unordered_map<std::string, std::string> _headers;
        for(auto &it : _header_region.include_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                auto mip = _headers.find(vit);
                if (mip != _headers.end()) {
                    continue;
                }
                builder<<vit<<"\n";
                _headers.insert(std::make_pair(vit, "1"));
            }
        }

        builder<<"\n";
        for(auto &it : _header_region.namespace_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                builder<<vit<<"\n";
            }
        }


        builder<<"\n";
        return builder.str();
    }

    std::string CodeWriter::flush_source() const {
        turbo::StringBuilder builder;
        for(auto &it : _source_region.header_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                builder<<vit<<"\n";
            }
        }

        builder<<"\n";
        for(auto &it : _source_region.include_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                builder<<vit<<"\n";
            }
        }

        builder<<"\n";
        for(auto &it : _source_region.namespace_blocks) {
            auto v  = it->gen_lines();
            for(auto &vit : v) {
                builder<<vit<<"\n";
            }
        }


        builder<<"\n";
        return builder.str();
    }

}  // namespace shark

