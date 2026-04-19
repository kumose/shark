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

#include <typesafe/debug_assert.hpp>

#ifndef CPPAST_ASSERTION_LEVEL
#    define CPPAST_ASSERTION_LEVEL 0
#endif

#ifndef CPPAST_PRECONDITION_LEVEL
#    ifdef NDEBUG
#        define CPPAST_PRECONDITION_LEVEL 0
#    else
#        define CPPAST_PRECONDITION_LEVEL 1
#    endif
#endif

namespace cppast {
    namespace detail {
        struct assert_handler : debug_assert::set_level<CPPAST_ASSERTION_LEVEL>,
                                debug_assert::default_handler {
        };

        struct precondition_error_handler : debug_assert::set_level<CPPAST_PRECONDITION_LEVEL>,
                                            debug_assert::default_handler {
        };
    } // namespace detail
} // namespace cppast
