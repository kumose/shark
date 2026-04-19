// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
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

#include <memory>
#include <string>
#include <vector>

namespace cppast {
    class CppType;
}

namespace shark::generator {
    /// Represents a C++ type to be generated. This is a lightweight wrapper around
    /// cppast::CppType, providing convenient factory methods for common types.
    class Type {
    public:
        enum class Kind {
            k_void,
            k_bool,
            k_int32,
            k_int64,
            k_uint32,
            k_uint64,
            k_float,
            k_double,
            k_string,
            k_vector,
            k_unordered_map,
            k_user_defined,
            k_enum,
        };

    public:
        /// Creates a void type.
        static Type void_type();

        /// Creates a bool type.
        static Type bool_type();

        /// Creates a signed 32-bit integer type.
        static Type int32_type();

        /// Creates a signed 64-bit integer type.
        static Type int64_type();

        /// Creates an unsigned 32-bit integer type.
        static Type uint32_type();

        /// Creates an unsigned 64-bit integer type.
        static Type uint64_type();

        /// Creates a float type.
        static Type float_type();

        /// Creates a double type.
        static Type double_type();

        /// Creates a std::string type.
        static Type string_type();

        /// Creates a std::vector<T> type.
        static Type vector_type(Type element);

        /// Creates a std::unordered_map<K, V> type.
        static Type unordered_map_type(Type key, Type value);

        /// Creates a user-defined type by its fully qualified name (e.g., "MyMessage" or "std::string").
        static Type user_defined(std::string name);

        /// Creates an enum type by its fully qualified name.
        static Type enum_type(std::string name);

        /// Builds the underlying cppast::CppType object.
        /// This is called when the AST is being constructed.
        std::unique_ptr<cppast::CppType> build() const;

        Kind kind() const { return kind_; }

        const std::vector<Type> &parameters() const { return parameters_; }

    private:
        Kind kind_;
        std::vector<Type> parameters_; ///< For template types, e.g., vector element.
        std::string name_; ///< For user-defined or enum types.
    };
} // namespace shark::generator
