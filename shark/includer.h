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

#include <turbo/container/flat_hash_set.h>

namespace cppast {
    class Includer {
    public:
        Includer() = default;

        ~Includer() = default;

        void system_include(std::string_view file) {
            _sys_includes.insert(std::string(file));
        }

        void local_include(std::string_view file) {
            _local_includes.insert(std::string(file));
        }

        const turbo::flat_hash_set<std::string>& system_includes() {
            return _sys_includes;
        }

        const turbo::flat_hash_set<std::string>& local_includes() {
            return _local_includes;
        }

    private:
        turbo::flat_hash_set<std::string> _sys_includes;
        turbo::flat_hash_set<std::string> _local_includes;
    };
} // namespace cppast
