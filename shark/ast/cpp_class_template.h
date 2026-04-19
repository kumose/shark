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

#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_template.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a class template.
    class CppClassTemplate final : public CppTemplate {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppClassTemplate]().
        class builder : public basic_builder<CppClassTemplate, CppClass> {
        public:
            using basic_builder::basic_builder;
        };

        /// A reference to the class that is being templated.
        const CppClass &class_() const noexcept {
            return static_cast<const CppClass &>(*begin());
        }

    private:
        CppClassTemplate(std::unique_ptr<CppClass> func)
            : CppTemplate(std::unique_ptr<CppEntity>(func.release())) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        friend basic_builder<CppClassTemplate, CppClass>;
    };

    /// A [cppast::CppEntity]() modelling a class template specialization.
    class CppClassTemplateSpecialization final : public CppTemplateSpecialization {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppClassTemplateSpecialization]().
        class builder : public specialization_builder<CppClassTemplateSpecialization, CppClass> {
        public:
            using specialization_builder::specialization_builder;
        };

        /// A reference to the class that is being specialized.
        const CppClass &class_() const noexcept {
            return static_cast<const CppClass &>(*begin());
        }

    private:
        CppClassTemplateSpecialization(std::unique_ptr<CppClass> func, CppTemplateRef primary)
            : CppTemplateSpecialization(std::unique_ptr<CppEntity>(func.release()), primary) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        friend specialization_builder<CppClassTemplateSpecialization, CppClass>;
    };
} // namespace cppast
