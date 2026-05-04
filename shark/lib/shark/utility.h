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

#include <turbo/utility/status.h>
#include <turbo/log/logging.h>
#include <xtoml/toml.h>

namespace shark {


    template<typename T>
    inline turbo::Status safe_find_array(const xtoml::Value &value, const std::string &key, std::vector<T> &result) {
        TURBO_MOVE_OR_RAISE(auto const *arr, xtoml::try_find_key(value, key));
        auto &a = arr->as_array();
        result.reserve(a.size());
        for (auto & it : a) {
            T t;
            TURBO_RETURN_NOT_OK((xtoml::ValueTraits< T, xtoml::Value::config_type>::primitive_extract(it, &t)));
            result.push_back(t);
        }
        return turbo::OkStatus();
    }

} // namespace shark
