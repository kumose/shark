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


#include <shark/ast/cpp_language_linkage.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppLanguageLinkage::kind() noexcept {
    return CppEntityKind::language_linkage_t;
}

bool CppLanguageLinkage::is_block() const noexcept {
    if (begin() == end()) {
        // An empty container must be a "block" of the form: extern "C" {}
        return true;
    }
    return std::next(begin()) != end(); // more than one entity, so block
}

CppEntityKind CppLanguageLinkage::do_get_entity_kind() const noexcept {
    return kind();
}
