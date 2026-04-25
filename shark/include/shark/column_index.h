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
#include <cstdint>


namespace shark {
    class ColumnIndex {
    public:
        /// bit 0      : is_null
        /// bit 1      : inplace
        /// bit 2..31  : offset (30 bits)
        /// bit 32..63 : length (32 bits)
        static uint64_t build(uint32_t offset, int32_t length, bool inplace, bool is_null) {
            uint64_t val = 0;

            val |= static_cast<uint64_t>(is_null ? 1 : 0) << 0;
            val |= static_cast<uint64_t>(inplace ? 1 : 0) << 1;
            val |= (static_cast<uint64_t>(offset) & 0x3FFFFFFF) << 2;  // 30 bits
            val |= static_cast<uint64_t>(static_cast<uint32_t>(length)) << 32;

            return val;
        }

        static void decode(uint64_t val, uint32_t &offset, int32_t &length, bool &inplace, bool &is_null) {
            is_null = (val & (1ULL << 0)) != 0;
            inplace = (val & (1ULL << 1)) != 0;

            offset = static_cast<uint32_t>((val >> 2) & 0x3FFFFFFF);
            length = static_cast<int32_t>(val >> 32);
        }
    };
} // namespace shark
