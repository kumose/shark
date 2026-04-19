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


#include <shark/ast/code_generator.h>
#include <shark/ast/cpp_alias_template.h>
#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_class_template.h>
#include <shark/ast/cpp_concept.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_enum.h>
#include <shark/ast/cpp_file.h>
#include <shark/ast/cpp_friend.h>
#include <shark/ast/cpp_function.h>
#include <shark/ast/cpp_function_template.h>
#include <shark/ast/cpp_language_linkage.h>
#include <shark/ast/cpp_member_function.h>
#include <shark/ast/cpp_member_variable.h>
#include <shark/ast/cpp_namespace.h>
#include <shark/ast/cpp_preprocessor.h>
#include <shark/ast/cpp_static_assert.h>
#include <shark/ast/cpp_template_parameter.h>
#include <shark/ast/cpp_token.h>
#include <shark/ast/cpp_type_alias.h>
#include <shark/ast/cpp_variable.h>
#include <shark/ast/cpp_variable_template.h>

using namespace cppast;

namespace {
    void opening_brace(const CodeGenerator::output &output) {
        if (output.formatting().is_set(FormattingFlags::brace_nl))
            output << newl;
        else if (output.formatting().is_set(FormattingFlags::brace_ws))
            output << whitespace;
        output << punctuation("{");
    }

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

    bool generate_code_impl(CodeGenerator &generator, const CppEntity &e,
                            CppAccessSpecifierKind cur_access);

    template<class Container, typename Sep>
    bool write_container(CodeGenerator::output &output, const Container &cont, Sep s,
                         CppAccessSpecifierKind cur_access) {
        auto need_sep = false;
        for (auto &child: cont) {
            auto is_excluded = output.options(child, cur_access).is_set(CodeGenerator::exclude);
            if (!is_excluded) {
                if (need_sep)
                    output << s;
                need_sep = generate_code_impl(*output.generator(), child, cur_access);
            }
        }
        return need_sep;
    }

    bool generate_file(CodeGenerator &generator, const CppFile &f,
                       CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(f), cur_access);
        if (output) {
            auto need_sep = write_container(output, f, newl, cur_access);
            if (!need_sep)
                // file empty, write newl
                output << newl;
            else
                output.container_end();
        }
        return static_cast<bool>(output);
    }

    bool generate_macro_parameter(CodeGenerator &generator, const CppMacroParameter &param,
                                  CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output)
            output << identifier(param.name());
        return static_cast<bool>(output);
    }

    bool generate_comment(CodeGenerator &generator, const CppComment &param,
                              CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output)
            output << identifier(param.name());
        return static_cast<bool>(output);
    }

    bool generate_macro_definition(CodeGenerator &generator, const CppMacroDefinition &def,
                                   CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(def), cur_access);
        if (output) {
            output << preprocessor_token("#define") << whitespace << identifier(def.name());
            if (def.is_function_like()) {
                output << preprocessor_token("(") << bracket_ws;
                auto need_sep = write_container(output, def.parameters(), comma, cpp_public);
                if (def.is_variadic()) {
                    if (need_sep)
                        output << comma;
                    output << preprocessor_token("...");
                }
                output << bracket_ws << preprocessor_token(")");
            }
            if (!def.replacement().empty() && !output.options().is_set(CodeGenerator::declaration))
                output << whitespace << preprocessor_token(def.replacement()) << newl;
            else
                output << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_include_directive(CodeGenerator &generator, const CppIncludeDirective &include,
                                    CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(include), cur_access);
        if (output) {
            output << preprocessor_token("#include") << whitespace;
            if (include.include_kind() == CppIncludeKind::system)
                output << preprocessor_token("<");
            else
                output << preprocessor_token("\"");
            output << include.target();
            if (include.include_kind() == CppIncludeKind::system)
                output << preprocessor_token(">");
            else
                output << preprocessor_token("\"");
            output << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_language_linkage(CodeGenerator &generator, const CppLanguageLinkage &linkage,
                                   CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(linkage), cur_access);
        if (output) {
            output << keyword("extern") << whitespace << string_literal(linkage.name());
            if (linkage.is_block()) {
                output << opening_brace;
                output.indent();

                write_container(output, linkage, newl, cur_access);

                output.unindent();
                output << punctuation("}") << newl;
            } else {
                output << whitespace;
                generate_code_impl(generator, *linkage.begin(), cur_access);
            }
        }
        return static_cast<bool>(output);
    }

    bool generate_namespace(CodeGenerator &generator, const CppNamespace &ns,
                            CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(ns), cur_access);
        if (output) {
            if (ns.is_inline())
                output << keyword("inline") << whitespace;
            output << keyword("namespace") << whitespace << identifier(ns.name());
            output << opening_brace;
            output.indent();

            write_container(output, ns, newl, cur_access);
            output.container_end();

            output.unindent();
            output << punctuation("}") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_namespace_alias(CodeGenerator &generator, const CppNamespaceAlias &alias,
                                  CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(alias), cur_access);
        if (output) {
            output << keyword("namespace") << whitespace << identifier(alias.name()) << operator_ws
                    << punctuation("=") << operator_ws;
            if (output.options() & CodeGenerator::exclude_target)
                output.excluded(alias);
            else
                output << alias.target();
            output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_using_directive(CodeGenerator &generator, const CppUsingDirective &directive,
                                  CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(directive), cur_access);
        if (output)
            output << keyword("using") << whitespace << keyword("namespace") << whitespace
                    << directive.target() << punctuation(";") << newl;
        return static_cast<bool>(output);
    }

    bool generate_using_declaration(CodeGenerator &generator, const CppUsingDeclaration &declaration,
                                    CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(declaration),
                                     cur_access);
        if (output)
            output << keyword("using") << whitespace << declaration.target() << punctuation(";")
                    << newl;
        return static_cast<bool>(output);
    }

    bool generate_type_alias(CodeGenerator &generator, const CppTypeAlias &alias,
                             CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(alias), cur_access);
        if (output) {
            output << keyword("using") << whitespace << identifier(alias.name()) << operator_ws
                    << punctuation("=") << operator_ws;
            if (output.options() & CodeGenerator::exclude_target)
                output.excluded(alias);
            else
                detail::write_type(output, alias.underlying_type(), "");
            output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_enum_value(CodeGenerator &generator, const CppEnumValue &value,
                             CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(value), cur_access);
        if (output) {
            output << identifier(value.name());
            if (value.value()) {
                output << operator_ws << punctuation("=") << operator_ws;
                detail::write_expression(output,
                                         value.value()
                                         .value()); // should have named something differently...
            }
        }
        return static_cast<bool>(output);
    }

    bool generate_enum(CodeGenerator &generator, const CppEnum &e,
                       CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(e), cur_access);
        if (output) {
            output << keyword("enum");
            if (e.is_scoped())
                output << whitespace << keyword("class");
            output << whitespace << identifier(e.semantic_scope()) << identifier(e.name());
            if (e.has_explicit_type()) {
                output << newl << punctuation(":") << operator_ws;
                detail::write_type(output, e.underlying_type(), "");
            }

            if (output.generate_definition() && e.is_definition()) {
                output << opening_brace;
                output.indent();

                auto need_sep = write_container(
                    output, e,
                    [](const CodeGenerator::output &out) { out << punctuation(",") << newl; },
                    cur_access);
                if (need_sep)
                    output << newl;

                output.container_end();

                output.unindent();
                output << punctuation("};") << newl;
            } else
                output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    void write_access_specifier(CodeGenerator::output &output, CppAccessSpecifierKind access) {
        output.unindent();
        output << keyword(to_string(access)) << punctuation(":");
        output.indent();
    }

    bool generate_access_specifier(CodeGenerator &generator, const CppAccessSpecifier &access,
                                   CppAccessSpecifierKind) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(access),
                                     access.access_specifier());
        if (output)
            write_access_specifier(output, access.access_specifier());
        return static_cast<bool>(output);
    }

    bool generate_base_class(CodeGenerator &generator, const CppBaseClass &base,
                             CppAccessSpecifierKind) {
        DEBUG_ASSERT(base.parent() && base.parent().value().kind() == CppEntityKind::class_t,
                     detail::assert_handler{});
        auto parent_kind = static_cast<const CppClass &>(base.parent().value()).class_kind();

        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(base),
                                     base.access_specifier());
        if (output) {
            if (base.is_virtual())
                output << keyword("virtual") << whitespace;

            auto access = base.access_specifier();
            if (access == cpp_protected)
                output << keyword("protected") << whitespace;
            else if (access == cpp_private && parent_kind != CppClassKind::class_t)
                output << keyword("private") << whitespace;
            else if (access == cpp_public && parent_kind == CppClassKind::class_t)
                output << keyword("public") << whitespace;

            output << identifier(base.name());
        }
        return static_cast<bool>(output);
    }

    void write_specialization_arguments(CodeGenerator::output &output,
                                        const CppTemplateSpecialization &spec) {
        if (spec.arguments_exposed())
            detail::write_template_arguments(output, spec.arguments());
        else if (!spec.unexposed_arguments().empty()) {
            output << punctuation("<") << bracket_ws;
            detail::write_token_string(output, spec.unexposed_arguments());
            output << bracket_ws << punctuation(">");
        }
    }

    void write_bases(CodeGenerator &generator, CodeGenerator::output &output, const CppClass &c) {
        auto need_sep = false;
        auto first = true;
        for (auto &base: c.bases()) {
            auto opt = output.options(base, base.access_specifier());
            if (first && !opt.is_set(CodeGenerator::exclude)) {
                first = false;
                output << newl << punctuation(":") << operator_ws;
            } else if (need_sep)
                output << comma;
            need_sep = generate_base_class(generator, base, cpp_public);
        }
    }

    bool generate_class(CodeGenerator &generator, const CppClass &c,
                        CppAccessSpecifierKind cur_access,
                        type_safe::optional_ref<const CppTemplateSpecialization> spec = nullptr) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(c), cur_access);
        if (output) {
            if (is_friended(c))
                output << keyword("friend") << whitespace;
            output << keyword(to_string(c.class_kind())) << whitespace;

            output << identifier(c.semantic_scope());
            if (spec) {
                output << spec.value().primary_template();
                write_specialization_arguments(output, spec.value());
            } else
                output << identifier(c.name());

            if (c.is_final())
                output << whitespace << keyword("final");

            if (!output.generate_definition() || c.is_declaration())
                output << punctuation(";") << newl;
            else {
                write_bases(generator, output, c);
                output << opening_brace;
                output.indent();

                auto need_sep = false;
                auto last_access = c.class_kind() == CppClassKind::class_t ? cpp_private : cpp_public;
                auto last_written_access = last_access;
                for (auto &member: c) {
                    if (member.kind() == CppEntityKind::access_specifier_t) {
                        auto &access = static_cast<const CppAccessSpecifier &>(member);
                        last_access = access.access_specifier();
                    } else if (output.options(member, last_access).is_set(CodeGenerator::exclude))
                        continue;
                    else {
                        if (need_sep)
                            output << newl;
                        if (last_access != last_written_access) {
                            write_access_specifier(output, last_access);
                            last_written_access = last_access;
                        }
                        need_sep = generate_code_impl(generator, member, last_access);
                    }
                }

                output.container_end();

                output.unindent();
                output << punctuation("};") << newl;
            }
        }
        return static_cast<bool>(output);
    }

    bool write_variable_base(CodeGenerator::output &output, const CppVariableBase &var,
                             const std::string &name) {
        detail::write_type(output, var.type(), name);

        if (var.default_value()) {
            output << operator_ws << punctuation("=") << operator_ws;
            detail::write_expression(output, var.default_value().value());
        }
        return static_cast<bool>(output);
    }

    void write_storage_class(CodeGenerator::output &output, CppStorageClassSpecifiers storage,
                             bool is_constexpr, bool is_consteval) {
        if (is_static(storage))
            output << keyword("static") << whitespace;
        if (is_extern(storage))
            output << keyword("extern") << whitespace;
        if (is_thread_local(storage))
            output << keyword("thread_local") << whitespace;
        if (is_constexpr)
            output << keyword("constexpr") << whitespace;
        else if (is_consteval)
            output << keyword("consteval") << whitespace;
    }

    bool generate_variable(CodeGenerator &generator, const CppVariable &var,
                           CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(var), cur_access);
        if (output) {
            write_storage_class(output, var.storage_class(), var.is_constexpr(), false);

            write_variable_base(output, var, var.name());
            output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_member_variable(CodeGenerator &generator, const CppMemberVariable &var,
                                  CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(var), cur_access);
        if (output) {
            if (var.is_mutable())
                output << keyword("mutable") << whitespace;
            write_variable_base(output, var, var.name());
            output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_bitfield(CodeGenerator &generator, const CppBitfield &var,
                           CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(var), cur_access);
        if (output) {
            if (var.is_mutable())
                output << keyword("mutable") << whitespace;
            write_variable_base(output, var, var.name());
            output << operator_ws << punctuation(":") << operator_ws
                    << int_literal(std::to_string(var.no_bits()));
            output << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_function_parameter(CodeGenerator &generator, const CppFunctionParameter &param,
                                     CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output)
            write_variable_base(output, param, param.name());
        return static_cast<bool>(output);
    }

    void write_function_parameters(CodeGenerator::output &output, const CppFunctionBase &base) {
        output << punctuation("(") << bracket_ws;
        auto need_sep = write_container(output, base.parameters(), comma, cpp_public);
        if (base.is_variadic()) {
            if (need_sep)
                output << comma;
            output << punctuation("...");
        }
        output << bracket_ws << punctuation(")");
    }

    void write_noexcept(CodeGenerator::output &output, const CppFunctionBase &base, bool need_ws) {
        if (!base.noexcept_condition())
            return;
        else if (need_ws)
            output << whitespace;

        auto &cond = base.noexcept_condition().value();
        if (cond.kind() == CppExpressionKind::literal_t
            && static_cast<const CppLiteralExpression &>(cond).value() == "true")
            output << keyword("noexcept");
        else {
            output << keyword("noexcept") << punctuation("(") << bracket_ws;
            // update check when expression gets exposed
            if (cond.kind() == CppExpressionKind::unexposed_t
                && static_cast<const CppUnexposedExpression &>(cond).expression().front().spelling
                == "false")
                output << keyword("false");
            else if (output.options().is_set(CodeGenerator::exclude_noexcept_condition))
                output.excluded(base);
            else
                detail::write_expression(output, cond);
            output << bracket_ws << punctuation(")");
        }
    }

    void write_function_body(CodeGenerator::output &output, const CppFunctionBase &base,
                             bool is_pure_virtual) {
        switch (base.body_kind()) {
            case cpp_function_declaration:
            case cpp_function_definition:
                if (is_pure_virtual)
                    output << operator_ws << punctuation("=") << operator_ws << int_literal("0");
                output << punctuation(";") << newl;
                break;

            case cpp_function_defaulted:
                output << operator_ws << punctuation("=") << operator_ws << keyword("default")
                        << punctuation(";") << newl;
                break;
            case cpp_function_deleted:
                output << operator_ws << punctuation("=") << operator_ws << keyword("delete")
                        << punctuation(";") << newl;
                break;
        }
    }

    bool generate_function(CodeGenerator &generator, const CppFunction &func,
                           CppAccessSpecifierKind cur_access,
                           type_safe::optional_ref<const CppTemplateSpecialization> spec = nullptr) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(func), cur_access);
        if (output) {
            if (is_friended(func))
                output << keyword("friend") << whitespace;
            write_storage_class(output, func.storage_class(), func.is_constexpr(), func.is_consteval());

            if (output.options() & CodeGenerator::exclude_return)
                output.excluded(func) << whitespace;
            else if (detail::is_complex_type(func.return_type()))
                output << keyword("auto") << whitespace;
            else {
                detail::write_type(output, func.return_type(), "");
                output << whitespace;
            }

            output << identifier(func.semantic_scope());
            if (spec) {
                output << spec.value().primary_template();
                write_specialization_arguments(output, spec.value());
            } else
                output << identifier(func.name());
            write_function_parameters(output, func);
            write_noexcept(output, func, output.formatting().is_set(FormattingFlags::operator_ws));

            if (!(output.options() & CodeGenerator::exclude_return)
                && detail::is_complex_type(func.return_type())) {
                output << operator_ws << punctuation("->") << operator_ws;
                detail::write_type(output, func.return_type(), "");
            }
            write_function_body(output, func, false);
        }
        return static_cast<bool>(output);
    }

    void write_prefix_virtual(CodeGenerator::output &output, const CppVirtual &virt) {
        if (is_virtual(virt))
            output << keyword("virtual") << whitespace;
    }

    void write_suffix_virtual(CodeGenerator::output &output, const CppVirtual &virt,
                              bool is_definition) {
        if (is_definition)
            // don't include it in definition
            return;

        if (is_overriding(virt))
            output << whitespace << keyword("override");
        if (is_final(virt))
            output << whitespace << keyword("final");
    }

    bool write_cv_ref(CodeGenerator::output &output, const CppMemberFunctionBase &base) {
        auto need_ws = false;
        switch (base.cv_qualifier()) {
            case cpp_cv_none:
                break;
            case cpp_cv_const:
                output << operator_ws << keyword("const");
                need_ws = true;
                break;
            case cpp_cv_volatile:
                output << operator_ws << keyword("volatile");
                need_ws = true;
                break;
            case cpp_cv_const_volatile:
                output << operator_ws << keyword("const") << whitespace << keyword("volatile");
                need_ws = true;
                break;
        }

        switch (base.ref_qualifier()) {
            case cpp_ref_none:
                break;
            case cpp_ref_lvalue:
                output << operator_ws << punctuation("&");
                need_ws = false;
                break;
            case cpp_ref_rvalue:
                output << operator_ws << punctuation("&&");
                need_ws = false;
                break;
        }

        return need_ws;
    }

    bool generate_member_function(CodeGenerator &generator, const CppMemberFunction &func,
                                  CppAccessSpecifierKind cur_access,
                                  type_safe::optional_ref<const CppTemplateSpecialization> spec
                                          = nullptr) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(func), cur_access);
        if (output) {
            if (is_friended(func))
                output << keyword("friend") << whitespace;
            if (func.is_constexpr())
                output << keyword("constexpr") << whitespace;
            else if (func.is_consteval())
                output << keyword("consteval") << whitespace;
            else
                write_prefix_virtual(output, func.virtual_info());

            if (output.options() & CodeGenerator::exclude_return)
                output.excluded(func) << whitespace;
            else if (detail::is_complex_type(func.return_type()))
                output << keyword("auto") << whitespace;
            else {
                detail::write_type(output, func.return_type(), "");
                output << whitespace;
            }

            output << identifier(func.semantic_scope());
            if (spec) {
                output << spec.value().primary_template();
                write_specialization_arguments(output, spec.value());
            } else
                output << identifier(func.name());
            write_function_parameters(output, func);
            auto need_ws = write_cv_ref(output, func);
            write_noexcept(output, func,
                           need_ws || output.formatting().is_set(FormattingFlags::operator_ws));

            if (!(output.options() & CodeGenerator::exclude_return)
                && detail::is_complex_type(func.return_type())) {
                output << operator_ws << punctuation("->") << operator_ws;
                detail::write_type(output, func.return_type(), "");
            }

            write_suffix_virtual(output, func.virtual_info(), func.is_definition());
            write_function_body(output, func, is_pure(func.virtual_info()));
        }
        return static_cast<bool>(output);
    }

    bool generate_conversion_op(CodeGenerator &generator, const CppConversionOp &op,
                                CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(op), cur_access);
        if (output) {
            if (is_friended(op))
                output << keyword("friend") << whitespace;
            if (op.is_explicit())
                output << keyword("explicit") << whitespace;
            if (op.is_constexpr())
                output << keyword("constexpr") << whitespace;
            else if (op.is_consteval())
                output << keyword("consteval") << whitespace;
            else
                write_prefix_virtual(output, op.virtual_info());

            output << identifier(op.semantic_scope());

            auto pos = op.name().find("operator");
            output << identifier(op.name().substr(0u, pos)) << keyword("operator") << whitespace;
            if (output.options() & CodeGenerator::exclude_return)
                output.excluded(op);
            else
                detail::write_type(output, op.return_type(), "");

            output << punctuation("(") << punctuation(")");
            auto need_ws = write_cv_ref(output, op);
            write_noexcept(output, op,
                           need_ws || output.formatting().is_set(FormattingFlags::operator_ws));

            write_suffix_virtual(output, op.virtual_info(), op.is_definition());
            write_function_body(output, op, is_pure(op.virtual_info()));
        }
        return static_cast<bool>(output);
    }

    bool generate_constructor(CodeGenerator &generator, const CppConstructor &ctor,
                              CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(ctor), cur_access);
        if (output) {
            if (is_friended(ctor))
                output << keyword("friend") << whitespace;
            if (ctor.is_explicit())
                output << keyword("explicit") << whitespace;
            if (ctor.is_constexpr())
                output << keyword("constexpr") << whitespace;
            if (ctor.is_consteval())
                output << keyword("consteval") << whitespace;

            output << identifier(ctor.semantic_scope()) << identifier(ctor.name());
            write_function_parameters(output, ctor);
            write_noexcept(output, ctor, output.formatting().is_set(FormattingFlags::operator_ws));

            write_function_body(output, ctor, false);
        }
        return static_cast<bool>(output);
    }

    bool generate_destructor(CodeGenerator &generator, const CppDestructor &dtor,
                             CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(dtor), cur_access);
        if (output) {
            if (is_friended(dtor))
                output << keyword("friend") << whitespace;
            write_prefix_virtual(output, dtor.virtual_info());
            output << identifier(dtor.semantic_scope()) << identifier(dtor.name()) << punctuation("(")
                    << punctuation(")");
            write_noexcept(output, dtor, output.formatting().is_set(FormattingFlags::operator_ws));

            write_suffix_virtual(output, dtor.virtual_info(), dtor.is_definition());
            write_function_body(output, dtor, is_pure(dtor.virtual_info()));
        }
        return static_cast<bool>(output);
    }

    bool generate_function_base(CodeGenerator &generator, const CppFunctionBase &base,
                                CppAccessSpecifierKind cur_access,
                                const CppTemplateSpecialization &spec) {
        switch (base.kind()) {
            case CppEntityKind::function_t:
                return generate_function(generator, static_cast<const CppFunction &>(base), cur_access,
                                         type_safe::ref(spec));
            case CppEntityKind::member_function_t:
                return generate_member_function(generator, static_cast<const CppMemberFunction &>(base),
                                                cur_access, type_safe::ref(spec));
            case CppEntityKind::conversion_op_t:
                return generate_conversion_op(generator, static_cast<const CppConversionOp &>(base),
                                              cur_access);
            case CppEntityKind::constructor_t:
                return generate_constructor(generator, static_cast<const CppConstructor &>(base),
                                            cur_access);

            default:
                DEBUG_UNREACHABLE(detail::assert_handler{});
                break;
        }
        return false;
    }

    bool generate_friend(CodeGenerator &generator, const CppFriend &f,
                         CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(f), cur_access);
        if (output) {
            if (auto e = f.entity())
                generate_code_impl(generator, e.value(), cur_access);
            else if (auto type = f.type()) {
                output << keyword("friend") << whitespace;
                detail::write_type(output, type.value(), "");
                output << punctuation(";");
            } else
                DEBUG_UNREACHABLE(detail::assert_handler{});
        }
        return static_cast<bool>(output);
    }

    bool generate_template_type_parameter(CodeGenerator &generator,
                                          const CppTemplateTypeParameter &param,
                                          CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output) {
            if (param.keyword() == cpp_template_keyword::concept_contraint)
                detail::write_token_string(output, param.concept_constraint().value());
            else
                output << keyword(to_string(param.keyword()));
            if (param.is_variadic())
                output << operator_ws << punctuation("...");
            if (!param.name().empty())
                output << whitespace << identifier(param.name());
            if (param.default_type()) {
                output << operator_ws << punctuation("=") << operator_ws;
                detail::write_type(output, param.default_type().value(), "");
            }
        }
        return static_cast<bool>(output);
    }

    bool generate_non_type_template_parameter(CodeGenerator &generator,
                                              const CppNonTypeTemplateParameter &param,
                                              CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output) {
            detail::write_type(output, param.type(), param.name(), param.is_variadic());
            if (param.default_value()) {
                output << operator_ws << punctuation("=") << operator_ws;
                detail::write_expression(output, param.default_value().value());
            }
        }
        return static_cast<bool>(output);
    }

    bool generate_template_template_parameter(CodeGenerator &generator,
                                              const CppTemplateTemplateParameter &param,
                                              CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(param), cur_access);
        if (output) {
            output << keyword("template") << operator_ws << punctuation("<") << bracket_ws;
            write_container(output, param.parameters(), punctuation(","), cur_access);
            output << bracket_ws << punctuation(">") << operator_ws
                    << keyword(to_string(param.keyword()));
            if (param.is_variadic())
                output << operator_ws << punctuation("...");
            output << whitespace << identifier(param.name());
            if (param.default_template())
                output << operator_ws << punctuation("=") << operator_ws
                        << param.default_template().value();
        }
        return static_cast<bool>(output);
    }

    void write_template_parameters(CodeGenerator::output &output, const CppTemplate &templ,
                                   bool hide_if_empty) {
        if (!hide_if_empty)
            output << keyword("template") << operator_ws << punctuation("<") << bracket_ws;

        auto need_sep = false;
        auto need_header = hide_if_empty;
        for (auto &param: templ.parameters()) {
            auto is_excluded = output.options(param, cpp_public).is_set(CodeGenerator::exclude);
            if (!is_excluded) {
                if (need_header) {
                    need_header = false;
                    output << keyword("template") << operator_ws << punctuation("<") << bracket_ws;
                } else if (need_sep)
                    output << comma;
                need_sep = generate_code_impl(*output.generator(), param, cpp_public);
            }
        }

        if (!need_header)
            output << bracket_ws << punctuation(">") << newl;
    }

    bool generate_alias_template(CodeGenerator &generator, const CppAliasTemplate &alias,
                                 CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(alias), cur_access);
        if (output) {
            write_template_parameters(output, alias, true);
            generate_code_impl(generator, alias.type_alias(), cur_access);
        }
        return static_cast<bool>(output);
    }

    bool generate_variable_template(CodeGenerator &generator, const CppVariableTemplate &var,
                                    CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(var), cur_access);
        if (output) {
            write_template_parameters(output, var, true);
            generate_code_impl(generator, var.variable(), cur_access);
        }
        return static_cast<bool>(output);
    }

    bool generate_function_template(CodeGenerator &generator, const CppFunctionTemplate &func,
                                    CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(func), cur_access);
        if (output) {
            write_template_parameters(output, func, true);
            generate_code_impl(generator, func.function(), cur_access);
        }
        return static_cast<bool>(output);
    }

    bool generate_function_template_specialization(CodeGenerator &generator,
                                                   const CppFunctionTemplateSpecialization &func,
                                                   CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(func), cur_access);
        if (output) {
            DEBUG_ASSERT(func.is_full_specialization(), detail::assert_handler{});
            if (!is_friended(func))
                // don't write template parameters in friend
                write_template_parameters(output, func, false);
            generate_function_base(generator, func.function(), cur_access, func);
        }
        return static_cast<bool>(output);
    }

    bool generate_class_template(CodeGenerator &generator, const CppClassTemplate &templ,
                                 CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(templ), cur_access);
        if (output) {
            write_template_parameters(output, templ, true);
            generate_class(generator, templ.class_(), cur_access);
        }
        return static_cast<bool>(output);
    }

    bool generate_class_template_specialization(CodeGenerator &generator,
                                                const CppClassTemplateSpecialization &templ,
                                                CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(templ), cur_access);
        if (output) {
            write_template_parameters(output, templ, false);
            generate_class(generator, templ.class_(), cur_access, type_safe::ref(templ));
        }
        return static_cast<bool>(output);
    }

    bool generate_concept(CodeGenerator &generator, const CppConcept &con,
                          CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(con), cur_access);
        if (output) {
            output << keyword("template") << operator_ws << punctuation("<") << bracket_ws;
            detail::write_token_string(output, con.parameters());
            output << bracket_ws << punctuation(">") << newl;
            output << keyword("concept") << operator_ws << identifier(con.name()) << operator_ws
                    << punctuation("=") << operator_ws;
            detail::write_expression(output, con.constraint_expression());
            output << operator_ws << punctuation(";") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_static_assert(CodeGenerator &generator, const CppStaticAssert &assert,
                                CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(assert), cur_access);
        if (output) {
            output << keyword("static_assert") << punctuation("(") << bracket_ws;
            detail::write_expression(output, assert.expression());
            output << comma << string_literal('"' + assert.message() + '"');
            output << bracket_ws << punctuation(");") << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_unexposed(CodeGenerator &generator, const CppUnexposedEntity &entity,
                            CppAccessSpecifierKind cur_access) {
        CodeGenerator::output output(type_safe::ref(generator), type_safe::ref(entity), cur_access);
        if (output) {
            detail::write_token_string(output, entity.spelling());
            output << newl;
        }
        return static_cast<bool>(output);
    }

    bool generate_code_impl(CodeGenerator &generator, const CppEntity &e,
                            CppAccessSpecifierKind cur_access) {
        switch (e.kind()) {
            case CppEntityKind::file_t: return generate_file(generator, static_cast<const CppFile &>(e), cur_access);
            case CppEntityKind::comment_t: return generate_comment(
                    generator, static_cast<const CppComment &>(e), cur_access);
            case CppEntityKind::macro_parameter_t: return generate_macro_parameter(
                    generator, static_cast<const CppMacroParameter &>(e), cur_access);
            case CppEntityKind::macro_definition_t: return generate_macro_definition(
                    generator, static_cast<const CppMacroDefinition &>(e), cur_access);
            case CppEntityKind::include_directive_t: return generate_include_directive(
                    generator, static_cast<const CppIncludeDirective &>(e), cur_access);

            case CppEntityKind::language_linkage_t: return generate_language_linkage(
                    generator, static_cast<const CppLanguageLinkage &>(e), cur_access);
            case CppEntityKind::namespace_t: return
                        generate_namespace(generator, static_cast<const CppNamespace &>(e), cur_access);
            case CppEntityKind::namespace_alias_t: return generate_namespace_alias(
                    generator, static_cast<const CppNamespaceAlias &>(e), cur_access);
            case CppEntityKind::using_directive_t: return generate_using_directive(
                    generator, static_cast<const CppUsingDirective &>(e), cur_access);
            case CppEntityKind::using_declaration_t: return generate_using_declaration(
                    generator, static_cast<const CppUsingDeclaration &>(e), cur_access);

            case CppEntityKind::type_alias_t: return generate_type_alias(
                    generator, static_cast<const CppTypeAlias &>(e),
                    cur_access);

            case CppEntityKind::enum_t: return generate_enum(generator, static_cast<const CppEnum &>(e), cur_access);

            case CppEntityKind::enum_value_t: return generate_enum_value(
                    generator, static_cast<const CppEnumValue &>(e),
                    cur_access);

            case CppEntityKind::class_t: return
                        generate_class(generator, static_cast<const CppClass &>(e), cur_access);
            case CppEntityKind::access_specifier_t: return generate_access_specifier(
                    generator, static_cast<const CppAccessSpecifier &>(e), cur_access);
            case CppEntityKind::base_class_t: return generate_base_class(
                    generator, static_cast<const CppBaseClass &>(e),
                    cur_access);

            case CppEntityKind::variable_t: return generate_variable(generator, static_cast<const CppVariable &>(e),
                                                                     cur_access);
            case CppEntityKind::member_variable_t: return generate_member_variable(
                    generator, static_cast<const CppMemberVariable &>(e), cur_access);
            case CppEntityKind::bitfield_t: return generate_bitfield(generator, static_cast<const CppBitfield &>(e),
                                                                     cur_access);

            case CppEntityKind::function_parameter_t: return generate_function_parameter(
                    generator, static_cast<const CppFunctionParameter &>(e), cur_access);

            case CppEntityKind::function_t: return generate_function(generator, static_cast<const CppFunction &>(e),
                                                                     cur_access);
            case CppEntityKind::member_function_t: return generate_member_function(
                    generator, static_cast<const CppMemberFunction &>(e), cur_access);
            case CppEntityKind::conversion_op_t: return generate_conversion_op(
                    generator, static_cast<const CppConversionOp &>(e),
                    cur_access);
            case CppEntityKind::constructor_t: return generate_constructor(
                    generator, static_cast<const CppConstructor &>(e),
                    cur_access);
            case CppEntityKind::destructor_t: return generate_destructor(
                    generator, static_cast<const CppDestructor &>(e),
                    cur_access);
            case CppEntityKind::friend_t: return generate_friend(generator, static_cast<const CppFriend &>(e),
                                                                 cur_access);
            case CppEntityKind::template_type_parameter_t: return generate_template_type_parameter(
                    generator, static_cast<const CppTemplateTypeParameter &>(e), cur_access);

            case CppEntityKind::non_type_template_parameter_t: return generate_non_type_template_parameter(
                    generator, static_cast<const CppNonTypeTemplateParameter &>(e), cur_access);
            case CppEntityKind::template_template_parameter_t: return generate_template_template_parameter(
                    generator, static_cast<const CppTemplateTemplateParameter &>(e), cur_access);

            case CppEntityKind::alias_template_t: return generate_alias_template(
                    generator, static_cast<const CppAliasTemplate &>(e), cur_access);

            case CppEntityKind::variable_template_t: return generate_variable_template(
                    generator, static_cast<const CppVariableTemplate &>(e), cur_access);
            case CppEntityKind::function_template_t: return generate_function_template(
                    generator, static_cast<const CppFunctionTemplate &>(e), cur_access);
            case CppEntityKind::function_template_specialization_t: return generate_function_template_specialization(
                    generator, static_cast<const CppFunctionTemplateSpecialization &>(e), cur_access);

            case CppEntityKind::class_template_t: return generate_class_template(
                    generator, static_cast<const CppClassTemplate &>(e), cur_access);

            case CppEntityKind::class_template_specialization_t: return generate_class_template_specialization(
                    generator, static_cast<const CppClassTemplateSpecialization &>(e), cur_access);

            case CppEntityKind::concept_t: return generate_concept(generator, static_cast<const CppConcept &>(e),
                                                                   cur_access);

            case CppEntityKind::static_assert_t: return generate_static_assert(
                    generator, static_cast<const CppStaticAssert &>(e),
                    cur_access);

            case CppEntityKind::unexposed_t:
                return generate_unexposed(generator, static_cast<const CppUnexposedEntity &>(e),
                                          cur_access);

            case CppEntityKind::count:
                DEBUG_UNREACHABLE(detail::assert_handler{});
                break;
        }

        return false;
    }
} // namespace

bool CodeGenerator::generate_code(const CppEntity &entity) {
    return generate_code_impl(*this, entity, cpp_public);
}

bool cppast::generate_code(CodeGenerator &generator, const CppEntity &e) {
    generator.main_entity_ = type_safe::ref(e);
    auto result = generate_code_impl(generator, e, cpp_public);
    generator.main_entity_ = nullptr;
    return result;
}

void detail::write_template_arguments(
    CodeGenerator::output &output,
    type_safe::optional<type_safe::array_ref<const CppTemplateArgument> > arguments) {
    if (!arguments) {
        output << punctuation("<") << punctuation(">");
    } else {
        output << punctuation("<") << bracket_ws;
        auto need_sep = false;
        for (auto &arg: arguments.value()) {
            if (need_sep)
                output << comma;
            else
                need_sep = true;

            if (auto type = arg.type())
                detail::write_type(output, type.value(), "");
            else if (auto expr = arg.expression())
                detail::write_expression(output, expr.value());
            else if (auto templ = arg.template_ref())
                output << templ.value();
            else
                DEBUG_UNREACHABLE(detail::assert_handler{});
        }
        output << bracket_ws << punctuation(">");
    }
}

void detail::write_token_string(CodeGenerator::output &output, const CppTokenString &tokens) {
    auto last_kind = CppTokenKind::punctuation; // neutral regarding whitespace
    for (auto &token: tokens) {
        switch (token.kind) {
            case CppTokenKind::identifier:
                if (last_kind == CppTokenKind::identifier || last_kind == CppTokenKind::keyword)
                    output << whitespace;
                output << keyword(token.spelling);
                break;

            case CppTokenKind::keyword:
                if (last_kind == CppTokenKind::identifier || last_kind == CppTokenKind::keyword)
                    output << whitespace;
                output << identifier(token.spelling);
                if (token.spelling == "template")
                    output << operator_ws;
                break;

            case CppTokenKind::int_literal:
                output << int_literal(token.spelling);
                break;
            case CppTokenKind::float_literal:
                output << float_literal(token.spelling);
                break;
            case CppTokenKind::char_literal:
            case CppTokenKind::string_literal:
                output << string_literal(token.spelling);
                break;

            case CppTokenKind::punctuation:
                if (token.spelling == ",")
                    output << comma;
                    // print brackets
                    // don't treat <> special as they might be operators...
                else if (token.spelling == "(" || token.spelling == "[" || token.spelling == "{")
                    output << punctuation(token.spelling) << bracket_ws;
                else if (token.spelling == ")" || token.spelling == "]" || token.spelling == "}")
                    output << bracket_ws << punctuation(token.spelling);
                    // operators that are always binary operators in all contexts
                else if (token.spelling.back() == '=' // all assignment operators
                         || token.spelling == "/" || token.spelling == "%" || token.spelling == "=="
                         || token.spelling == "!=" || token.spelling == "<=" || token.spelling == ">="
                         || token.spelling == "&&" || token.spelling == "||" || token.spelling == "|"
                         || token.spelling == "^" || token.spelling == "?")
                    output << operator_ws << punctuation(token.spelling) << operator_ws;
                else
                    output << punctuation(token.spelling);
                break;
        }

        last_kind = token.kind;
    }
}
