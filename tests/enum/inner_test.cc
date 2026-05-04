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
#include <tests/enum/inner.tml.h>
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

using inner::Outter;
using inner::parse_Outter_Color;
using inner::to_string;

/// Test that to_string() returns correct string for each enum value.
TEST(OutterTest, ToStringReturnsCorrectString) {
    EXPECT_EQ(to_string(Outter::Color::RED), "RED");
    EXPECT_EQ(to_string(Outter::Color::GREEN), "GREEN");
    EXPECT_EQ(to_string(Outter::Color::BLUE), "BLUE");
}

/// Test that parse_Outter_Color correctly parses valid strings.
TEST(OutterTest, ParseColorValidInput) {
    auto opt = parse_Outter_Color("RED");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Outter::Color::RED);

    opt = parse_Outter_Color("GREEN");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Outter::Color::GREEN);

    opt = parse_Outter_Color("BLUE");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Outter::Color::BLUE);
}

/// Test that parse_Outter_Color returns nullopt for invalid strings.
TEST(OutterTest, ParseColorInvalidInput) {
    EXPECT_FALSE(parse_Outter_Color("red").has_value());
    EXPECT_FALSE(parse_Outter_Color("YELLOW").has_value());
    EXPECT_FALSE(parse_Outter_Color("").has_value());
    EXPECT_FALSE(parse_Outter_Color("RED ").has_value());
    EXPECT_FALSE(parse_Outter_Color(" BLUE").has_value());
}

/// Test round-trip of Outter message with an enum field.
TEST(OutterTest, RoundTripViaToml) {
    Outter original;
    original.c = Outter::Color::GREEN;

    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    Outter parsed;
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    EXPECT_EQ(parsed.c, original.c);
}

/// Test required serialization: Outter has no required fields,
/// so serialize_required_toml() should produce an empty table.
TEST(OutterTest, RequiredSerializationEmpty) {
    Outter config;
    config.c = Outter::Color::BLUE;

    auto required_val = config.serialize_toml();
    EXPECT_TRUE(required_val.is_table());
    EXPECT_FALSE(required_val.as_table().empty());
}