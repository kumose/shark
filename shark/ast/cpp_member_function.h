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


#include <typesafe/flag_set.hpp>

#include <shark/ast/cpp_function.h>

namespace cppast {
    /// The `virtual`-ness of a member function.
    ///
    /// This is a [ts::flag_set]() `enum`.
    /// \notes It does not specify whether a member function is `virtual` or not,
    /// only the kind of `virtual`.
    /// \notes As surprising as it may be, any of these can be used in combination,
    /// i.e. you can have a `final` non-overriding function or an overriding pure `virtual` function.
    enum class CppVirtualFlags {
        pure, //< Set if the function is pure.
        override, //< Set if the function overrides a base class function.
        final, //< Set if the function is marked `final`.

        _flag_set_size, //< \exclude
    };

    /// The `virtual` information of a member function.
    ///
    /// This is an optional of the combination of the [cppast::CppVirtualFlags]().
    /// If the optional has a value, the member function is `virtual`,
    /// and the [ts::flag_set]() describes additional information.
    using CppVirtual = type_safe::optional<type_safe::flag_set<CppVirtualFlags> >;

    /// \returns Whether or not a member function is `virtual`.
    inline bool is_virtual(const CppVirtual &virt) noexcept {
        return virt.has_value();
    }

    /// \returns Whether or not a member function is pure.
    inline bool is_pure(const CppVirtual &virt) noexcept {
        return static_cast<bool>(virt.value_or(CppVirtualFlags::final) & CppVirtualFlags::pure);
    }

    /// \returns Whether or not a member function overrides another one.
    inline bool is_overriding(const CppVirtual &virt) noexcept {
        return static_cast<bool>(virt.value_or(CppVirtualFlags::pure) & CppVirtualFlags::override);
    }

    /// \returns Whether or not a member function is `final`.
    inline bool is_final(const CppVirtual &virt) noexcept {
        return static_cast<bool>(virt.value_or(CppVirtualFlags::pure) & CppVirtualFlags::final);
    }

    /// Base classes for all regular member function.
    ///
    /// The two derived classes are [cppast::CppMemberFunction]() and [cppast::CppConversionOp]().
    class CppMemberFunctionBase : public CppFunctionBase {
    public:
        /// \returns The return type of the member function.
        const CppType &return_type() const noexcept {
            return *return_type_;
        }

        /// \returns Whether or not it is `virtual`.
        bool is_virtual() const noexcept {
            return virtual_info().has_value();
        }

        /// \returns The `virtual`-ness of the member function.
        const CppVirtual &virtual_info() const noexcept {
            return virtual_;
        }

        /// \returns The cv-qualifier on the member function.
        CppCv cv_qualifier() const noexcept {
            return cv_;
        }

        /// \returns The ref-qualifier on the member function.
        CppReference ref_qualifier() const noexcept {
            return ref_;
        }

        /// \returns Whether or not the member function is `constexpr`.
        bool is_constexpr() const noexcept {
            return constexpr_;
        }

        /// \returns Whether or not the member function is `consteval`.
        bool is_consteval() const noexcept {
            return consteval_;
        }

    protected:
        /// Builder class for member functions.
        template<typename T>
        class basic_member_builder : public basic_builder<T> {
        public:
            /// \effects Sets the name and return type.
            basic_member_builder(std::string name, std::unique_ptr<CppType> return_type) {
                this->function = std::unique_ptr<T>(new T(std::move(name), std::move(return_type)));
            }

            /// \effects Sets the cv- and ref-qualifier.
            void cv_ref_qualifier(CppCv cv, CppReference ref) noexcept {
                auto &base = static_cast<CppMemberFunctionBase &>(*this->function);
                base.cv_ = cv;
                base.ref_ = ref;
            }

            /// \effects Sets the `virtual`-ness of the function.
            void virtual_info(type_safe::flag_set<CppVirtualFlags> virt) noexcept {
                static_cast<CppMemberFunctionBase &>(*this->function).virtual_ = virt;
            }

            /// \effects Marks the function as `constexpr`.
            void is_constexpr() noexcept {
                static_cast<CppMemberFunctionBase &>(*this->function).constexpr_ = true;
            }

            /// \effects Marks the function as `consteval`.
            void is_consteval() noexcept {
                static_cast<CppMemberFunctionBase &>(*this->function).consteval_ = true;
            }

        protected:
            basic_member_builder() noexcept = default;
        };

        /// \effects Sets name and return type, as well as the rest to defaults.
        CppMemberFunctionBase(std::string name, std::unique_ptr<CppType> return_type)
            : CppFunctionBase(std::move(name)), return_type_(std::move(return_type)), cv_(cpp_cv_none),
              ref_(cpp_ref_none), constexpr_(false), consteval_(false) {
        }

    protected:
        std::string do_get_signature() const override;

    private:
        std::unique_ptr<CppType> return_type_;
        CppVirtual virtual_;
        CppCv cv_;
        CppReference ref_;
        bool constexpr_;
        bool consteval_;
    };

    /// A [cppast::CppEntity]() modelling a member function.
    class CppMemberFunction final : public CppMemberFunctionBase {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppMemberFunction]().
        class builder : public CppMemberFunctionBase::basic_member_builder<CppMemberFunction> {
        public:
            using CppMemberFunctionBase::basic_member_builder<
                CppMemberFunction>::basic_member_builder;
        };

    private:
        using CppMemberFunctionBase::CppMemberFunctionBase;

        CppEntityKind do_get_entity_kind() const noexcept override;

        friend basic_member_builder<CppMemberFunction>;
    };

    /// A [cppast::CppEntity]() modelling a C++ conversion operator.
    class CppConversionOp final : public CppMemberFunctionBase {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppConversionOp]().
        class builder : public basic_member_builder<CppConversionOp> {
        public:
            using basic_member_builder::basic_member_builder;

            /// \effects Marks the conversion operator `explicit`.
            void is_explicit() noexcept {
                function->explicit_ = true;
            }

        private:
            using basic_member_builder::add_parameter;
            using basic_member_builder::is_variadic;
        };

        /// \returns Whether or not the conversion is `explicit`.
        bool is_explicit() const noexcept {
            return explicit_;
        }

    private:
        CppConversionOp(std::string name, std::unique_ptr<CppType> return_t)
            : CppMemberFunctionBase(std::move(name), std::move(return_t)), explicit_(false) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        bool explicit_;

        friend basic_member_builder<CppConversionOp>;
    };

    /// A [cppast::CppEntity]() modelling a C++ constructor.
    class CppConstructor final : public CppFunctionBase {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppConstructor]().
        class builder : public basic_builder<CppConstructor> {
        public:
            using basic_builder::basic_builder;

            /// \effects Marks the constructor `explicit`.
            void is_explicit() noexcept {
                function->explicit_ = true;
            }

            /// \effects Marks the constructor `constexpr`.
            void is_constexpr() noexcept {
                function->constexpr_ = true;
            }

            /// \effects Marks the constructor `consteval`.
            void is_consteval() noexcept {
                function->consteval_ = true;
            }
        };

        /// \returns Whether or not the constructor is `explicit`.
        bool is_explicit() const noexcept {
            return explicit_;
        }

        /// \returns Whether or not the constructor is `constexpr`.
        bool is_constexpr() const noexcept {
            return constexpr_;
        }

        /// \returns Whether or not the constructor is `consteval`.
        bool is_consteval() const noexcept {
            return consteval_;
        }

    private:
        CppConstructor(std::string name)
            : CppFunctionBase(std::move(name)), explicit_(false), constexpr_(false), consteval_(false) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        bool explicit_;
        bool constexpr_;
        bool consteval_;

        friend basic_builder<CppConstructor>;
    };

    /// A [cppast::CppEntity]() modelling a C++ destructor.
    class CppDestructor final : public CppFunctionBase {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppDestructor]().
        class builder : public basic_builder<CppDestructor> {
        public:
            using basic_builder::basic_builder;

            /// \effects Sets the `virtual`-ness of the destructor.
            void virtual_info(CppVirtual virt) noexcept {
                function->virtual_ = virt;
            }

        private:
            using basic_builder::add_parameter;
            using basic_builder::is_variadic;
        };

        /// \returns Whether or not it is `virtual`.
        bool is_virtual() const noexcept {
            return virtual_info().has_value();
        }

        /// \returns The `virtual`-ness of the constructor.
        CppVirtual virtual_info() const noexcept {
            return virtual_;
        }

    private:
        CppDestructor(std::string name) : CppFunctionBase(std::move(name)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppVirtual virtual_;

        friend basic_builder<CppDestructor>;
    };
} // namespace cppast
