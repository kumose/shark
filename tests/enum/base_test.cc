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
#include <tests/enum/base.tml.h>   // generated header with Color and parse/to_string
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

using my::em::Color;
using my::em::parse_Color;
using my::em::to_string;

/// Test that to_string() returns correct string for each enum value.
TEST(ColorTest, ToStringReturnsCorrectString) {
    EXPECT_EQ(to_string(Color::RED), "RED");
    EXPECT_EQ(to_string(Color::GREEN), "GREEN");
    EXPECT_EQ(to_string(Color::BLUE), "BLUE");
}

/// Test that parse_Color correctly parses valid strings.
TEST(ColorTest, ParseColorValidInput) {
    auto opt = parse_Color("RED");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Color::RED);

    opt = parse_Color("GREEN");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Color::GREEN);

    opt = parse_Color("BLUE");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Color::BLUE);
}

/// Test that parse_Color returns nullopt for invalid strings.
TEST(ColorTest, ParseColorInvalidInput) {
    EXPECT_FALSE(parse_Color("red").has_value());      // case mismatch
    EXPECT_FALSE(parse_Color("YELLOW").has_value());   // unknown
    EXPECT_FALSE(parse_Color("").has_value());         // empty
    EXPECT_FALSE(parse_Color("RED ").has_value());     // trailing space
    EXPECT_FALSE(parse_Color(" BLUE").has_value());    // leading space
}

/// Test round-trip via TOML serialization/deserialization.
/// This tests that the generated code can extract enum from a TOML primitive.
TEST(ColorTest, TomlRoundTrip) {
    // Original enum value to be serialized.
    Color original = Color::GREEN;

    // Serialize to TOML Value.
    xtoml::Value out;
    out["color"] = xtoml::Value(to_string(original));

    // Convert to string and parse back (the string format is not directly compared).
    std::string toml_str = xtoml::serialize(out).value_or_die();
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    ASSERT_TRUE(parsed_val.is_table());

    // Extract the "color" field and parse it back to enum.
    const auto& color_node = parsed_val.as_table().at("color");
    ASSERT_TRUE(color_node.is_string());
    auto opt_enum = parse_Color(color_node.as_string());
    ASSERT_TRUE(opt_enum.has_value());

    // Compare the original and the round-tripped enum value.
    EXPECT_EQ(*opt_enum, original);
}