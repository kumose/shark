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

#include <xsimd/xsimd.hpp>
#include <xsimd/memory/xsimd_aligned_allocator.hpp>
#include <vector>
#include <cstdint>
#include <turbo/container/span.h>

namespace shark {

    template<typename T, size_t Alignment>
    using AlignedVector = std::vector<T, xsimd::aligned_allocator<T, Alignment> >;

    template<typename T, size_t Alignment>
    class AlignedSpan : public turbo::span<T> {
    private:
        using Base = turbo::span<T>;

        static_assert(Alignment != 0, "Alignment must not be zero");
        static_assert((Alignment & (Alignment - 1)) == 0,
                      "Alignment must be a power of two");

        static constexpr bool IsAligned(const void *ptr) noexcept {
            return (reinterpret_cast<uintptr_t>(ptr) & (Alignment - 1)) == 0;
        }

    public:
        using Base::Base;

        constexpr AlignedSpan(T *ptr, size_t size) noexcept : Base(ptr, size) {
            TURBO_HARDENING_ASSERT(IsAligned(ptr));
        }

        constexpr AlignedSpan(T *ptr, T *end) noexcept : Base(ptr, end - ptr) {
            TURBO_HARDENING_ASSERT(IsAligned(ptr));
        }

        template<size_t N>
        constexpr AlignedSpan(T (&arr)[N]) noexcept : Base(arr, N) {
            TURBO_HARDENING_ASSERT(IsAligned(arr));
        }

        explicit constexpr AlignedSpan(const Base &span) noexcept : Base(span) {
            TURBO_HARDENING_ASSERT(IsAligned(span.data()));
        }

        constexpr AlignedSpan subspan(size_t pos = 0, size_t len = Base::npos) const {
            return AlignedSpan{Base::subspan(pos, len)};
        }

        constexpr AlignedSpan first(size_t len) const {
            return AlignedSpan{Base::first(len)};
        }

        constexpr AlignedSpan last(size_t len) const {
            return AlignedSpan{Base::last(len)};
        }

        void remove_prefix(size_t n) noexcept {
            Base::remove_prefix(n);
            TURBO_HARDENING_ASSERT(IsAligned(this->data()));
        }

        void remove_suffix(size_t n) noexcept {
            Base::remove_suffix(n);
        }
    };

    template<int &... ExplicitArgumentBarrier, typename T, size_t Alignment>
    constexpr AlignedSpan<T, Alignment> make_aligned_span(T *ptr, size_t size) noexcept {
        return AlignedSpan<T, Alignment>(ptr, size);
    }
} // namespace shark
