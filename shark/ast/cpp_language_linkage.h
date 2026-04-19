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

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_container.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a language linkage.
    class CppLanguageLinkage final : public CppEntity,
                                     public CppEntityContainer<CppLanguageLinkage, CppEntity> {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppLanguageLinkage]().
        class builder {
        public:
            /// \effects Sets the name, that is the kind of language linkage.
            explicit builder(std::string name) : linkage_(new CppLanguageLinkage(std::move(name))) {
            }

            /// \effects Adds an entity to the language linkage.
            void add_child(std::unique_ptr<CppEntity> child) {
                linkage_->add_child(std::move(child));
            }

            /// \returns The not yet finished language linkage.
            CppLanguageLinkage &get() const noexcept {
                return *linkage_;
            }

            /// \returns The finalized language linkage.
            /// \notes It is not registered on purpose as nothing can refer to it.
            std::unique_ptr<CppLanguageLinkage> finish() {
                return std::move(linkage_);
            }

        private:
            std::unique_ptr<CppLanguageLinkage> linkage_;
        };

        /// \returns `true` if the linkage is a block, `false` otherwise.
        bool is_block() const noexcept;

    private:
        using CppEntity::CppEntity;

        CppEntityKind do_get_entity_kind() const noexcept override;
    };
} // namespace cppast
