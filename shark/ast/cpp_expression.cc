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


#include <shark/ast/cpp_expression.h>

using namespace cppast;

namespace {
    void write_literal(CodeGenerator::output &output, const CppLiteralExpression &expr) {
        auto type_kind = cpp_void;
        if (expr.type().kind() == CppTypeKind::builtin_t)
            type_kind = static_cast<const CppBuiltinType &>(expr.type()).builtin_type_kind();
        else if (expr.type().kind() == CppTypeKind::pointer_t) {
            auto &pointee = static_cast<const CppPointerType &>(expr.type()).pointee();
            if (pointee.kind() == CppTypeKind::builtin_t) {
                auto &builtin_pointee = static_cast<const CppBuiltinType &>(pointee);
                if (builtin_pointee.builtin_type_kind() == cpp_char
                    || builtin_pointee.builtin_type_kind() == cpp_wchar
                    || builtin_pointee.builtin_type_kind() == cpp_char16
                    || builtin_pointee.builtin_type_kind() == cpp_char32)
                    // pointer to char aka string
                    type_kind = builtin_pointee.builtin_type_kind();
            }
        }

        switch (type_kind) {
            case cpp_void:
                output << token_seq(expr.value());
                break;

            case cpp_bool:
                output << keyword(expr.value());
                break;

            case cpp_uchar:
            case cpp_ushort:
            case cpp_uint:
            case cpp_ulong:
            case cpp_ulonglong:
            case cpp_uint128:
            case cpp_schar:
            case cpp_short:
            case cpp_int:
            case cpp_long:
            case cpp_longlong:
            case cpp_int128:
                output << int_literal(expr.value());
                break;

            case cpp_float:
            case cpp_double:
            case cpp_longdouble:
            case cpp_float128:
                output << float_literal(expr.value());
                break;

            case cpp_char:
            case cpp_wchar:
            case cpp_char16:
            case cpp_char32:
                output << string_literal(expr.value());
                break;

            case cpp_nullptr:
                output << keyword(expr.value());
                break;
        }
    }

    void write_unexposed(CodeGenerator::output &output, const CppUnexposedExpression &expr) {
        detail::write_token_string(output, expr.expression());
    }
} // namespace

void detail::write_expression(CodeGenerator::output &output, const CppExpression &expr) {
    switch (expr.kind()) {
        case CppExpressionKind::literal_t:
            write_literal(output, static_cast<const CppLiteralExpression &>(expr));
            break;
        case CppExpressionKind::unexposed_t:
            write_unexposed(output, static_cast<const CppUnexposedExpression &>(expr));
            break;
    }
}
