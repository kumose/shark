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

#include <gtest/gtest.h>
#include <tests/require/base.tml.h>
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

using my::base::Base;
using my::base::BaseEnum;

/// Test that serialize_required_toml() outputs only required fields
/// (native required + is_required marked), and no optional fields appear.
TEST(BaseTest, RequiredSerializationOnlyOutputsRequiredFields) {
    // 1. Create an object with non-default values for all fields.
    Base config;
    // Optional fields (should NOT appear in required output)
    config.benum = BaseEnum::B2;
    config.bbool = false;
    config.age = 100;
    config.int64_val = 1000;
    config.uint32_val = 2000;
    config.uint64_val = 3000;
    config.sint32_val = -10;
    config.sint64_val = -20;
    config.fixed32_val = 4000;
    config.fixed64_val = 5000;
    config.sfixed32_val = -30;
    config.sfixed64_val = -40;
    config.float_val = 1.23f;
    config.double_val = 4.56;
    config.string_val = "optional_string";
    // Required fields (should appear)
    config.require_benum = BaseEnum::B3;
    config.required_bbool = true;
    config.required_age = 30;
    config.required_int64 = 2000;
    config.required_uint32 = 3000;
    config.required_uint64 = 4000;
    config.required_sint32 = -50;
    config.required_sint64 = -60;
    config.required_fixed32 = 5000;
    config.required_fixed64 = 6000;
    config.required_sfixed32 = -70;
    config.required_sfixed64 = -80;
    config.required_float = 7.89f;
    config.required_double = 9.10;
    config.required_string = "required_string";

    // 2. Serialize required fields to TOML.
    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();
    std::cout << std::string('*', 100) << std::endl;
    std::cout << toml_str << std::endl;
    std::cout << std::string('*', 100) << std::endl;
    // 3. Parse back to TOML value.
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    ASSERT_TRUE(parsed_val.is_table());
    const auto& table = parsed_val.as_table();

    // 4. Verify all required fields are present.
    EXPECT_TRUE(table.count("require_benum"));
    EXPECT_TRUE(table.count("required_bbool"));
    EXPECT_TRUE(table.count("required_age"));
    EXPECT_TRUE(table.count("required_int64"));
    EXPECT_TRUE(table.count("required_uint32"));
    EXPECT_TRUE(table.count("required_uint64"));
    EXPECT_TRUE(table.count("required_sint32"));
    EXPECT_TRUE(table.count("required_sint64"));
    EXPECT_TRUE(table.count("required_fixed32"));
    EXPECT_TRUE(table.count("required_fixed64"));
    EXPECT_TRUE(table.count("required_sfixed32"));
    EXPECT_TRUE(table.count("required_sfixed64"));
    EXPECT_TRUE(table.count("required_float"));
    EXPECT_TRUE(table.count("required_double"));
    EXPECT_TRUE(table.count("required_string"));

    // 5. Verify no optional field appears.
    EXPECT_FALSE(table.count("benum"));
    EXPECT_FALSE(table.count("bbool"));
    EXPECT_FALSE(table.count("age"));
    EXPECT_FALSE(table.count("int64_val"));
    EXPECT_FALSE(table.count("uint32_val"));
    EXPECT_FALSE(table.count("uint64_val"));
    EXPECT_FALSE(table.count("sint32_val"));
    EXPECT_FALSE(table.count("sint64_val"));
    EXPECT_FALSE(table.count("fixed32_val"));
    EXPECT_FALSE(table.count("fixed64_val"));
    EXPECT_FALSE(table.count("sfixed32_val"));
    EXPECT_FALSE(table.count("sfixed64_val"));
    EXPECT_FALSE(table.count("float_val"));
    EXPECT_FALSE(table.count("double_val"));
    EXPECT_FALSE(table.count("string_val"));
}