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

#include <shark/ast/cpp_type.h>
#include <shark/ast/cpp_array_type.h>
#include <turbo/container/flat_hash_map.h>
#include <turbo/container/flat_hash_set.h>
#include <optional>

namespace cppast {
    struct TypeRegistry {
        static TypeRegistry &instance() {
            static TypeRegistry t;
            return t;
        }

        bool has_type(std::string name);

        void register_type(std::string name, std::string include);

        std::optional<std::string> type_include(std::string name);

    private:
        TypeRegistry();

        void init_builtin();

        void init_cstd();

        void init_stl();

        turbo::flat_hash_map<std::string, std::string> _includes;
    };

    struct TypeBuilder {
        /// builtin
        static std::unique_ptr<CppType> cpp_builtin_type(CppBuiltinTypeKind kind);

        static std::unique_ptr<CppType> int8_type();

        static std::unique_ptr<CppType> int16_type();

        static std::unique_ptr<CppType> int32_type();

        static std::unique_ptr<CppType> int64_type();

        static std::unique_ptr<CppType> uint8_type();

        static std::unique_ptr<CppType> uint16_type();

        static std::unique_ptr<CppType> uint32_type();

        static std::unique_ptr<CppType> uint64_type();

        static std::unique_ptr<CppType> sizet_type();

        static std::unique_ptr<CppType> void_type();

        static std::unique_ptr<CppType> bool_type();

        static std::unique_ptr<CppType> uchar_type();

        static std::unique_ptr<CppType> ushort_type();

        static std::unique_ptr<CppType> uint_type();

        static std::unique_ptr<CppType> ulong_type();

        static std::unique_ptr<CppType> ulonglong_type();

        static std::unique_ptr<CppType> uint128_type();

        static std::unique_ptr<CppType> schar_type();

        static std::unique_ptr<CppType> short_type();

        static std::unique_ptr<CppType> int_type();

        static std::unique_ptr<CppType> long_type();

        static std::unique_ptr<CppType> longlong_type();

        static std::unique_ptr<CppType> int128_type();

        static std::unique_ptr<CppType> char_type();

        static std::unique_ptr<CppType> wchar_type();

        static std::unique_ptr<CppType> char16_type();

        static std::unique_ptr<CppType> char32_type();

        static std::unique_ptr<CppType> float_type();

        static std::unique_ptr<CppType> double_type();

        static std::unique_ptr<CppType> longdouble_type();

        static std::unique_ptr<CppType> float128_type();

        static std::unique_ptr<CppType> cv_none_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> cv_const_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> cv_volatile_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> cv_const_volatile_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> pointer_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> lref_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> rref_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> const_ref_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> const_pointer_type(std::unique_ptr<CppType> t);

        static std::unique_ptr<CppType> build_type(std::string);

        static std::unique_ptr<CppType> array_type(std::unique_ptr<CppType> t, std::unique_ptr<CppExpression> s);

        turbo::flat_hash_set<std::string> includes;
    };

    inline std::unique_ptr<CppType> TypeBuilder::int8_type() {
        return build_type("int8_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::int16_type() {
        return build_type("int16_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::int32_type() {
        return build_type("int32_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::int64_type() {
        return build_type("int64_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint8_type() {
        return build_type("uint8_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint16_type() {
        return build_type("uint16_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint32_type() {
        return build_type("uint32_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint64_type() {
        return build_type("uint64_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::sizet_type() {
        return build_type("size_t");
    }

    inline std::unique_ptr<CppType> TypeBuilder::void_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_void);
    }

    inline std::unique_ptr<CppType> TypeBuilder::bool_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_bool);
    }

    inline std::unique_ptr<CppType> TypeBuilder::uchar_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::ushort_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::ulong_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::ulonglong_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::uint128_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_uchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::schar_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_schar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::short_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_short);
    }

    inline std::unique_ptr<CppType> TypeBuilder::int_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_int);
    }

    inline std::unique_ptr<CppType> TypeBuilder::long_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_long);
    }

    inline std::unique_ptr<CppType> TypeBuilder::longlong_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_longlong);
    }

    inline std::unique_ptr<CppType> TypeBuilder::int128_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_int128);
    }

    inline std::unique_ptr<CppType> TypeBuilder::char_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_char);
    }

    inline std::unique_ptr<CppType> TypeBuilder::wchar_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_wchar);
    }

    inline std::unique_ptr<CppType> TypeBuilder::char16_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_char16);
    }

    inline std::unique_ptr<CppType> TypeBuilder::char32_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_char32);
    }

    inline std::unique_ptr<CppType> TypeBuilder::float_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_float);
    }

    inline std::unique_ptr<CppType> TypeBuilder::double_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_double);
    }

    inline std::unique_ptr<CppType> TypeBuilder::longdouble_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_longdouble);
    }

    inline std::unique_ptr<CppType> TypeBuilder::float128_type() {
        return cpp_builtin_type(CppBuiltinTypeKind::cpp_float128);
    }

    inline std::unique_ptr<CppType> TypeBuilder::cv_none_type(std::unique_ptr<CppType> t) {
        return CppCvQualifiedType::build(std::move(t), CppCv::cpp_cv_none);
    }

    inline std::unique_ptr<CppType> TypeBuilder::cv_const_type(std::unique_ptr<CppType> t) {
        return CppCvQualifiedType::build(std::move(t), CppCv::cpp_cv_const);
    }

    inline std::unique_ptr<CppType> TypeBuilder::cv_volatile_type(std::unique_ptr<CppType> t) {
        return CppCvQualifiedType::build(std::move(t), CppCv::cpp_cv_volatile);
    }

    inline std::unique_ptr<CppType> TypeBuilder::cv_const_volatile_type(std::unique_ptr<CppType> t) {
        return CppCvQualifiedType::build(std::move(t), CppCv::cpp_cv_const_volatile);
    }

    inline std::unique_ptr<CppType> TypeBuilder::pointer_type(std::unique_ptr<CppType> t) {
        return CppPointerType::build(std::move(t));
    }

    inline std::unique_ptr<CppType> TypeBuilder::lref_type(std::unique_ptr<CppType> t) {
        return CppReferenceType::build(std::move(t), CppReference::cpp_ref_lvalue);
    }

    inline std::unique_ptr<CppType> TypeBuilder::rref_type(std::unique_ptr<CppType> t) {
        return CppReferenceType::build(std::move(t), CppReference::cpp_ref_rvalue);
    }

    inline std::unique_ptr<CppType> TypeBuilder::const_ref_type(std::unique_ptr<CppType> t) {
        return CppReferenceType::build(cv_const_type(std::move(t)), CppReference::cpp_ref_lvalue);
    }

    inline std::unique_ptr<CppType> TypeBuilder::const_pointer_type(std::unique_ptr<CppType> t) {
        return pointer_type(cv_const_type(std::move(t)));
    }

    inline std::unique_ptr<CppType> TypeBuilder::build_type(std::string t) {
        return CppUnexposedType::build(t);
    }

    inline std::unique_ptr<CppType> TypeBuilder::array_type(std::unique_ptr<CppType> t,
                                                            std::unique_ptr<CppExpression> s) {
        return CppArrayType::build(std::move(t), std::move(s));
    }
} // namespace cppast
