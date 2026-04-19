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
#include <shark/ast/cpp_expression.h>
#include <shark/ast/cpp_template_parameter.h>

namespace cppast {
    /// A [[cppast::CppEntity]() modelling a c++ concept declaration
    /// \notes while concepts are technically templates,
    /// this is not a [cppast::CppTemplate](),
    /// as concepts act very differently from other templates
    class CppConcept final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns the template parameters as a string
        const CppTokenString &parameters() const noexcept {
            return parameters_;
        }

        /// \returns the [cppast::CppExpression]() defining the concept constraint
        const CppExpression &constraint_expression() const noexcept {
            return *expression_;
        }

        class builder {
        public:
            builder(std::string name) : concept_(new CppConcept(std::move(name))) {
            }

            CppTokenString &set_parameters(CppTokenString string) noexcept {
                concept_->parameters_ = std::move(string);
                return concept_->parameters_;
            }

            CppExpression &set_expression(std::unique_ptr<CppExpression> expression) noexcept {
                concept_->expression_ = std::move(expression);
                return *concept_->expression_;
            }

            std::unique_ptr<CppConcept> finish(const CppEntityIndex &idx, CppEntityId id);

        private:
            std::unique_ptr<CppConcept> concept_;
        };

    private:
        CppConcept(std::string name) : CppEntity(std::move(name)), parameters_({}) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppTokenString parameters_;

        std::unique_ptr<CppExpression> expression_;
    };
} // namespace cppast
