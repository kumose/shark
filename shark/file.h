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


#pragma once

#include <shark/ast/code_generator.h>
#include <shark/ast/cpp_file.h>
#include <shark/ast/cpp_namespace.h>
#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_preprocessor.h>
#include <turbo/strings/string_builder.h>
#include <shark/includer.h>

namespace cppast {
    class FileBuilder {
    public:
        explicit FileBuilder(std::string_view filename);

        FileBuilder &include(std::string_view full_path, bool sys = true) noexcept;

        FileBuilder &pragma_once() noexcept;

        FileBuilder &header_comment(std::string_view comment) noexcept;

        FileBuilder &comment(std::string_view comment) noexcept;

        FileBuilder &add_body(std::unique_ptr<CppEntity> child) noexcept;

        std::unique_ptr<CppFile> finish() noexcept;

    private:
        void process_includes() noexcept;

        FileBuilder &add_child(std::unique_ptr<CppEntity> child) noexcept;

    private:
        CppEntityId _id;
        std::string _filename;
        CppFile::builder _builder;
        Includer _includer;
        std::vector<std::unique_ptr<CppEntity> > _header;
        std::vector<std::unique_ptr<CppEntity> > _body;
    };
} // namespace cppast
