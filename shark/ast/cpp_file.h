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

#include <vector>

#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_entity_ref.h>

namespace cppast {
    /// An unmatched documentation comment.
    struct CppDocComment {
        std::string content;
        unsigned line;

        CppDocComment(std::string content, unsigned line) : content(std::move(content)), line(line) {
        }
    };

    /// A [cppast::CppEntity]() modelling a file.
    ///
    /// This is the top-level entity of the AST.
    class CppFile final : public CppEntity, public CppEntityContainer<CppFile, CppEntity> {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppFile]().
        class builder {
        public:
            /// \effects Sets the file name.
            explicit builder(std::string name) : file_(new CppFile(std::move(name))) {
            }

            /// \effects Adds an entity.
            void add_child(std::unique_ptr<CppEntity> child) noexcept {
                file_->add_child(std::move(child));
            }

            /// \effects Adds an unmatched documentation comment.
            void add_unmatched_comment(CppDocComment comment) {
                file_->comments_.push_back(std::move(comment));
            }

            /// \returns The not yet finished file.
            CppFile &get() noexcept {
                return *file_;
            }

            /// \effects Registers the file in the [cppast::CppEntityIndex]().
            /// It will use the file name as identifier.
            /// \returns The finished file, or `nullptr`, if that file was already registered.
            std::unique_ptr<CppFile> finish(const CppEntityIndex &idx) noexcept {
                auto res = idx.register_file(CppEntityId(file_->name()), type_safe::ref(*file_));
                return res ? std::move(file_) : nullptr;
            }

        private:
            std::unique_ptr<CppFile> file_;
        };

        /// \returns The unmatched documentation comments.
        type_safe::array_ref<const CppDocComment> unmatched_comments() const noexcept {
            return type_safe::ref(comments_.data(), comments_.size());
        }

    private:
        CppFile(std::string name) : CppEntity(std::move(name)) {
        }

        /// \returns [cpp_entity_type::file_t]().
        CppEntityKind do_get_entity_kind() const noexcept override;

        std::vector<CppDocComment> comments_;
    };

    /// \exclude
    namespace detail {
        struct CppFileRefPredicate {
            bool operator()(const CppEntity &e);
        };
    } // namespace detail

    /// A reference to a [cppast::CppFile]().
    using CppFileRef = BasicCppEntityRef<CppFile, detail::CppFileRefPredicate>;
} // namespace cppast
