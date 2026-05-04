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
#include <tests/require/msg_base.tml.h>   // base::Bear
#include <tests/require/msg_ext.tml.h>    // msgext::Wine, msgext::Market
#include <xtoml/toml.h>
#include <turbo/utility/status.h>

/// Test Wine message serialization/deserialization.
TEST(MessageTest, WineRoundTrip) {
    msgext::Wine original;
    original.name = "Château Margaux";
    original.city = "Bordeaux";

    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    msgext::Wine parsed;
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    EXPECT_EQ(parsed.name, original.name);
    EXPECT_EQ(parsed.city, original.city);
}

/// Test required fields are serialized in serialize_required_toml.
TEST(MessageTest, WineRequiredSerialization) {
    msgext::Wine config;
    config.name = "Château d'Yquem";
    config.city = "Sauternes";

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    const auto &table = parsed_val.as_table();

    // Only required field "name" should appear.
    EXPECT_TRUE(table.count("name"));
    EXPECT_EQ(table.at("name").as_string(), config.name);
    EXPECT_FALSE(table.count("city"));
}

/// Test Bear message (with nested required message HaRbin).
TEST(MessageTest, BearRoundTrip) {
    base::Bear original;
    original.band = "Grizzly";
    original.price = 1500;
    original.haerbin.data = 42; // nested required field

    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    base::Bear parsed;
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    EXPECT_EQ(parsed.band, original.band);
    EXPECT_EQ(parsed.price, original.price);
    EXPECT_EQ(parsed.haerbin.data, original.haerbin.data);
}

/// Test required propagation in Bear: serialize_required_toml should output
/// required fields including nested required message's required fields.
TEST(MessageTest, BearRequiredSerialization) {
    base::Bear config;
    config.band = "Polar";
    config.price = 2000; // optional
    config.haerbin.data = 100;

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    const auto &table = parsed_val.as_table();

    // required field "band" appears.
    EXPECT_TRUE(table.count("band"));
    EXPECT_EQ(table.at("band").as_string(), config.band);
    // optional field "price" should be absent.
    EXPECT_FALSE(table.count("price"));
    // required nested message "haerbin" appears and within it required "data".
    EXPECT_TRUE(table.count("haerbin"));
    EXPECT_TRUE(table.at("haerbin").is_table());
    const auto &haerbin_tbl = table.at("haerbin").as_table();
    EXPECT_TRUE(haerbin_tbl.count("data"));
    EXPECT_EQ(haerbin_tbl.at("data").as_integer(), config.haerbin.data);
}

/// Test Market message with all field types.
TEST(MessageTest, MarketRoundTripWithAllFields) {
    msgext::Market original;
    original.name = "Farmers Market";

    // bears (repeated, optional)
    base::Bear bear1, bear2;
    bear1.band = "Brown";
    bear1.haerbin.data = 1;
    bear2.band = "Black";
    bear2.haerbin.data = 2;
    original.bears = {bear1, bear2};

    // required_bears (repeated, is_required)
    base::Bear req_bear;
    req_bear.band = "Required";
    req_bear.haerbin.data = 99;
    original.required_bears = {req_bear};

    // wine (optional)
    msgext::Wine w;
    w.name = "Cabernet";
    w.city = "Napa";
    original.wine = w;

    // required_wine (required)
    msgext::Wine rw;
    rw.name = "Merlot";
    original.required_wine = rw;

    // oid (optional nest)
    msgext::Market::Oid o;
    o.size = 100;
    o.data = 3.14;
    original.oid = o;

    // required_oid (required nest)
    msgext::Market::Oid ro;
    ro.size = 200;
    original.required_oid = ro;

    // required_ids (repeated required nest)
    msgext::Market::Oid id1, id2;
    id1.size = 300;
    id2.size = 400;
    id2.data = 5.5;
    original.required_ids = {id1, id2};

    // ids (repeated optional nest)
    msgext::Market::Oid id3, id4;
    id3.size = 500;
    id4.size = 600;
    original.ids = {id3, id4};

    // Serialize and parse back.
    auto toml_val = original.serialize_toml();
    std::string toml_str = xtoml::serialize(toml_val).value_or_die();

    msgext::Market parsed;
    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    auto status = parsed.parse_toml(parsed_val, {}, {});
    ASSERT_TRUE(status.ok()) << status;

    // Compare.
    EXPECT_EQ(parsed.name, original.name);

    EXPECT_EQ(parsed.bears.size(), original.bears.size());
    for (size_t i = 0; i < parsed.bears.size(); ++i) {
        EXPECT_EQ(parsed.bears[i].band, original.bears[i].band);
        EXPECT_EQ(parsed.bears[i].haerbin.data, original.bears[i].haerbin.data);
    }

    EXPECT_EQ(parsed.required_bears.size(), original.required_bears.size());
    for (size_t i = 0; i < parsed.required_bears.size(); ++i) {
        EXPECT_EQ(parsed.required_bears[i].band, original.required_bears[i].band);
        EXPECT_EQ(parsed.required_bears[i].haerbin.data, original.required_bears[i].haerbin.data);
    }

    // Compare.
    EXPECT_EQ(parsed.name, original.name);

    EXPECT_EQ(parsed.bears.size(), original.bears.size());
    for (size_t i = 0; i < parsed.bears.size(); ++i) {
        EXPECT_EQ(parsed.bears[i].band, original.bears[i].band);
        EXPECT_EQ(parsed.bears[i].price, original.bears[i].price);
        EXPECT_EQ(parsed.bears[i].haerbin.data, original.bears[i].haerbin.data);
    }

    EXPECT_EQ(parsed.required_bears.size(), original.required_bears.size());
    for (size_t i = 0; i < parsed.required_bears.size(); ++i) {
        EXPECT_EQ(parsed.required_bears[i].band, original.required_bears[i].band);
        EXPECT_EQ(parsed.required_bears[i].price, original.required_bears[i].price);
        EXPECT_EQ(parsed.required_bears[i].haerbin.data, original.required_bears[i].haerbin.data);
    }

    // wine is optional (but no has_wine method, just compare directly)
    EXPECT_EQ(parsed.wine.name, original.wine.name);
    EXPECT_EQ(parsed.wine.city, original.wine.city);

    // required_wine always present
    EXPECT_EQ(parsed.required_wine.name, original.required_wine.name);

    // oid is optional (similarly compare directly)
    EXPECT_EQ(parsed.oid.size, original.oid.size);
    EXPECT_DOUBLE_EQ(parsed.oid.data, original.oid.data);

    // required_oid always present
    EXPECT_EQ(parsed.required_oid.size, original.required_oid.size);

    // required_ids
    EXPECT_EQ(parsed.required_ids.size(), original.required_ids.size());
    for (size_t i = 0; i < parsed.required_ids.size(); ++i) {
        EXPECT_EQ(parsed.required_ids[i].size, original.required_ids[i].size);
        EXPECT_DOUBLE_EQ(parsed.required_ids[i].data, original.required_ids[i].data);
    }

    // ids
    EXPECT_EQ(parsed.ids.size(), original.ids.size());
    for (size_t i = 0; i < parsed.ids.size(); ++i) {
        EXPECT_EQ(parsed.ids[i].size, original.ids[i].size);
        EXPECT_DOUBLE_EQ(parsed.ids[i].data, original.ids[i].data);
    }
}

/// Test that Market::serialize_required_toml outputs only required-marked fields
/// and propagates required nesting correctly.
TEST(MessageTest, MarketRequiredSerialization) {
    msgext::Market config;
    config.name = "Downtown Market"; // required string

    // required_bears (is_required repeated)
    base::Bear req_bear;
    req_bear.band = "Required Bear";
    req_bear.haerbin.data = 77;
    config.required_bears = {req_bear};

    // required_wine (required message)
    msgext::Wine rw;
    rw.name = "Syrah";
    config.required_wine = rw;

    // required_oid (required nested message)
    msgext::Market::Oid ro;
    ro.size = 999;
    config.required_oid = ro;

    // required_ids (repeated required nested message)
    msgext::Market::Oid rid;
    rid.size = 111;
    config.required_ids = {rid};

    // Optional fields (set but should not appear)
    config.bears = {base::Bear{}}; // will be ignored
    msgext::Wine w;
    w.name = "Chardonnay";
    config.wine = w;
    msgext::Market::Oid o;
    o.size = 123;
    config.oid = o;
    config.ids = {msgext::Market::Oid{}};

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    const auto &table = parsed_val.as_table();

    // Required fields should exist.
    EXPECT_TRUE(table.count("name"));
    EXPECT_TRUE(table.count("required_bears"));
    EXPECT_TRUE(table.count("required_wine"));
    EXPECT_TRUE(table.count("required_oid"));
    EXPECT_TRUE(table.count("required_ids"));

    // Optional fields should NOT exist.
    EXPECT_FALSE(table.count("bears"));
    EXPECT_FALSE(table.count("wine"));
    EXPECT_FALSE(table.count("oid"));
    EXPECT_FALSE(table.count("ids"));

    // Check content of required_bears: should contain required subfields of Bear.
    const auto &bears_arr = table.at("required_bears").as_array();
    ASSERT_EQ(bears_arr.size(), 1);
    const auto &bear_tbl = bears_arr[0].as_table();
    EXPECT_TRUE(bear_tbl.count("band"));
    EXPECT_FALSE(bear_tbl.count("price")); // optional
    EXPECT_TRUE(bear_tbl.count("haerbin"));
    const auto &h_tbl = bear_tbl.at("haerbin").as_table();
    EXPECT_TRUE(h_tbl.count("data"));

    // Check required_wine: should contain only "name" (required) not "city".
    const auto &wine_tbl = table.at("required_wine").as_table();
    EXPECT_TRUE(wine_tbl.count("name"));
    EXPECT_FALSE(wine_tbl.count("city"));

    // Check required_oid: should contain only "size" (required) not "data".
    const auto &oid_tbl = table.at("required_oid").as_table();
    EXPECT_TRUE(oid_tbl.count("size"));
    EXPECT_FALSE(oid_tbl.count("data"));
}

/// Test that when required repeated fields are empty, they are still included
/// (depending on generator design; this test is disabled by default).
TEST(MessageTest, DISABLED_RequiredRepeatedEmptyArrays) {
    msgext::Market config;
    config.name = "Empty Market";
    // required_bears, required_ids are empty vectors.

    auto required_val = config.serialize_required_toml();
    std::string toml_str = xtoml::serialize(required_val).value_or_die();

    auto parsed_val = xtoml::parse_string(toml_str).value_or_die();
    const auto &table = parsed_val.as_table();

    EXPECT_TRUE(table.count("required_bears"));
    EXPECT_TRUE(table.at("required_bears").is_array());
    EXPECT_TRUE(table.at("required_bears").as_array().empty());

    EXPECT_TRUE(table.count("required_ids"));
    EXPECT_TRUE(table.at("required_ids").is_array());
    EXPECT_TRUE(table.at("required_ids").as_array().empty());
}
