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
#include <turbo/strings/string_builder.h>

namespace cppast {
    class DefaultGenerator : public cppast::CodeGenerator {
    public:
        const std::string &str() const noexcept {
            return str_;
        }

        void build() {
            if (str_.empty()) {
                str_ = std::move(builder_).str();
            }
        }

    private:
        GenerationOptions do_get_options(const cppast::CppEntity &,
                                         cppast::CppAccessSpecifierKind) override {
            return options_;
        }

        void do_indent() override {
            ++indent_;
        }

        void do_unindent() override {
            if (indent_)
                --indent_;
        }

        void do_write_token_seq(cppast::string_view tokens) override {
            if (was_newline_) {
                builder_ << std::string(indent_ * 4u, ' ');
                was_newline_ = false;
            }
            builder_ << tokens.c_str();
        }

        void do_write_newline() override {
            builder_ << "\n";
            was_newline_ = true;
        }

    private:
        std::string str_;
        turbo::StringBuilder builder_;
        GenerationOptions options_;
        unsigned indent_ = 0;
        bool was_newline_ = false;
    };

    cppast::CppEntityId get_seq_entity_id(std::string_view id = "");
} // namespace cppast
