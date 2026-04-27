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


#include <shark/to_toml.h>

namespace shark {
   shark::Value convert_to_toml_debug(const nlohmann::json &j) {
        if (j.is_array()) {
            shark::Value a(shark::Array{});
            for (const auto &v: j) {
                a.push_back(convert_to_toml(v));
            }
            return a;
        } else if (j.size() == 2 &&
                   j.contains("type") && j.at("type").is_string() &&
                   j.contains("value") && j.at("value").is_string()) {
            const auto type = j.at("type").get<std::string>();
            const auto value = j.at("value").get<std::string>();
            if (type == "string") {
                return shark::Value(value);
            } else if (type == "int") {
                shark::detail::context<shark::type_config> ctx(shark::spec::default_version());
                auto loc = shark::detail::make_temporary_location(value);
                return shark::detail::parse_integer(loc, ctx).unwrap();
            } else if (type == "float") {
                shark::detail::context<shark::type_config> ctx(shark::spec::default_version());
                auto loc = shark::detail::make_temporary_location(value);
                if (auto f_r = shark::detail::parse_floating(loc, ctx)) {
                    return f_r.unwrap();
                } else // not conform TOML floating-point syntax.
                {
                    // toml-test converts "inf" into "Inf"
                    if (value == "Inf" || value == "+Inf") {
                        return shark::Value(std::numeric_limits<double>::infinity());
                    } else if (value == "-Inf") {
                        return shark::Value(-std::numeric_limits<double>::infinity());
                    } else // sometimes toml-test uses large int with type:float
                    {
                        shark::floating_format_info fmt;
                        fmt.prec = value.size();
                        return shark::Value(shark::detail::from_string<double>(value).unwrap(), fmt);
                    }
                }
            } else {
                return shark::detail::literal_internal_impl(
                    shark::detail::make_temporary_location(value));
            }
        } else // table.
        {
            shark::Value t(shark::Table{});
            for (const auto &kv: j.items()) {
                t[kv.key()] = convert_to_toml(kv.value());
            }
            return t;
        }
    }

    shark::Value convert_to_toml(const nlohmann::json &j) {
       if (j.is_array()) {
           shark::Value a(shark::Array{});
           for (const auto &v : j) {
               a.push_back(convert_to_toml(v));
           }
           return a;
       } else if (j.is_boolean()) {
           return shark::Value(j.get<bool>());
       } else if (j.is_number_integer()) {
           return shark::Value(j.get<int64_t>());
       } else if (j.is_number_float()) {
           return shark::Value(j.get<double>());
       } else if (j.is_string()) {
           return shark::Value(j.get<std::string>());
       } else if (j.is_object()) {
           shark::Value t(shark::Table{});
           for (const auto &kv : j.items()) {
               t[kv.key()] = convert_to_toml(kv.value());
           }
           return t;
       }

       return shark::Value();
   }
} // namespace shark
