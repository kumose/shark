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

namespace cppast {
    /// A [cppast::CppType]() that is a function.
    ///
    /// A function pointer is created by wrapping it in [cppast::CppPointerType]().
    class CppFunctionType final : public CppType {
    public:
        /// Builds a [cppast::CppFunctionType]().
        class builder {
        public:
            /// \effects Sets the return type.
            explicit builder(std::unique_ptr<CppType> return_type)
                : func_(new CppFunctionType(std::move(return_type))) {
            }

            /// \effects Adds an parameter type.
            void add_parameter(std::unique_ptr<CppType> arg) {
                func_->parameters_.push_back(*func_, std::move(arg));
            }

            /// \effects Adds an ellipsis, marking it as variadic.
            void is_variadic() {
                func_->variadic_ = true;
            }

            /// \returns The finished [cppast::CppFunctionType]().
            std::unique_ptr<CppFunctionType> finish() {
                return std::move(func_);
            }

        private:
            std::unique_ptr<CppFunctionType> func_;
        };

        /// \returns A reference to the return [cppast::CppType]().
        const CppType &return_type() const noexcept {
            return *return_type_;
        }

        /// \returns An iteratable object iterating over the parameter types.
        detail::iteratable_intrusive_list<CppType> parameter_types() const noexcept {
            return type_safe::ref(parameters_);
        }

        /// \returns Whether or not the function is variadic (C-style ellipsis).
        bool is_variadic() const noexcept {
            return variadic_;
        }

    private:
        CppFunctionType(std::unique_ptr<CppType> return_type)
            : return_type_(std::move(return_type)), variadic_(false) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::function_t;
        }

        std::unique_ptr<CppType> return_type_;
        detail::intrusive_list<CppType> parameters_;
        bool variadic_;
    };

    /// A [cppast::CppType]() that is a member function.
    ///
    /// A member function with cv qualifier is created by wrapping it in
    /// [cppast::CppCvQualifiedType](). A member function with reference qualifier is created by
    /// wrapping it in [cppast::CppReferenceType](). A member function pointer is created by wrapping
    /// it in [cppast::CppPointerType]().
    class CppMemberFunctionType final : public CppType {
    public:
        /// Builds a [cppast::CppMemberFunctionType]().
        class builder {
        public:
            /// \effects Sets the class and return type.
            builder(std::unique_ptr<CppType> class_type, std::unique_ptr<CppType> return_type)
                : func_(new CppMemberFunctionType(std::move(class_type), std::move(return_type))) {
            }

            /// \effects Adds a parameter type.
            void add_parameter(std::unique_ptr<CppType> arg) {
                func_->parameters_.push_back(*func_, std::move(arg));
            }

            /// \effects Adds an ellipsis, marking it as variadic.
            void is_variadic() {
                func_->variadic_ = true;
            }

            /// \returns The finished [cppast::CppMemberFunctionType]().
            std::unique_ptr<CppMemberFunctionType> finish() {
                return std::move(func_);
            }

        private:
            std::unique_ptr<CppMemberFunctionType> func_;
        };

        /// \returns A reference to the class [cppast::CppType]().
        const CppType &class_type() const noexcept {
            return *class_type_;
        }

        /// \returns A reference to the return [cppast::CppType]().
        const CppType &return_type() const noexcept {
            return *return_type_;
        }

        /// \returns An iteratable object iterating over the parameter types.
        detail::iteratable_intrusive_list<CppType> parameter_types() const noexcept {
            return type_safe::ref(parameters_);
        }

        /// \returns Whether or not the function is variadic (C-style ellipsis).
        bool is_variadic() const noexcept {
            return variadic_;
        }

    private:
        CppMemberFunctionType(std::unique_ptr<CppType> class_type,
                              std::unique_ptr<CppType> return_type)
            : class_type_(std::move(class_type)), return_type_(std::move(return_type)), variadic_(false) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::member_function_t;
        }

        std::unique_ptr<CppType> class_type_, return_type_;
        detail::intrusive_list<CppType> parameters_;
        bool variadic_;
    };

    /// A [cppast::CppType]() that is a member object.
    ///
    /// A member object pointer is created by wrapping it in [cppast::CppPointerType]().
    class CppMemberObjectType final : public CppType {
    public:
        /// \returns A newly created member object type.
        static std::unique_ptr<CppMemberObjectType> build(std::unique_ptr<CppType> class_type,
                                                          std::unique_ptr<CppType> object_type) {
            return std::unique_ptr<CppMemberObjectType>(
                new CppMemberObjectType(std::move(class_type), std::move(object_type)));
        }

        /// \returns A reference to the class [cppast::CppType]().
        const CppType &class_type() const noexcept {
            return *class_type_;
        }

        /// \returns A reference to the object [cppast::CppType]().
        const CppType &object_type() const noexcept {
            return *object_type_;
        }

    private:
        CppMemberObjectType(std::unique_ptr<CppType> class_type,
                            std::unique_ptr<CppType> object_type)
            : class_type_(std::move(class_type)), object_type_(std::move(object_type)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::member_object_t;
        }

        std::unique_ptr<CppType> class_type_, object_type_;
    };
} // namespace cppast
