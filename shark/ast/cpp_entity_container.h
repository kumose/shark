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

#include <shark/ast/cpp_entity.h>

namespace cppast {
    /// Helper class for entities that are containers.
    ///
    /// Inherit from it to generate container access.
    template<class Derived, typename T>
    class CppEntityContainer {
    public:
        using iterator = typename detail::intrusive_list<T>::const_iterator;

        /// \returns A const iterator to the first child.
        iterator begin() const noexcept {
            return children_.begin();
        }

        /// \returns A const iterator to the last child.
        iterator end() const noexcept {
            return children_.end();
        }

        /// \effects Adds a new child to the container.
        void add_child(std::unique_ptr<T> ptr) noexcept {
            children_.push_back(static_cast<Derived &>(*this), std::move(ptr));
        }

    protected:
        /// \returns A non-const iterator to the first child.
        typename detail::intrusive_list<T>::iterator mutable_begin() noexcept {
            return children_.begin();
        }

        /// \returns A non-const iterator one past the last child.
        typename detail::intrusive_list<T>::iterator mutable_end() noexcept {
            return children_.begin();
        }

        ~CppEntityContainer() noexcept = default;

    private:
        detail::intrusive_list<T> children_;
    };
} // namespace cppast
