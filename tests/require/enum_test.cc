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
#include <tests/require/enum_repeat.tml.h>   // generated header for BaseRepeat
#include <tests/require/enum_global.tml.h>   // for global::Errno
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

/// Test conversion functions for local enum RpcErr.
TEST(BaseRepeatTest, LocalEnumRpcErr) {
    // to_string
    EXPECT_EQ(brpc::to_string(brpc::RpcErr::RPC_SUCC), "RPC_SUCC");
    EXPECT_EQ(brpc::to_string(brpc::RpcErr::RPC_INNER), "RPC_INNER");
    EXPECT_EQ(brpc::to_string(brpc::RpcErr::RPC_CONNECT_FAIL), "RPC_CONNECT_FAIL");

    // parse valid
    auto opt = brpc::parse_RpcErr("RPC_SUCC");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::RpcErr::RPC_SUCC);
    opt = brpc::parse_RpcErr("RPC_INNER");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::RpcErr::RPC_INNER);
    opt = brpc::parse_RpcErr("RPC_CONNECT_FAIL");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::RpcErr::RPC_CONNECT_FAIL);

    // parse invalid
    EXPECT_FALSE(brpc::parse_RpcErr("rpc_succ").has_value());
    EXPECT_FALSE(brpc::parse_RpcErr("UNKNOWN").has_value());
    EXPECT_FALSE(brpc::parse_RpcErr("").has_value());
}

/// Test conversion functions for external enum global::Errno.
TEST(BaseRepeatTest, ExternalEnumErrno) {
    // to_string
    EXPECT_EQ(global::to_string(global::Errno::SUCCESS), "SUCCESS");
    EXPECT_EQ(global::to_string(global::Errno::INVALID_ERRNO), "INVALID_ERRNO");
    EXPECT_EQ(global::to_string(global::Errno::NOT_FOUND), "NOT_FOUND");

    // parse valid
    auto opt = global::parse_Errno("SUCCESS");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, global::Errno::SUCCESS);
    opt = global::parse_Errno("INVALID_ERRNO");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, global::Errno::INVALID_ERRNO);
    opt = global::parse_Errno("NOT_FOUND");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, global::Errno::NOT_FOUND);

    // parse invalid
    EXPECT_FALSE(global::parse_Errno("success").has_value());
    EXPECT_FALSE(global::parse_Errno("EINVAL").has_value());
}

/// Test conversion functions for internal enum BaseRepeat::MyFriut.
TEST(BaseRepeatTest, InternalEnumMyFriut) {
    // to_string (uses brpc::to_string overload)
    EXPECT_EQ(brpc::to_string(brpc::BaseRepeat::MyFriut::APPLE), "APPLE");
    EXPECT_EQ(brpc::to_string(brpc::BaseRepeat::MyFriut::PEAR), "PEAR");
    EXPECT_EQ(brpc::to_string(brpc::BaseRepeat::MyFriut::BANANA), "BANANA");

    // parse valid
    auto opt = brpc::parse_BaseRepeat_MyFriut("APPLE");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::BaseRepeat::MyFriut::APPLE);
    opt = brpc::parse_BaseRepeat_MyFriut("PEAR");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::BaseRepeat::MyFriut::PEAR);
    opt = brpc::parse_BaseRepeat_MyFriut("BANANA");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, brpc::BaseRepeat::MyFriut::BANANA);

    // parse invalid
    EXPECT_FALSE(brpc::parse_BaseRepeat_MyFriut("apple").has_value());
    EXPECT_FALSE(brpc::parse_BaseRepeat_MyFriut("ORANGE").has_value());
}

/// Test round-trip serialization for BaseRepeat with all fields set to non-default values.
TEST(BaseRepeatTest, RoundTripAllFieldsNonDefault) {
    brpc::BaseRepeat original;

    // repeated fields
    original.err = {brpc::RpcErr::RPC_INNER, brpc::RpcErr::RPC_CONNECT_FAIL};
    original.required_err = {brpc::RpcErr::RPC_SUCC};
    original.gerr = {global::Errno::NOT_FOUND, global::Errno::INVALID_ERRNO};
    original.required_gerr = {global::Errno::SUCCESS, global::Errno::NOT_FOUND};
    original.fruit = {brpc::BaseRepeat::MyFriut::PEAR, brpc::BaseRepeat::MyFriut::BANANA};
    original.required_fruit = {brpc::BaseRepeat::MyFriut::APPLE};

    // single fields
    original.s_err = brpc::RpcErr::RPC_CONNECT_FAIL;
    original.required_s_err = brpc::RpcErr::RPC_INNER;
    original.s_gerr = global::Errno::INVALID_ERRNO;
    original.required_s_gerr = global::Errno::NOT_FOUND;
    original.s_fruit = brpc::BaseRepeat::MyFriut::BANANA;
    original.required_s_fruit = brpc::BaseRepeat::MyFriut::PEAR;

    // Serialize to TOML.
    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    // Parse back.
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    brpc::BaseRepeat parsed;
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    // Compare repeated fields.
    EXPECT_EQ(parsed.err, original.err);
    EXPECT_EQ(parsed.required_err, original.required_err);
    EXPECT_EQ(parsed.gerr, original.gerr);
    EXPECT_EQ(parsed.required_gerr, original.required_gerr);
    EXPECT_EQ(parsed.fruit, original.fruit);
    EXPECT_EQ(parsed.required_fruit, original.required_fruit);

    // Compare single fields.
    EXPECT_EQ(parsed.s_err, original.s_err);
    EXPECT_EQ(parsed.required_s_err, original.required_s_err);
    EXPECT_EQ(parsed.s_gerr, original.s_gerr);
    EXPECT_EQ(parsed.required_s_gerr, original.required_s_gerr);
    EXPECT_EQ(parsed.s_fruit, original.s_fruit);
    EXPECT_EQ(parsed.required_s_fruit, original.required_s_fruit);
}

/// Test that required fields appear in serialize_required_toml() output.
TEST(BaseRepeatTest, RequiredFieldsAppearInMinimalOutput) {
    brpc::BaseRepeat config;
    // Set required fields only.
    config.required_err = {brpc::RpcErr::RPC_INNER};
    config.required_gerr = {global::Errno::SUCCESS};
    config.required_fruit = {brpc::BaseRepeat::MyFriut::APPLE, brpc::BaseRepeat::MyFriut::PEAR};
    config.required_s_err = brpc::RpcErr::RPC_CONNECT_FAIL;
    config.required_s_gerr = global::Errno::NOT_FOUND;
    config.required_s_fruit = brpc::BaseRepeat::MyFriut::BANANA;

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    ASSERT_TRUE(parsed_val.is_table());
    const auto& table = parsed_val.as_table();

    // Check that required repeated fields exist.
    EXPECT_TRUE(table.count("required_err"));
    EXPECT_TRUE(table.count("required_gerr"));
    EXPECT_TRUE(table.count("required_fruit"));
    // Check that required singular fields exist.
    EXPECT_TRUE(table.count("required_s_err"));
    EXPECT_TRUE(table.count("required_s_gerr"));
    EXPECT_TRUE(table.count("required_s_fruit"));

    // Check that optional fields (even if set) do NOT appear.
    // (We didn't set them, but they could have defaults; in this case they are omitted)
    EXPECT_FALSE(table.count("err"));
    EXPECT_FALSE(table.count("gerr"));
    EXPECT_FALSE(table.count("fruit"));
    EXPECT_FALSE(table.count("s_err"));
    EXPECT_FALSE(table.count("s_gerr"));
    EXPECT_FALSE(table.count("s_fruit"));
}

/// Test that empty repeated required fields are still serialized (if generator emits them).
/// This test is optional; enable if your design requires empty arrays.
TEST(BaseRepeatTest, DISABLED_EmptyRequiredRepeatedFieldsAppear) {
    brpc::BaseRepeat config;   // All required repeated fields are empty.

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    const auto& table = parsed_val.as_table();

    EXPECT_TRUE(table.count("required_err"));
    EXPECT_TRUE(table.at("required_err").is_array());
    EXPECT_TRUE(table.at("required_err").as_array().empty());

    EXPECT_TRUE(table.count("required_gerr"));
    EXPECT_TRUE(table.at("required_gerr").is_array());
    EXPECT_TRUE(table.at("required_gerr").as_array().empty());

    EXPECT_TRUE(table.count("required_fruit"));
    EXPECT_TRUE(table.at("required_fruit").is_array());
    EXPECT_TRUE(table.at("required_fruit").as_array().empty());

    // Also check singular required fields should appear with their default values.
    // But default values may be omitted if generator skips defaults; adjust accordingly.
}