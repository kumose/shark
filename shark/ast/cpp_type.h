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

#include <atomic>
#include <memory>

#include <shark/ast/code_generator.h>
#include <shark/ast/cpp_entity_ref.h>
#include <shark/detail/intrusive_list.hpp>

namespace cppast {
    /// The kinds of a [cppast::CppType]().
    enum class CppTypeKind {
        builtin_t,
        user_defined_t,

        auto_t,
        decltype_t,
        decltype_auto_t,

        cv_qualified_t,
        pointer_t,
        reference_t,

        array_t,
        function_t,
        member_function_t,
        member_object_t,

        template_parameter_t,
        template_instantiation_t,

        dependent_t,

        unexposed_t,
    };

    /// Base class for all C++ types.
    class CppType : detail::intrusive_list_node<CppType> {
    public:
        CppType(const CppType &) = delete;

        CppType &operator=(const CppType &) = delete;

        virtual ~CppType() noexcept = default;

        /// \returns The [cppast::CppTypeKind]().
        CppTypeKind kind() const noexcept {
            return do_get_kind();
        }

        /// \returns The specified user data.
        void *user_data() const noexcept {
            return user_data_.load();
        }

        /// \effects Sets some kind of user data.
        ///
        /// User data is just some kind of pointer, there are no requirements.
        /// The class will do no lifetime management.
        ///
        /// User data is useful if you need to store additional data for an entity without the need to
        /// maintain a registry.
        void set_user_data(void *data) const noexcept {
            user_data_ = data;
        }

    protected:
        CppType() noexcept : user_data_(nullptr) {
        }

    private:
        /// \returns The [cppast::CppTypeKind]().
        virtual CppTypeKind do_get_kind() const noexcept = 0;

        void on_insert(const CppType &) {
        }

        mutable std::atomic<void *> user_data_;

        template<typename T>
        friend struct detail::intrusive_list_access;
        friend detail::intrusive_list_node<CppType>;
    };

    /// An unexposed [cppast::CppType]().
    ///
    /// This is one where no further information besides a name is available.
    class CppUnexposedType final : public CppType {
    public:
        /// \returns A newly created unexposed type.
        static std::unique_ptr<CppUnexposedType> build(std::string name) {
            return std::unique_ptr<CppUnexposedType>(new CppUnexposedType(std::move(name)));
        }

        /// \returns The name of the type.
        const std::string &name() const noexcept {
            return name_;
        }

    private:
        CppUnexposedType(std::string name) : name_(std::move(name)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::unexposed_t;
        }

        std::string name_;
    };

    /// The C++ builtin types.
    enum CppBuiltinTypeKind : int {
        cpp_void, //< `void`

        cpp_bool, //< `bool`

        cpp_uchar, //< `unsigned char`
        cpp_ushort, //< `unsigned short`
        cpp_uint, //< `unsigned int`
        cpp_ulong, //< `unsigned long`
        cpp_ulonglong, //< `unsigned long long`
        cpp_uint128, //< `unsigned __int128`

        cpp_schar, //< `signed char`
        cpp_short, //< `short`
        cpp_int, //< `int`
        cpp_long, //< `long`
        cpp_longlong, //< `long long`
        cpp_int128, //< `__int128`

        cpp_float, //< `float`
        cpp_double, //< `double`
        cpp_longdouble, //< `long double`
        cpp_float128, //< `__float128`

        cpp_char, //< `char`
        cpp_wchar, //< `wchar_t`
        cpp_char16, //< `char16_t`
        cpp_char32, //< `char32_t`

        cpp_nullptr, //< `decltype(nullptr)` aka `std::nullptr_t`
    };

    /// \returns The string representing the spelling of that type in the source code.
    const char *to_string(CppBuiltinTypeKind kind) noexcept;

    /// A builtin [cppast::CppType]().
    ///
    /// This is one where there is no associated [cppast::CppEntity]().
    class CppBuiltinType final : public CppType {
    public:
        /// \returns A newly created builtin type.
        static std::unique_ptr<CppBuiltinType> build(CppBuiltinTypeKind kind) {
            return std::unique_ptr<CppBuiltinType>(new CppBuiltinType(kind));
        }

        /// \returns Which builtin type it is.
        CppBuiltinTypeKind builtin_type_kind() const noexcept {
            return kind_;
        }

    private:
        CppBuiltinType(CppBuiltinTypeKind kind) : kind_(kind) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::builtin_t;
        }

        CppBuiltinTypeKind kind_;
    };

    /// \exclude
    namespace detail {
        struct cpp_type_ref_predicate {
            bool operator()(const CppEntity &e);
        };
    } // namespace detail

    /// Reference to a [cppast::CppEntity]() representing a new type.
    using CppTypeRef = BasicCppEntityRef<CppEntity, detail::cpp_type_ref_predicate>;

    /// A user-defined [cppast::CppType]().
    ///
    /// It has an associated [cppast::CppEntity]().
    class CppUserDefinedType final : public CppType {
    public:
        /// \returns A newly created user-defined type.
        static std::unique_ptr<CppUserDefinedType> build(CppTypeRef entity) {
            return std::unique_ptr<CppUserDefinedType>(new CppUserDefinedType(std::move(entity)));
        }

        /// \returns A [cppast::CppTypeRef]() to the associated [cppast::CppEntity]() that is the
        /// type.
        const CppTypeRef &entity() const noexcept {
            return entity_;
        }

    private:
        CppUserDefinedType(CppTypeRef entity) : entity_(std::move(entity)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::user_defined_t;
        }

        CppTypeRef entity_;
    };

    /// A [cppast::CppType]() that isn't given but deduced by `auto`.
    class CppAutoType final : public CppType {
    public:
        /// \returns A newly created `auto` type.
        static std::unique_ptr<CppAutoType> build() {
            return std::unique_ptr<CppAutoType>(new CppAutoType);
        }

    private:
        CppAutoType() = default;

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::auto_t;
        }
    };

    /// A [cppast::CppType]() that depends on another type.
    class CppDependentType final : public CppType {
    public:
        /// \returns A newly created type dependent on a [cppast::CppTemplateParameterType]().
        static std::unique_ptr<CppDependentType> build(
            std::string name, std::unique_ptr<CppTemplateParameterType> dependee);

        /// \returns A newly created type dependent on a [cppast::CppTemplateInstantiationType]().
        static std::unique_ptr<CppDependentType> build(
            std::string name, std::unique_ptr<CppTemplateInstantiationType> dependee);

        /// \returns The name of the dependent type.
        /// \notes It does not include a scope.
        const std::string &name() const noexcept {
            return name_;
        }

        /// \returns A reference to the [cppast::CppType]() it depends one.
        /// \notes This is either [cppast::CppTemplateParameterType]() or
        /// [cppast:CppTemplateInstantiationType]().
        const CppType &dependee() const noexcept {
            return *dependee_;
        }

    private:
        CppDependentType(std::string name, std::unique_ptr<CppType> dependee)
            : name_(std::move(name)), dependee_(std::move(dependee)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::dependent_t;
        }

        std::string name_;
        std::unique_ptr<CppType> dependee_;
    };

    /// The kinds of C++ cv qualifiers.
    enum CppCv : int {
        cpp_cv_none,
        cpp_cv_const,
        cpp_cv_volatile,
        cpp_cv_const_volatile,
    };

    /// \returns `true` if the qualifier contains `const`.
    inline bool is_const(CppCv cv) noexcept {
        return cv == cpp_cv_const || cv == cpp_cv_const_volatile;
    }

    /// \returns `true` if the qualifier contains `volatile`.
    inline bool is_volatile(CppCv cv) noexcept {
        return cv == cpp_cv_volatile || cv == cpp_cv_const_volatile;
    }

    /// A [cppast::CppCv]() qualified [cppast::CppType]().
    class CppCvQualifiedType final : public CppType {
    public:
        /// \returns A newly created qualified type.
        /// \requires `cv` must not be [cppast::CppCv::cpp_cv_none]().
        static std::unique_ptr<CppCvQualifiedType> build(std::unique_ptr<CppType> type, CppCv cv) {
            DEBUG_ASSERT(cv != cpp_cv_none, detail::precondition_error_handler{});
            return std::unique_ptr<CppCvQualifiedType>(
                new CppCvQualifiedType(std::move(type), cv));
        }

        /// \returns A reference to the [cppast::CppType]() that is qualified.
        const CppType &type() const noexcept {
            return *type_;
        }

        /// \returns The [cppast::CppCv]() qualifier.
        CppCv cv_qualifier() const noexcept {
            return cv_;
        }

    private:
        CppCvQualifiedType(std::unique_ptr<CppType> type, CppCv cv)
            : type_(std::move(type)), cv_(cv) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::cv_qualified_t;
        }

        std::unique_ptr<CppType> type_;
        CppCv cv_;
    };

    /// \returns The type without top-level const/volatile qualifiers.
    const CppType &remove_cv(const CppType &type) noexcept;

    /// \returns The type without top-level const qualifiers.
    const CppType &remove_const(const CppType &type) noexcept;

    /// \returns The type without top-level volatile qualifiers.
    const CppType &remove_volatile(const CppType &type) noexcept;

    /// A pointer to a [cppast::CppType]().
    class CppPointerType final : public CppType {
    public:
        /// \returns A newly created pointer type.
        static std::unique_ptr<CppPointerType> build(std::unique_ptr<CppType> pointee) {
            return std::unique_ptr<CppPointerType>(new CppPointerType(std::move(pointee)));
        }

        /// \returns A reference to the [cppast::CppType]() that is the pointee.
        const CppType &pointee() const noexcept {
            return *pointee_;
        }

    private:
        CppPointerType(std::unique_ptr<CppType> pointee) : pointee_(std::move(pointee)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::pointer_t;
        }

        std::unique_ptr<CppType> pointee_;
    };

    /// The kinds of C++ references.
    enum CppReference : int {
        cpp_ref_none,
        cpp_ref_lvalue,
        cpp_ref_rvalue,
    };

    /// A reference to a [cppast::CppType]().
    class CppReferenceType final : public CppType {
    public:
        /// \returns A newly created qualified type.
        /// \requires `ref` must not be [cppast::CppReference::cpp_ref_none]().
        static std::unique_ptr<CppReferenceType> build(std::unique_ptr<CppType> type,
                                                       CppReference ref) {
            DEBUG_ASSERT(ref != cpp_ref_none, detail::precondition_error_handler{});
            return std::unique_ptr<CppReferenceType>(new CppReferenceType(std::move(type), ref));
        }

        /// \returns A reference to the [cppast::CppType]() that is referenced.
        const CppType &referee() const noexcept {
            return *referee_;
        }

        /// \returns The [cppast::CppReference]() type.
        CppReference reference_kind() const noexcept {
            return ref_;
        }

    private:
        CppReferenceType(std::unique_ptr<CppType> referee, CppReference ref)
            : referee_(std::move(referee)), ref_(ref) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::reference_t;
        }

        std::unique_ptr<CppType> referee_;
        CppReference ref_;
    };

    /// \returns The type as a string representation.
    std::string to_string(const CppType &type);

    /// \exclude
    namespace detail {
        // whether or not it requires special treatment when printing it
        // i.e. function pointer types are complex as the identifier has to be put inside
        bool is_complex_type(const CppType &type) noexcept;

        // write part of the type that comes before the variable name
        void write_type_prefix(CodeGenerator::output &output, const CppType &type);

        // write part of the type that comes after the name
        // for non-complex types, this does nothing
        void write_type_suffix(CodeGenerator::output &output, const CppType &type);

        // write prefix, variadic, name, suffix
        void write_type(CodeGenerator::output &output, const CppType &type, std::string name,
                        bool is_variadic = false);
    } // namespace detail
} // namespace cppast
