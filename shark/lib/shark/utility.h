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

#include <shark/toml/color.h>
#include <shark/toml/comments.h>
#include <shark/toml/compat.h>
#include <shark/toml/context.h>
#include <shark/toml/conversion.h>
#include <shark/toml/datetime.h>
#include <shark/toml/error_info.h>
#include <shark/toml/exception.h>
#include <shark/toml/find.h>
#include <shark/toml/format.h>
#include <shark/toml/from.h>
#include <shark/toml/get.h>
#include <shark/toml/into.h>
#include <shark/toml/literal.h>
#include <shark/toml/location.h>
#include <shark/toml/ordered_map.h>
#include <shark/toml/parser.h>
#include <shark/toml/region.h>
#include <shark/toml/result.h>
#include <shark/toml/scanner.h>
#include <shark/toml/serializer.h>
#include <shark/toml/skip.h>
#include <shark/toml/source_location.h>
#include <shark/toml/spec.h>
#include <shark/toml/storage.h>
#include <shark/toml/syntax.h>
#include <shark/toml/traits.h>
#include <shark/toml/types.h>
#include <shark/toml/utility.h>
#include <shark/toml/value.h>
#include <shark/toml/value_t.h>
#include <shark/toml/version.h>
#include <shark/toml/visit.h>
#include  <turbo/utility/status.h>
#include <turbo/log/logging.h>

namespace shark {
    inline turbo::Status safe_find_string(const shark::Value &value, const std::string &key, std::string &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }

        if (!it->second.is_string()) {
            return turbo::invalid_argument_error("found key:", key, " but miss type");
        }
        result = it->second.as_string();
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_int64(const shark::Value &value, const std::string &key, int64_t &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }
        if (!it->second.is_integer()) {
            return turbo::invalid_argument_error("found key:", key, " but not integer");
        }
        result = it->second.as_integer();
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_uint64(const shark::Value &value, const std::string &key, uint64_t &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }
        if (!it->second.is_integer()) {
            return turbo::invalid_argument_error("found key:", key, " but not integer");
        }
        result = static_cast<uint64_t>(it->second.as_integer());
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_double(const shark::Value &value, const std::string &key, double &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }
        if (!it->second.is_floating()) {
            return turbo::invalid_argument_error("found key:", key, " but not floating");
        }
        result = it->second.as_floating();
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_bool(const shark::Value &value, const std::string &key, bool &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }
        if (!it->second.is_boolean()) {
            return turbo::invalid_argument_error("found key:", key, " but not boolean");
        }
        result = it->second.as_boolean();
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_array(const shark::Value &value, const std::string &key, shark::Array &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);

        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }

        if (!it->second.is_array()) {
            return turbo::invalid_argument_error("found key:", key, " but not array");
        }
        result = it->second.as_array();
        return turbo::OkStatus();
    }

    inline turbo::Status safe_find_table(const shark::Value &value, const std::string &key, shark::Value &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("shark::Value::contains(key_type)", value_t::table);
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("not found key:", key);
        }
        if (!it->second.is_table()) {
            return turbo::invalid_argument_error("found key:", key, " but not table");
        }
        result = it->second;
        return turbo::OkStatus();
    }

    template<typename T>
    inline turbo::Status safe_find_primitive(const shark::Value &value, const std::string &key, T &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("value is not a table");
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("key not found: ", key);
        }
        const auto &v = it->second;
        if constexpr (std::is_same_v<T, std::string>) {
            if (!v.is_string()) {
                return turbo::invalid_argument_error("not a string");
            }
            result = v.as_string();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (!v.is_boolean()) {
                return turbo::invalid_argument_error("not a boolean");
            }
            result = v.as_boolean();
        } else if constexpr (std::is_floating_point_v<T>) {
            if (!v.is_floating()) {
                return turbo::invalid_argument_error("not a floating");
            }
            result = static_cast<T>(v.as_floating());
        } else if constexpr (std::is_integral_v<T>) {
            if (!v.is_integer()) {
                return turbo::invalid_argument_error("not an integer");
            }
            result = static_cast<T>(v.as_integer());
        } else {
            static_assert(sizeof(T) == 0, "Unsupported primitive type");
        }
        return turbo::OkStatus();
    }

    template<typename T>
    inline turbo::Status safe_find_array(const shark::Value &value, const std::string &key, std::vector<T> &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("value is not a table");
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::not_found_error("key not found: ", key);
        }
        if (!it->second.is_array()) {
            return turbo::invalid_argument_error("key ", key, " is not an array");
        }
        const auto &arr = it->second.as_array();
        result.clear();
        result.reserve(arr.size());

        for (const auto &elem: arr) {
            if constexpr (std::is_same_v<T, std::string>) {
                if (!elem.is_string()) {
                    return turbo::invalid_argument_error("array element not string");
                }
                result.push_back(elem.as_string());
            } else if constexpr (std::is_same_v<T, bool>) {
                if (!elem.is_boolean()) {
                    return turbo::invalid_argument_error("array element not boolean");
                }
                result.push_back(elem.as_boolean());
            } else if constexpr (std::is_floating_point_v<T>) {
                if (!elem.is_floating()) {
                    return turbo::invalid_argument_error("array element not floating");
                }
                result.push_back(static_cast<T>(elem.as_floating()));
            } else if constexpr (std::is_integral_v<T>) {
                if (!elem.is_integer()) {
                    return turbo::invalid_argument_error("array element not integer");
                }
                // Convert int64_t to T (implicit conversion, may truncate)
                result.push_back(static_cast<T>(elem.as_integer()));
            } else {
                static_assert(sizeof(T) == 0, "Unsupported array element type");
            }
        }
        return turbo::OkStatus();
    }

    template<typename T>
   inline turbo::Status safe_try_find_primitive(const shark::Value &value, const std::string &key, T &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("value is not a table");
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::OkStatus();
        }
        const auto &v = it->second;
        if constexpr (std::is_same_v<T, std::string>) {
            if (!v.is_string()) {
                return turbo::invalid_argument_error("not a string");
            }
            result = v.as_string();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (!v.is_boolean()) {
                return turbo::invalid_argument_error("not a boolean");
            }
            result = v.as_boolean();
        } else if constexpr (std::is_floating_point_v<T>) {
            if (!v.is_floating()) {
                return turbo::invalid_argument_error("not a floating");
            }
            result = static_cast<T>(v.as_floating());
        } else if constexpr (std::is_integral_v<T>) {
            if (!v.is_integer()) {
                return turbo::invalid_argument_error("not an integer");
            }
            result = static_cast<T>(v.as_integer());
        } else {
            static_assert(sizeof(T) == 0, "Unsupported primitive type");
        }
        return turbo::OkStatus();
    }

    template<typename T>
    inline turbo::Status safe_try_find_array(const shark::Value &value, const std::string &key, std::vector<T> &result) {
        if (!value.is_table()) {
            return turbo::invalid_argument_error("value is not a table");
        }
        const auto &table = value.as_table(std::nothrow);
        auto it = table.find(key);
        if (it == table.end()) {
            return turbo::OkStatus();
        }
        if (!it->second.is_array()) {
            return turbo::invalid_argument_error("key ", key, " is not an array");
        }
        const auto &arr = it->second.as_array();
        result.clear();
        result.reserve(arr.size());

        for (const auto &elem: arr) {
            if constexpr (std::is_same_v<T, std::string>) {
                if (!elem.is_string()) {
                    return turbo::invalid_argument_error("array element not string");
                }
                result.push_back(elem.as_string());
            } else if constexpr (std::is_same_v<T, bool>) {
                if (!elem.is_boolean()) {
                    return turbo::invalid_argument_error("array element not boolean");
                }
                result.push_back(elem.as_boolean());
            } else if constexpr (std::is_floating_point_v<T>) {
                if (!elem.is_floating()) {
                    return turbo::invalid_argument_error("array element not floating");
                }
                result.push_back(static_cast<T>(elem.as_floating()));
            } else if constexpr (std::is_integral_v<T>) {
                if (!elem.is_integer()) {
                    return turbo::invalid_argument_error("array element not integer");
                }
                // Convert int64_t to T (implicit conversion, may truncate)
                result.push_back(static_cast<T>(elem.as_integer()));
            } else {
                static_assert(sizeof(T) == 0, "Unsupported array element type");
            }
        }
        return turbo::OkStatus();
    }

} // namespace shark
