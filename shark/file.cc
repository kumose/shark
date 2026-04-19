// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#include <shark/file.h>
#include <shark/default_generator.h>

namespace cppast {
    FileBuilder::FileBuilder(std::string_view filename) : _id(get_seq_entity_id(filename)), _filename(filename),
                                                          _builder(_filename) {
    }

    FileBuilder &FileBuilder::add_child(std::unique_ptr<CppEntity> child) noexcept {
        _builder.add_child(std::move(child));
        return *this;
    }

    FileBuilder &FileBuilder::include(std::string_view full_path, bool sys) noexcept {
        if (sys) {
            _includer.system_include(full_path);
        } else {
            _includer.local_include(full_path);
        }
        return *this;
    }

    FileBuilder &FileBuilder::pragma_once() noexcept {
        _header.push_back(CppMacroParameter::build("#pragma once"));
        return *this;
    }

    FileBuilder &FileBuilder::header_comment(std::string_view comment) noexcept {
        _header.push_back(CppComment::build(std::string(comment)));
        return *this;
    }

    FileBuilder &FileBuilder::comment(std::string_view comment) noexcept {
        _body.push_back(CppComment::build(std::string(comment)));
        return *this;
    }

    FileBuilder &FileBuilder::add_body(std::unique_ptr<CppEntity> child) noexcept {
        _body.push_back(std::move(child));
        return *this;
    }

    std::unique_ptr<CppFile> FileBuilder::finish() noexcept {
        for (auto &it : _header) {
            add_child(std::move(it));
        }
        process_includes();
        for (auto &it : _body) {
            add_child(std::move(it));
        }
        CppEntityIndex idx;
        return _builder.finish(idx);
    }

    void FileBuilder::process_includes() noexcept {
        for (auto &it : _includer.system_includes()) {
            CppEntityId fid = get_seq_entity_id(it);
            CppFileRef ref(fid, std::string(it));
            auto inc = CppIncludeDirective::build(ref, CppIncludeKind::system, std::string(it));
            add_child(std::move(inc));
        }
        for (auto &it : _includer.local_includes()) {
            CppEntityId fid = get_seq_entity_id(it);
            CppFileRef ref(fid, std::string(it));
            auto inc = CppIncludeDirective::build(ref, CppIncludeKind::local, std::string(it));
            add_child(std::move(inc));
        }
    }
} // namespace cppast
