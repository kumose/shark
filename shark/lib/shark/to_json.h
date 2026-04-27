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

#include <iomanip>
#include <iostream>
#include <string>

#include <shark/toml.h>
#include <nlohmann/json.hpp>

#include <cmath>

namespace shark {
#ifdef TOML11_TO_JSON_USE_V1_1_0
#  define TOML11_TO_JSON_SPEC shark::spec::v(1, 1, 0)
#else
#  define TOML11_TO_JSON_SPEC shark::spec::v(1, 0, 0)
#endif

    template<typename Value = shark::Value>
    struct JsonConverterDebug {
        using value_type = Value;
        using boolean_type = typename value_type::boolean_type;
        using integer_type = typename value_type::integer_type;
        using floating_type = typename value_type::floating_type;
        using string_type = typename value_type::string_type;
        using local_time_type = typename value_type::local_time_type;
        using local_date_type = typename value_type::local_date_type;
        using local_datetime_type = typename value_type::local_datetime_type;
        using offset_datetime_type = typename value_type::offset_datetime_type;
        using array_type = typename value_type::array_type;
        using table_type = typename value_type::table_type;

        nlohmann::json operator()(boolean_type v) {
            nlohmann::json j;
            j["type"] = "bool";
            if (v) {
                j["value"] = "true";
            } else {
                j["value"] = "false";
            }
            return j;
        }

        nlohmann::json operator()(integer_type v) {
            nlohmann::json j;
            j["type"] = "integer";
            j["value"] = std::to_string(v);
            return j;
        }

        nlohmann::json operator()(floating_type v) {
            std::ostringstream oss;
            oss << std::setprecision(16);
            // if we set the precision as max_digit, 1.1 will be 1.1000000000000001.
            // But toml-test does not allow 1.1000000000000001.
            if (std::isnan(v) && std::signbit(v)) {
                // toml-test does not allow negative NaN represented in "-nan" because
                // there are languages that does not distinguish nan and -nan.
                // But toml11 keeps sign from input. To resolve this difference,
                // we convert -nan to nan here.
                v = std::numeric_limits<floating_type>::quiet_NaN();
            }
            oss << v;

            nlohmann::json j;
            j["type"] = "float";
            j["value"] = oss.str();
            return j;
        }

        nlohmann::json operator()(const string_type &v) {
            nlohmann::json j;
            j["type"] = "string";
            j["value"] = v;
            return j;
        }

        nlohmann::json operator()(const shark::local_time &v) {
            nlohmann::json j;
            j["type"] = "time-local";
            j["value"] = shark::to_string(v);
            return j;
        }

        nlohmann::json operator()(const shark::local_date &v) {
            nlohmann::json j;
            j["type"] = "date-local";
            j["value"] = shark::to_string(v);
            return j;
        }

        nlohmann::json operator()(const shark::local_datetime &v) {
            nlohmann::json j;
            j["type"] = "datetime-local";
            j["value"] = shark::to_string(v);
            return j;
        }

        nlohmann::json operator()(const shark::offset_datetime &v) {
            nlohmann::json j;
            j["type"] = "datetime";
            j["value"] = shark::to_string(v);
            return j;
        }

        nlohmann::json operator()(const shark::Array &v) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto &elem: v) {
                j.push_back(shark::visit(*this, elem));
            }
            return j;
        }

        nlohmann::json operator()(const shark::Table &v) {
            nlohmann::json j = nlohmann::json::object();
            for (const auto &kv: v) {
                j[kv.first] = shark::visit(*this, kv.second);
            }
            return j;
        }

        static nlohmann::json to_json(const shark::Value &v) {
            nlohmann::json j = shark::visit(JsonConverterDebug<>{}, v);
            return j;
        }
    };

    template<typename Value = shark::Value>
    struct json_converter {
        using value_type = Value;
        using boolean_type = typename value_type::boolean_type;
        using integer_type = typename value_type::integer_type;
        using floating_type = typename value_type::floating_type;
        using string_type = typename value_type::string_type;
        using local_time_type = typename value_type::local_time_type;
        using local_date_type = typename value_type::local_date_type;
        using local_datetime_type = typename value_type::local_datetime_type;
        using offset_datetime_type = typename value_type::offset_datetime_type;
        using array_type = typename value_type::array_type;
        using table_type = typename value_type::table_type;

        nlohmann::json operator()(boolean_type v) {
            return v;
        }

        nlohmann::json operator()(integer_type v) {
            return v;
        }

        nlohmann::json operator()(floating_type v) {
            if (std::isnan(v) && std::signbit(v)) {
                v = std::numeric_limits<floating_type>::quiet_NaN();
            }
            return v;
        }

        nlohmann::json operator()(const string_type &v) {
            return v;
        }

        nlohmann::json operator()(const local_time_type &v) {
            return to_string(v);
        }

        nlohmann::json operator()(const local_date_type &v) {
            return to_string(v);
        }

        nlohmann::json operator()(const local_datetime_type &v) {
            return to_string(v);
        }

        nlohmann::json operator()(const offset_datetime_type &v) {
            return to_string(v);
        }

        nlohmann::json operator()(const array_type &v) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto &elem: v) {
                j.push_back(visit(*this, elem));
            }
            return j;
        }

        nlohmann::json operator()(const table_type &v) {
            nlohmann::json j = nlohmann::json::object();
            for (const auto &kv: v) {
                j[kv.first] = visit(*this, kv.second);
            }
            return j;
        }

        static nlohmann::json to_json(const value_type &v) {
            return visit(json_converter{}, v);
        }
    };
} // namespace shark
