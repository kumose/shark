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


#include <shark/default_generator.h>
#include <atomic>

namespace cppast {

    std::atomic<uint32_t> g_seq{0};

    cppast::CppEntityId get_seq_entity_id(std::string_view id) {
        std::string id_str(id);
        if (id_str.empty()) {
            id_str = std::to_string(g_seq.fetch_add(std::memory_order_relaxed));
        }
        return cppast::CppEntityId(id_str);
    }

}  // namespace cppast
