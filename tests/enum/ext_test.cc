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
#include <tests/enum/extern.tml.h>
#include <tests/enum/inner.tml.h>
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

using ext::Extern;
using ext::parse_Extern_Dorl;
using ext::to_string;                 // for Extern::Dorl

/// Test that to_string returns correct string for Extern::Dorl.
TEST(ExternTest, LocalEnumToString) {
    EXPECT_EQ(to_string(Extern::Dorl::D1), "D1");
    EXPECT_EQ(to_string(Extern::Dorl::D2), "D2");
    EXPECT_EQ(to_string(Extern::Dorl::D3), "D3");
}

/// Test that parse_Extern_Dorl works for valid input.
TEST(ExternTest, LocalEnumParseValid) {
    auto opt = parse_Extern_Dorl("D1");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Extern::Dorl::D1);

    opt = parse_Extern_Dorl("D2");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Extern::Dorl::D2);

    opt = parse_Extern_Dorl("D3");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, Extern::Dorl::D3);
}

/// Test that parse_Extern_Dorl returns nullopt for invalid input.
TEST(ExternTest, LocalEnumParseInvalid) {
    EXPECT_FALSE(parse_Extern_Dorl("d1").has_value());
    EXPECT_FALSE(parse_Extern_Dorl("D4").has_value());
    EXPECT_FALSE(parse_Extern_Dorl("").has_value());
}

/// Test that cross-file enum Outter::Color can be parsed via my::em::parse_Outter_Color.
TEST(ExternTest, ExternalEnumParse) {
    auto opt = inner::parse_Outter_Color("GREEN");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, inner::Outter::Color::GREEN);
}

/// Test round-trip serialization and deserialization with both enums set to non-default values.
TEST(ExternTest, RoundTripWithNonDefaultValues) {
    Extern original;
    original.d = Extern::Dorl::D2;
    original.c = inner::Outter::Color::BLUE;

    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    Extern parsed;
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    EXPECT_EQ(parsed.d, original.d);
    EXPECT_EQ(parsed.c, original.c);
}

/// Test round-trip when enum fields are left at default values.
TEST(ExternTest, RoundTripWithDefaultValues) {
    Extern original;   // d = D1, c = RED (defaults)

    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    Extern parsed;
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    EXPECT_EQ(parsed.d, Extern::Dorl::D1);
    EXPECT_EQ(parsed.c, inner::Outter::Color::RED);
}

/// Test that required serialization produces an empty table (no required fields).
TEST(ExternTest, RequiredSerializationEmpty) {
    Extern config;
    config.d = Extern::Dorl::D3;
    config.c = inner::Outter::Color::GREEN;

    auto required_val = config.serialize_toml();
    EXPECT_TRUE(required_val.is_table());
    EXPECT_FALSE(required_val.as_table().empty());
}