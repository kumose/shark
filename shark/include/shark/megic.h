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

#include <shark/idl/shark_options.pb.h>

namespace shark {
    class SharkMagic {
    public:
        static constexpr std::string_view kMagic = "SK";
        static constexpr uint32_t kBaseMagic = 0x4B53; // ASCII 'SK'

        /**
         * @brief Build magic number with format and endianness.
         * @param fmt Serialization format (3 bits used).
         * @param bigendian True for big-endian, false for little-endian.
         * @return 32-bit magic value.
         *
         * Bit layout (32-bit):
         * [ 0-7]  'S'
         * [ 8-15] 'K'
         * [16-18] format (3 bits)
         * [19]    big-endian flag
         * [20-31] reserved (0)
         */
        static uint32_t build(idl::SharkFormat fmt, bool bigendian) {
            uint32_t magic = kBaseMagic;
            magic |= (static_cast<uint32_t>(fmt) & 0x7) << 16;
            if (bigendian) {
                magic |= 1U << 19;
            }
            return magic;
        }

        /// Check if the magic number starts with "SK".
        static bool is_shark_format(uint32_t magic) {
            return (magic & 0xFFFF) == kBaseMagic;
        }

        /// Extract format from magic number.
        static idl::SharkFormat format(uint32_t magic) {
            return static_cast<idl::SharkFormat>((magic >> 16) & 0x7);
        }

        /// Extract endianness flag from magic number.
        static bool is_bigendian(uint32_t magic) {
            return (magic & (1U << 19)) != 0;
        }
    };
} // namespace shark
