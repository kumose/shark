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
#include <tests/require/repeat.tml.h>
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

using my::base::BaseRepeat;

/// Test that serialize_required_toml() outputs only required-marked repeated fields,
/// and ordinary repeated fields are omitted. Verify that the extracted values
/// round-trip correctly.
TEST(BaseRepeatTest, RequiredSerializationOnlyOutputsRequiredMarkedRepeatedFields) {
    BaseRepeat config;

    // Ordinary repeated fields (should NOT appear in required serialization)
    config.friends = {"alice", "bob"};
    config.repeated_int32 = {10, 20};
    config.repeated_int64 = {100, 200};
    config.repeated_uint32 = {1, 2};
    config.repeated_uint64 = {3, 4};
    config.repeated_sint32 = {-5, -6};
    config.repeated_sint64 = {-7, -8};
    config.repeated_fixed32 = {9, 10};
    config.repeated_fixed64 = {11, 12};
    config.repeated_sfixed32 = {-13, -14};
    config.repeated_sfixed64 = {-15, -16};
    config.repeated_float = {1.1f, 2.2f};
    config.repeated_double = {3.3, 4.4};
    config.repeated_bool = {true, false};
    config.repeated_string = {"foo", "bar"};

    // Required-marked repeated fields (MUST appear with correct values)
    config.age = {1, 2, 3};
    config.required_friends = {"carol", "dave"};
    config.required_int32 = {100, 200};
    config.required_int64 = {1000, 2000};
    config.required_uint32 = {300, 400};
    config.required_uint64 = {500, 600};
    config.required_sint32 = {-30, -40};
    config.required_sint64 = {-50, -60};
    config.required_fixed32 = {700, 800};
    config.required_fixed64 = {900, 1000};
    config.required_sfixed32 = {-110, -120};
    config.required_sfixed64 = {-130, -140};
    config.required_float = {5.5f, 6.6f};
    config.required_double = {7.7, 8.8};
    config.required_bool = {true, true, false};
    config.required_string = {"hello", "world"};

    // Serialize required fields to TOML.
    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    ASSERT_TRUE(parsed_val.is_table());

    // Helper lambda to extract an array via find_key_array and compare.
    auto check_array = [&]<typename T>(const std::string& key, const std::vector<T>& expected) {
        std::vector<T> actual;
        auto status = xtoml::find_key_array(parsed_val, key, actual);
        EXPECT_TRUE(status.ok()) << "Failed to extract " << key << ": " << status;
        if (status.ok()) {
            EXPECT_EQ(actual, expected) << "Array mismatch for " << key;
        }
    };

    // Verify all required repeated fields are present and equal.
    check_array("age", std::vector<int32_t>{1, 2, 3});
    check_array("required_friends", std::vector<std::string>{"carol", "dave"});
    check_array("required_int32", std::vector<int32_t>{100, 200});
    check_array("required_int64", std::vector<int64_t>{1000, 2000});
    check_array("required_uint32", std::vector<uint32_t>{300, 400});
    check_array("required_uint64", std::vector<uint64_t>{500, 600});
    check_array("required_sint32", std::vector<int32_t>{-30, -40});
    check_array("required_sint64", std::vector<int64_t>{-50, -60});
    check_array("required_fixed32", std::vector<uint32_t>{700, 800});
    check_array("required_fixed64", std::vector<uint64_t>{900, 1000});
    check_array("required_sfixed32", std::vector<int32_t>{-110, -120});
    check_array("required_sfixed64", std::vector<int64_t>{-130, -140});
    check_array("required_float", std::vector<float>{5.5f, 6.6f});
    check_array("required_double", std::vector<double>{7.7, 8.8});
    check_array("required_bool", std::vector<bool>{true, true, false});
    check_array("required_string", std::vector<std::string>{"hello", "world"});

    // Verify that ordinary repeated fields are NOT present.
    // We attempt to extract them and expect a not-found or invalid error.
    auto check_not_present = [&](const std::string& key) {
        std::vector<int> dummy;
        auto status = xtoml::find_key_array(parsed_val, key, dummy);
        EXPECT_FALSE(status.ok()) << "Ordinary field " << key << " should not be present";
    };

    check_not_present("friends");
    check_not_present("repeated_int32");
    check_not_present("repeated_int64");
    check_not_present("repeated_uint32");
    check_not_present("repeated_uint64");
    check_not_present("repeated_sint32");
    check_not_present("repeated_sint64");
    check_not_present("repeated_fixed32");
    check_not_present("repeated_fixed64");
    check_not_present("repeated_sfixed32");
    check_not_present("repeated_sfixed64");
    check_not_present("repeated_float");
    check_not_present("repeated_double");
    check_not_present("repeated_bool");
    check_not_present("repeated_string");
}

/// Optional: Test that empty required repeated arrays are still output (if your design requires it).
/// By default this test is disabled because the behavior may depend on generator configuration.
TEST(BaseRepeatTest, DISABLED_RequiredRepeatedFieldsEmptyArrayStillAppears) {
    BaseRepeat config;   // All fields empty (including required repeated ones)

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();

    // Extract each required repeated field; they should exist and be empty arrays.
    std::vector<int32_t> age;
    auto status = xtoml::find_key_array(parsed_val, "age", age);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(age.empty());

    std::vector<std::string> required_friends;
    status = xtoml::find_key_array(parsed_val, "required_friends", required_friends);
    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(required_friends.empty());

    // Add similar checks for other required repeated fields as needed.
}