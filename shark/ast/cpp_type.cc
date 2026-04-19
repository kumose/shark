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


#include <shark/ast/cpp_type.h>

#include <shark/ast/cpp_array_type.h>
#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_decltype_type.h>
#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_function_type.h>
#include <shark/ast/cpp_template.h>
#include <shark/ast/cpp_type_alias.h>

using namespace cppast;

const char *cppast::to_string(CppBuiltinTypeKind kind) noexcept {
    switch (kind) {
        case cpp_void:
            return "void";

        case cpp_bool:
            return "bool";

        case cpp_uchar:
            return "unsigned char";
        case cpp_ushort:
            return "unsigned short";
        case cpp_uint:
            return "unsigned int";
        case cpp_ulong:
            return "unsigned long";
        case cpp_ulonglong:
            return "unsigned long long";
        case cpp_uint128:
            return "unsigned __int128";

        case cpp_schar:
            return "signed char";
        case cpp_short:
            return "short";
        case cpp_int:
            return "int";
        case cpp_long:
            return "long";
        case cpp_longlong:
            return "long long";
        case cpp_int128:
            return "__int128";

        case cpp_float:
            return "float";
        case cpp_double:
            return "double";
        case cpp_longdouble:
            return "long double";
        case cpp_float128:
            return "__float128";

        case cpp_char:
            return "char";
        case cpp_wchar:
            return "wchar_t";
        case cpp_char16:
            return "char16_t";
        case cpp_char32:
            return "char32_t";

        case cpp_nullptr:
            return "std::nullptr_t";
    }
    DEBUG_UNREACHABLE(detail::assert_handler{});
    return "__ups";
}

const CppType &cppast::remove_cv(const CppType &type) noexcept {
    if (type.kind() == CppTypeKind::cv_qualified_t) {
        auto &cv = static_cast<const CppCvQualifiedType &>(type);
        return cv.type();
    }

    return type;
}

const CppType &cppast::remove_const(const CppType &type) noexcept {
    if (type.kind() == CppTypeKind::cv_qualified_t) {
        auto &cv = static_cast<const CppCvQualifiedType &>(type);
        if (is_const(cv.cv_qualifier()))
            return cv.type();
    }
    return type;
}

const CppType &cppast::remove_volatile(const CppType &type) noexcept {
    if (type.kind() == CppTypeKind::cv_qualified_t) {
        auto &cv = static_cast<const CppCvQualifiedType &>(type);
        if (is_volatile(cv.cv_qualifier()))
            return cv.type();
    }
    return type;
}

bool detail::cpp_type_ref_predicate::operator()(const CppEntity &e) {
    switch (e.kind()) {
        case CppEntityKind::type_alias_t:
        case CppEntityKind::enum_t:
        case CppEntityKind::class_t:
            return true;

        case CppEntityKind::file_t:
        case CppEntityKind::macro_parameter_t:
        case CppEntityKind::macro_definition_t:
        case CppEntityKind::include_directive_t:
        case CppEntityKind::language_linkage_t:
        case CppEntityKind::namespace_t:
        case CppEntityKind::namespace_alias_t:
        case CppEntityKind::using_directive_t:
        case CppEntityKind::using_declaration_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::access_specifier_t:
        case CppEntityKind::base_class_t:
        case CppEntityKind::variable_t:
        case CppEntityKind::member_variable_t:
        case CppEntityKind::bitfield_t:
        case CppEntityKind::function_parameter_t:
        case CppEntityKind::function_t:
        case CppEntityKind::member_function_t:
        case CppEntityKind::conversion_op_t:
        case CppEntityKind::constructor_t:
        case CppEntityKind::destructor_t:
        case CppEntityKind::friend_t:
        case CppEntityKind::template_type_parameter_t:
        case CppEntityKind::non_type_template_parameter_t:
        case CppEntityKind::template_template_parameter_t:
        case CppEntityKind::alias_template_t:
        case CppEntityKind::variable_template_t:
        case CppEntityKind::function_template_t:
        case CppEntityKind::function_template_specialization_t:
        case CppEntityKind::class_template_t:
        case CppEntityKind::class_template_specialization_t:
        case CppEntityKind::concept_t:
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
        case CppEntityKind::count:
            break;
    }

    return false;
}

std::unique_ptr<CppDependentType> CppDependentType::build(
    std::string name, std::unique_ptr<CppTemplateParameterType> dependee) {
    return std::unique_ptr<CppDependentType>(
        new CppDependentType(std::move(name), std::move(dependee)));
}

std::unique_ptr<CppDependentType> CppDependentType::build(
    std::string name, std::unique_ptr<CppTemplateInstantiationType> dependee) {
    return std::unique_ptr<CppDependentType>(
        new CppDependentType(std::move(name), std::move(dependee)));
}

namespace {
    // is directly a complex type
    // is_complex_type also checks for children
    bool is_direct_complex(const CppType &type) noexcept {
        switch (type.kind()) {
            case CppTypeKind::builtin_t:
            case CppTypeKind::user_defined_t:
            case CppTypeKind::auto_t:
            case CppTypeKind::decltype_t:
            case CppTypeKind::decltype_auto_t:
            case CppTypeKind::cv_qualified_t:
            case CppTypeKind::pointer_t:
            case CppTypeKind::reference_t:
            case CppTypeKind::template_parameter_t:
            case CppTypeKind::template_instantiation_t:
            case CppTypeKind::dependent_t:
            case CppTypeKind::unexposed_t:
                return false;

            case CppTypeKind::array_t:
            case CppTypeKind::function_t:
            case CppTypeKind::member_function_t:
            case CppTypeKind::member_object_t:
                return true;
        }

        DEBUG_UNREACHABLE(detail::assert_handler{});
        return false;
    }
} // namespace

bool detail::is_complex_type(const CppType &type) noexcept {
    switch (type.kind()) {
        case CppTypeKind::cv_qualified_t:
            return is_complex_type(static_cast<const CppCvQualifiedType &>(type).type());
        case CppTypeKind::pointer_t:
            return is_complex_type(static_cast<const CppPointerType &>(type).pointee());
        case CppTypeKind::reference_t:
            return is_complex_type(static_cast<const CppReferenceType &>(type).referee());

        default:
            break;
    }

    return is_direct_complex(type);
}

namespace {
    void comma(const CodeGenerator::output &output) {
        output << punctuation(",");
        if (output.formatting().is_set(FormattingFlags::comma_ws))
            output << whitespace;
    }

    void bracket_ws(const CodeGenerator::output &output) {
        if (output.formatting().is_set(FormattingFlags::bracket_ws))
            output << whitespace;
    }

    void operator_ws(const CodeGenerator::output &output) {
        if (output.formatting().is_set(FormattingFlags::operator_ws))
            output << whitespace;
    }

    void write_builtin(CodeGenerator::output &output, const CppBuiltinType &type) {
        output << keyword(to_string(type.builtin_type_kind()));
    }

    void write_user_defined(CodeGenerator::output &output, const CppUserDefinedType &type) {
        output << type.entity();
    }

    void write_auto(CodeGenerator::output &output, const CppAutoType &) {
        output << keyword("auto");
    }

    void write_decltype(CodeGenerator::output &output, const CppDecltypeType &type) {
        output << keyword("decltype") << punctuation("(") << bracket_ws;
        detail::write_expression(output, type.expression());
        output << bracket_ws << punctuation(")");
    }

    void write_decltype_auto(CodeGenerator::output &output, const CppDecltypeAutoType &) {
        output << keyword("decltype") << punctuation("(") << bracket_ws << keyword("auto") << bracket_ws
                << punctuation(")");
    }

    void write_cv_qualified_prefix(CodeGenerator::output &output, const CppCvQualifiedType &type) {
        detail::write_type_prefix(output, type.type());

        if (is_direct_complex(type.type()))
            output << punctuation("(") << bracket_ws;

        if (is_const(type.cv_qualifier()))
            output << whitespace << keyword("const");
        if (is_volatile(type.cv_qualifier()))
            output << whitespace << keyword("volatile");
    }

    void write_cv_qualified_suffix(CodeGenerator::output &output, const CppCvQualifiedType &type) {
        if (is_direct_complex(type.type()))
            output << bracket_ws << punctuation(")");
        detail::write_type_suffix(output, type.type());
    }

    bool pointer_requires_paren(const CppPointerType &type) {
        auto kind = type.pointee().kind();
        return kind == CppTypeKind::function_t || kind == CppTypeKind::array_t;
    }

    void write_pointer_prefix(CodeGenerator::output &output, const CppPointerType &type) {
        detail::write_type_prefix(output, type.pointee());

        if (pointer_requires_paren(type))
            output << punctuation("(") << bracket_ws;
        else if (output.formatting().is_set(FormattingFlags::ptr_ref_var))
            output << whitespace;

        output << punctuation("*");
    }

    void write_pointer_suffix(CodeGenerator::output &output, const CppPointerType &type) {
        if (pointer_requires_paren(type))
            output << bracket_ws << punctuation(")");
        detail::write_type_suffix(output, type.pointee());
    }

    void write_reference_prefix(CodeGenerator::output &output, const CppReferenceType &type) {
        detail::write_type_prefix(output, type.referee());

        if (is_direct_complex(type.referee()))
            output << punctuation("(") << bracket_ws;
        else if (output.formatting().is_set(FormattingFlags::ptr_ref_var))
            output << whitespace;

        if (type.reference_kind() == cpp_ref_lvalue)
            output << punctuation("&");
        else if (type.reference_kind() == cpp_ref_rvalue)
            output << punctuation("&&");
        else
            DEBUG_UNREACHABLE(detail::assert_handler{});
    }

    void write_reference_suffix(CodeGenerator::output &output, const CppReferenceType &type) {
        if (is_direct_complex(type.referee()))
            output << bracket_ws << punctuation(")");
        detail::write_type_suffix(output, type.referee());
    }

    void write_array_prefix(CodeGenerator::output &output, const CppArrayType &type) {
        detail::write_type_prefix(output, type.value_type());
    }

    void write_array_suffix(CodeGenerator::output &output, const CppArrayType &type) {
        output << punctuation("[");
        if (type.size()) {
            output << bracket_ws;
            detail::write_expression(output, type.size().value());
            output << bracket_ws;
        }
        output << punctuation("]");
        detail::write_type_suffix(output, type.value_type());
    }

    void write_function_prefix(CodeGenerator::output &output, const CppFunctionType &type) {
        detail::write_type_prefix(output, type.return_type());
    }

    template<typename T>
    void write_parameters(CodeGenerator::output &output, const T &type) {
        output << punctuation("(") << bracket_ws;

        auto need_sep = false;
        for (auto &param: type.parameter_types()) {
            if (need_sep)
                output << comma;
            else
                need_sep = true;
            detail::write_type_prefix(output, param);
            detail::write_type_suffix(output, param);
        }
        if (type.is_variadic()) {
            if (need_sep)
                output << comma;
            output << punctuation("...");
        }

        output << bracket_ws << punctuation(")");
    }

    void write_function_suffix(CodeGenerator::output &output, const CppFunctionType &type) {
        write_parameters(output, type);

        detail::write_type_suffix(output, type.return_type());
    }

    const CppType &strip_class_type(const CppType &type, CppCv *cv, CppReference *ref) {
        if (type.kind() == CppTypeKind::cv_qualified_t) {
            auto &cv_qual = static_cast<const CppCvQualifiedType &>(type);
            if (cv)
                *cv = cv_qual.cv_qualifier();
            return strip_class_type(cv_qual.type(), cv, ref);
        } else if (type.kind() == CppTypeKind::reference_t) {
            auto &ref_type = static_cast<const CppReferenceType &>(type);
            if (ref)
                *ref = ref_type.reference_kind();
            return strip_class_type(ref_type.referee(), cv, ref);
        } else {
            DEBUG_ASSERT(!detail::is_complex_type(type), detail::assert_handler{});
            return type;
        }
    }

    void write_member_function_prefix(CodeGenerator::output &output,
                                      const CppMemberFunctionType &type) {
        detail::write_type_prefix(output, type.return_type());

        output << punctuation("(") << bracket_ws;
        detail::write_type_prefix(output, strip_class_type(type.class_type(), nullptr, nullptr));
        output << punctuation("::");
    }

    void write_member_function_suffix(CodeGenerator::output &output,
                                      const CppMemberFunctionType &type) {
        output << bracket_ws << punctuation(")");
        write_parameters(output, type);

        auto cv = cpp_cv_none;
        auto ref = cpp_ref_none;
        strip_class_type(type.class_type(), &cv, &ref);

        if (cv == cpp_cv_const_volatile)
            output << keyword("const") << whitespace << keyword("volatile");
        else if (is_const(cv))
            output << keyword("const");
        else if (is_volatile(cv))
            output << keyword("volatile");

        if (ref == cpp_ref_lvalue)
            output << operator_ws << punctuation("&") << operator_ws;
        else if (ref == cpp_ref_rvalue)
            output << operator_ws << punctuation("&&") << operator_ws;

        detail::write_type_suffix(output, type.return_type());
    }

    void write_member_object_prefix(CodeGenerator::output &output, const CppMemberObjectType &type) {
        detail::write_type_prefix(output, type.object_type());
        output << punctuation("(") << bracket_ws;
        DEBUG_ASSERT(!detail::is_complex_type(type.class_type()), detail::assert_handler{});
        detail::write_type_prefix(output, type.class_type());
        output << punctuation("::");
    }

    void write_member_object_suffix(CodeGenerator::output &output, const CppMemberObjectType &) {
        output << bracket_ws << punctuation(")");
    }

    void write_template_parameter(CodeGenerator::output &output,
                                  const CppTemplateParameterType &type) {
        output << type.entity();
    }

    void write_template_instantiation(CodeGenerator::output &output,
                                      const CppTemplateInstantiationType &type) {
        output << type.primary_template();
        if (output.was_reference_excluded())
            return;

        if (type.arguments_exposed())
            detail::write_template_arguments(output, type.arguments());
        else
            output << punctuation("<") << bracket_ws << token_seq(type.unexposed_arguments())
                    << bracket_ws << punctuation(">");
    }

    void write_dependent(CodeGenerator::output &output, const CppDependentType &type) {
        output << keyword("typename") << whitespace;
        detail::write_type(output, type.dependee(), "");
        output << punctuation("::") << identifier(type.name());
    }

    void write_unexposed(CodeGenerator::output &output, const CppUnexposedType &type) {
        output << token_seq(type.name());
    }
} // namespace

void detail::write_type_prefix(CodeGenerator::output &output, const CppType &type) {
    switch (type.kind()) {
        case CppTypeKind::builtin_t: write_builtin(output, static_cast<const CppBuiltinType &>(type));
            break;
        case CppTypeKind::user_defined_t:
            write_user_defined(output, static_cast<const CppUserDefinedType &>(type));
            break;
        case CppTypeKind::auto_t: write_auto(output, static_cast<const CppAutoType &>(type));
            break;
        case CppTypeKind::decltype_t: write_decltype(output, static_cast<const CppDecltypeType &>(type));
            break;
        case CppTypeKind::decltype_auto_t:
            write_decltype_auto(output, static_cast<const CppDecltypeAutoType &>(type));
            break;
        case CppTypeKind::cv_qualified_t: write_cv_qualified_prefix(
                output, static_cast<const CppCvQualifiedType &>(type));
            break;
        case CppTypeKind::pointer_t: write_pointer_prefix(output, static_cast<const CppPointerType &>(type));
            break;
        case CppTypeKind::reference_t: write_reference_prefix(output, static_cast<const CppReferenceType &>(type));
            break;
        case CppTypeKind::array_t: write_array_prefix(output, static_cast<const CppArrayType &>(type));
            break;
        case CppTypeKind::function_t: write_function_prefix(output, static_cast<const CppFunctionType &>(type));
            break;
        case CppTypeKind::member_function_t: write_member_function_prefix(
                output, static_cast<const CppMemberFunctionType &>(type));
            break;
        case CppTypeKind::member_object_t: write_member_object_prefix(
                output, static_cast<const CppMemberObjectType &>(type));
            break;

        case CppTypeKind::template_parameter_t: write_template_parameter(
                output, static_cast<const CppTemplateParameterType &>(type));
            break;
        case CppTypeKind::template_instantiation_t: write_template_instantiation(
                output, static_cast<const CppTemplateInstantiationType &>(type));
            break;
        case CppTypeKind::dependent_t: write_dependent(output, static_cast<const CppDependentType &>(type));
            break;
        case CppTypeKind::unexposed_t: write_unexposed(output, static_cast<const CppUnexposedType &>(type));
            break;
    }
}

void detail::write_type_suffix(CodeGenerator::output &output, const CppType &type) {
    switch (type.kind()) {
        case CppTypeKind::builtin_t: break;
        case CppTypeKind::user_defined_t: break;
        case CppTypeKind::auto_t: break;
        case CppTypeKind::decltype_t: break;
        case CppTypeKind::decltype_auto_t: break;

        case CppTypeKind::cv_qualified_t: write_cv_qualified_suffix(
                output, static_cast<const CppCvQualifiedType &>(type));
            break;
        case CppTypeKind::pointer_t: write_pointer_suffix(output, static_cast<const CppPointerType &>(type));
            break;
        case CppTypeKind::reference_t: write_reference_suffix(output, static_cast<const CppReferenceType &>(type));
            break;
        case CppTypeKind::array_t: write_array_suffix(output, static_cast<const CppArrayType &>(type));
            break;
        case CppTypeKind::function_t: write_function_suffix(output, static_cast<const CppFunctionType &>(type));
            break;
        case CppTypeKind::member_function_t: write_member_function_suffix(
                output, static_cast<const CppMemberFunctionType &>(type));
            break;\
        case CppTypeKind::member_object_t: write_member_object_suffix(
                output, static_cast<const CppMemberObjectType &>(type));
            break;
        case CppTypeKind::template_parameter_t: break;
        case CppTypeKind::template_instantiation_t: break;
        case CppTypeKind::dependent_t: break;
        case CppTypeKind::unexposed_t: break;
    }
}

void detail::write_type(CodeGenerator::output &output, const CppType &type, std::string name,
                        bool is_variadic) {
    write_type_prefix(output, type);
    if (!name.empty())
        output << whitespace << identifier(name);
    if (is_variadic)
        output << operator_ws << punctuation("...") << operator_ws;
    write_type_suffix(output, type);
}

std::string cppast::to_string(const CppType &type) {
    class to_string_generator : public CodeGenerator {
    public:
        std::string get() {
            return std::move(result_);
        }

    private:
        void do_indent() override {
        }

        void do_unindent() override {
        }

        void do_write_token_seq(string_view tokens) override {
            result_ += tokens.c_str();
        }

        std::string result_;
    } generator;

    // just a dummy type for the output
    static auto dummy_entity = CppTypeAlias::build("foo", CppBuiltinType::build(cpp_int));
    to_string_generator::output output(type_safe::ref(generator), type_safe::ref(*dummy_entity),
                                       cpp_public);
    write_type(output, type, "");
    return generator.get();
}
