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


#include <shark/ast/cpp_function.h>
#include <shark/ast/cpp_template.h>

namespace cppast
{
/// A [cppast::CppEntity]() modelling a function template.
class CppFunctionTemplate final : public CppTemplate
{
public:
    static CppEntityKind kind() noexcept;

    /// Builder for [cppast::CppFunctionTemplate]().
    class builder : public basic_builder<CppFunctionTemplate, CppFunctionBase>
    {
    public:
        using basic_builder::basic_builder;
    };

    /// A reference to the function that is being templated.
    const CppFunctionBase& function() const noexcept
    {
        return static_cast<const CppFunctionBase&>(*begin());
    }

private:
    CppFunctionTemplate(std::unique_ptr<CppFunctionBase> func)
    : CppTemplate(std::unique_ptr<CppEntity>(func.release()))
    {}

    CppEntityKind do_get_entity_kind() const noexcept override;

    friend basic_builder<CppFunctionTemplate, CppFunctionBase>;
};

/// A [cppast::CppEntity]() modelling a function template specialization.
class CppFunctionTemplateSpecialization final : public CppTemplateSpecialization
{
public:
    static CppEntityKind kind() noexcept;

    /// Builder for [cppast::CppFunctionTemplateSpecialization]().
    class builder
    : public specialization_builder<CppFunctionTemplateSpecialization, CppFunctionBase>
    {
    public:
        using specialization_builder::specialization_builder;

    private:
        using specialization_builder::add_parameter;
    };

    /// A reference to the function that is being specialized.
    const CppFunctionBase& function() const noexcept
    {
        return static_cast<const CppFunctionBase&>(*begin());
    }

private:
    CppFunctionTemplateSpecialization(std::unique_ptr<CppFunctionBase> func,
                                         CppTemplateRef                   primary)
    : CppTemplateSpecialization(std::unique_ptr<CppEntity>(func.release()), primary)
    {}

    CppEntityKind do_get_entity_kind() const noexcept override;

    friend specialization_builder<CppFunctionTemplateSpecialization, CppFunctionBase>;
};
} // namespace cppast

