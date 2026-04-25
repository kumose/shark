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
#include "test.sk.h"          // Generated classes: Person, Address, Detail, enums
#include <google/protobuf/any.pb.h>
#include <google/protobuf/text_format.h>
#include <turbo/container/flat_hash_map.h>

namespace my::custom::ns {
namespace {

// -------------------------------------------------------------------
// Helper: create a Person object fully populated with test data (includes Any fields).
// -------------------------------------------------------------------
Person CreateFullPerson() {
    Person p;
    p.set_any_one({"type.googleapis.com/example.Type", "value123"});
    auto& any_two = p.mutable_any_two();
    any_two.push_back({"type.googleapis.com/foo", "foo_data"});
    any_two.push_back({"type.googleapis.com/bar", "bar_data"});
    p.set_name("Alice");
    p.age(42);
    p.mutable_emails().push_back("alice@example.com");
    p.mutable_emails().push_back("alice@work.com");
    p.mutable_ages().push_back(30);
    p.mutable_ages().push_back(35);
    p.mutable_ages().push_back(40);
    absl::flat_hash_map<std::string, int32_t> scores;
    scores["math"] = 95;
    scores["physics"] = 88;
    p.set_scores(scores);
    p.set_favorite_color(Color::BLUE);
    Person::Address addr;
    addr.set_street("Main St");
    addr.number(100);
    Person::Address::Detail detail;
    detail.set_region("North");
    detail.set_prcode("12345");
    addr.set_detail(detail);
    p.set_address(std::move(addr));
    Person::Address addr2_1, addr2_2;
    addr2_1.set_street("Second St");
    addr2_1.number(200);
    addr2_2.set_street("Third Ave");
    addr2_2.number(300);
    p.mutable_address2().push_back(addr2_1);
    p.mutable_address2().push_back(std::move(addr2_2));
    p.set_long_name("Alice Johnson Wonderland");
    p.set_aaa("test_aaa_string");
    return p;
}

// -------------------------------------------------------------------
// Helper: create a Person object without Any fields (suitable for JSON tests).
// -------------------------------------------------------------------
Person CreatePersonWithoutAny() {
    Person p;
    p.set_name("Alice");
    p.age(42);
    p.mutable_emails().push_back("alice@example.com");
    p.mutable_emails().push_back("alice@work.com");
    p.mutable_ages().push_back(30);
    p.mutable_ages().push_back(35);
    p.mutable_ages().push_back(40);
    absl::flat_hash_map<std::string, int32_t> scores;
    scores["math"] = 95;
    scores["physics"] = 88;
    p.set_scores(scores);
    p.set_favorite_color(Color::BLUE);
    Person::Address addr;
    addr.set_street("Main St");
    addr.number(100);
    Person::Address::Detail detail;
    detail.set_region("North");
    detail.set_prcode("12345");
    addr.set_detail(detail);
    p.set_address(std::move(addr));
    Person::Address addr2_1, addr2_2;
    addr2_1.set_street("Second St");
    addr2_1.number(200);
    addr2_2.set_street("Third Ave");
    addr2_2.number(300);
    p.mutable_address2().push_back(addr2_1);
    p.mutable_address2().push_back(std::move(addr2_2));
    p.set_long_name("Alice Johnson Wonderland");
    p.set_aaa("test_aaa_string");
    return p;
}

// Helper: compare two Person objects for exact equality.
bool PersonsEqual(const Person& a, const Person& b) {
    if (a.any_one() != b.any_one()) return false;
    if (a.any_two() != b.any_two()) return false;
    if (a.name() != b.name()) return false;
    if (a.age() != b.age()) return false;
    if (a.emails() != b.emails()) return false;
    if (a.ages() != b.ages()) return false;
    if (a.scores() != b.scores()) return false;
    if (a.favorite_color() != b.favorite_color()) return false;
    if (a.long_name() != b.long_name()) return false;

    if (a.address().street() != b.address().street()) return false;
    if (a.address().number() != b.address().number()) return false;
    if (a.address().detail().region() != b.address().detail().region()) return false;
    if (a.address().detail().prcode() != b.address().detail().prcode()) return false;

    if (a.address2().size() != b.address2().size()) return false;
    for (size_t i = 0; i < a.address2().size(); ++i) {
        if (a.address2()[i].street() != b.address2()[i].street()) return false;
        if (a.address2()[i].number() != b.address2()[i].number()) return false;
        if (a.address2()[i].detail().region() != b.address2()[i].detail().region()) return false;
        if (a.address2()[i].detail().prcode() != b.address2()[i].detail().prcode()) return false;
    }

    if (a.kind_case() != b.kind_case()) return false;
    switch (a.kind_case()) {
        case Person::kindCase::AAA:
            if (a.aaa() != b.aaa()) return false;
            break;
        case Person::kindCase::BBB:
            if (a.bbb() != b.bbb()) return false;
            break;
        case Person::kindCase::DDD:
            if (a.ddd().street() != b.ddd().street()) return false;
            if (a.ddd().number() != b.ddd().number()) return false;
            if (a.ddd().detail().region() != b.ddd().detail().region()) return false;
            if (a.ddd().detail().prcode() != b.ddd().detail().prcode()) return false;
            break;
        default: break;
    }
    return true;
}

// -------------------------------------------------------------------
// Default construction and basic getters/setters.
// -------------------------------------------------------------------
TEST(PersonTest, DefaultConstruction) {
    Person p;
    EXPECT_EQ(p.name(), "Lothar");
    EXPECT_EQ(p.age(), 0);
    EXPECT_EQ(p.favorite_color(), Color::GREEN);
    EXPECT_EQ(p.long_name(), "");
    EXPECT_EQ(p.address().street(), "");
    EXPECT_EQ(p.address().number(), 20);
    EXPECT_EQ(p.address().detail().region(), "");
    EXPECT_EQ(p.address().detail().prcode(), "");
    EXPECT_TRUE(p.emails().empty());
    EXPECT_TRUE(p.ages().empty());
    EXPECT_TRUE(p.scores().empty());
    EXPECT_TRUE(p.address2().empty());
    EXPECT_EQ(p.kind_case(), Person::kindCase::NONE);
    EXPECT_FALSE(p.has_kind());
}

TEST(PersonTest, SetAndGetPrimitives) {
    Person p;
    p.set_name("Bob");
    p.age(25);
    p.set_favorite_color(Color::RED);
    p.set_long_name("Robert Johnson");
    EXPECT_EQ(p.name(), "Bob");
    EXPECT_EQ(p.age(), 25);
    EXPECT_EQ(p.favorite_color(), Color::RED);
    EXPECT_EQ(p.long_name(), "Robert Johnson");
}

TEST(PersonTest, AnyOneAndAnyTwo) {
    Person p;
    std::pair<std::string, std::string> any_val = {"type/foo", "binary_data"};
    p.set_any_one(any_val);
    EXPECT_EQ(p.any_one(), any_val);
    auto& vec = p.mutable_any_two();
    vec.push_back({"t1", "d1"});
    vec.push_back({"t2", "d2"});
    ASSERT_EQ(p.any_two().size(), 2);
    EXPECT_EQ(p.any_two()[0].first, "t1");
    EXPECT_EQ(p.any_two()[1].second, "d2");
}

TEST(PersonTest, RepeatedFields) {
    Person p;
    auto& emails = p.mutable_emails();
    emails.push_back("a@b.com");
    emails.push_back("c@d.com");
    ASSERT_EQ(p.emails().size(), 2);
    EXPECT_EQ(p.emails()[0], "a@b.com");
    auto& ages = p.mutable_ages();
    ages.push_back(10);
    ages.push_back(20);
    ASSERT_EQ(p.ages().size(), 2);
    EXPECT_EQ(p.ages()[1], 20);
}

TEST(PersonTest, MapFieldScores) {
    Person p;
    absl::flat_hash_map<std::string, int32_t> scores;
    scores["math"] = 100;
    scores["eng"] = 90;
    p.set_scores(scores);
    EXPECT_EQ(p.scores().size(), 2);
    EXPECT_EQ(p.scores().at("math"), 100);
    EXPECT_EQ(p.scores().at("eng"), 90);
}

TEST(PersonTest, AddressAndDetail) {
    Person p;
    Person::Address addr;
    addr.set_street("Elm St");
    addr.number(42);
    Person::Address::Detail det;
    det.set_region("West");
    det.set_prcode("67890");
    addr.set_detail(det);
    p.set_address(addr);
    EXPECT_EQ(p.address().street(), "Elm St");
    EXPECT_EQ(p.address().number(), 42);
    EXPECT_EQ(p.address().detail().region(), "West");
    EXPECT_EQ(p.address().detail().prcode(), "67890");
    auto& mutable_det = const_cast<Person::Address::Detail&>(p.address().detail());
    mutable_det.set_region("East");
    EXPECT_EQ(p.address().detail().region(), "East");
}

TEST(PersonTest, RepeatedAddress2) {
    Person p;
    Person::Address a1, a2;
    a1.set_street("First");
    a1.number(1);
    a2.set_street("Second");
    a2.number(2);
    p.mutable_address2().push_back(a1);
    p.mutable_address2().push_back(std::move(a2));
    ASSERT_EQ(p.address2().size(), 2);
    EXPECT_EQ(p.address2()[0].street(), "First");
    EXPECT_EQ(p.address2()[1].number(), 2);
}

// -------------------------------------------------------------------
// Oneof tests.
// -------------------------------------------------------------------
TEST(PersonTest, OneofKindAaa) {
    Person p;
    EXPECT_FALSE(p.has_kind());
    p.set_aaa("hello");
    EXPECT_TRUE(p.has_kind());
    EXPECT_EQ(p.kind_case(), Person::kindCase::AAA);
    EXPECT_TRUE(p.has_aaa());
    EXPECT_EQ(p.aaa(), "hello");
    EXPECT_FALSE(p.has_bbb());
    EXPECT_FALSE(p.has_ddd());
    p.clear_kind();
    EXPECT_FALSE(p.has_kind());
    EXPECT_EQ(p.kind_case(), Person::kindCase::NONE);
}

TEST(PersonTest, OneofKindBbb) {
    Person p;
    p.set_bbb(12345u);
    EXPECT_EQ(p.kind_case(), Person::kindCase::BBB);
    EXPECT_TRUE(p.has_bbb());
    EXPECT_EQ(p.bbb(), 12345u);
}

TEST(PersonTest, OneofKindDdd) {
    Person p;
    Person::Address addr;
    addr.set_street("Oneof Street");
    addr.number(999);
    p.set_ddd(std::move(addr));
    EXPECT_EQ(p.kind_case(), Person::kindCase::DDD);
    EXPECT_TRUE(p.has_ddd());
    EXPECT_EQ(p.ddd().street(), "Oneof Street");
    EXPECT_EQ(p.ddd().number(), 999);
}

TEST(PersonTest, OneofOverwrite) {
    Person p;
    p.set_aaa("first");
    EXPECT_EQ(p.aaa(), "first");
    p.set_bbb(42u);
    EXPECT_EQ(p.kind_case(), Person::kindCase::BBB);
    EXPECT_EQ(p.bbb(), 42u);
    EXPECT_FALSE(p.has_aaa());
}

// -------------------------------------------------------------------
// Enum parsing and to_string.
// -------------------------------------------------------------------
TEST(EnumTest, ColorToString) {
    EXPECT_EQ(to_string(Color::RED), "RED");
    EXPECT_EQ(to_string(Color::GREEN), "GREEN");
    EXPECT_EQ(to_string(Color::BLUE), "BLUE");
}

TEST(EnumTest, ParseColor) {
    auto c = parse_Color("GREEN");
    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(*c, Color::GREEN);
    c = parse_Color("YELLOW");
    EXPECT_FALSE(c.has_value());
}

TEST(EnumTest, ColorU16AndU32) {
    EXPECT_EQ(to_string(ColorU16::RED16), "RED16");
    EXPECT_EQ(to_string(ColorU32::BLUE32), "BLUE32");
}

TEST(EnumTest, PersonDolor) {
    EXPECT_EQ(Person::to_string(Person::Dolor::DRED), "DRED");
    auto d = Person::parse_Dolor("DGREEN");
    ASSERT_TRUE(d.has_value());
    EXPECT_EQ(*d, Person::Dolor::DGREEN);
}

// -------------------------------------------------------------------
// Copy and move semantics (using full Person with Any fields).
// -------------------------------------------------------------------
TEST(PersonTest, CopyConstructor) {
    Person original = CreateFullPerson();
    Person copy(original);
    EXPECT_TRUE(PersonsEqual(original, copy));
}

TEST(PersonTest, CopyAssignment) {
    Person original = CreateFullPerson();
    Person copy;
    copy = original;
    EXPECT_TRUE(PersonsEqual(original, copy));
}

TEST(PersonTest, MoveConstructor) {
    Person original = CreateFullPerson();
    Person moved(std::move(original));
    Person expected = CreateFullPerson();
    EXPECT_TRUE(PersonsEqual(moved, expected));
}

TEST(PersonTest, MoveAssignment) {
    Person original = CreateFullPerson();
    Person moved;
    moved = std::move(original);
    Person expected = CreateFullPerson();
    EXPECT_TRUE(PersonsEqual(moved, expected));
}

// -------------------------------------------------------------------
// Binary Protobuf serialization (works with Any fields).
// -------------------------------------------------------------------
TEST(PersonTest, SerializeToProtobufAndBack) {
    Person original = CreateFullPerson();
    test::pb::pa::Person pb;
    original.serialize_to_proto(pb);
    Person deserialized;
    deserialized.parse_from_proto(pb);
    EXPECT_TRUE(PersonsEqual(original, deserialized));
}

TEST(PersonTest, ProtobufRoundTripWithEmpty) {
    Person original;
    test::pb::pa::Person pb;
    original.serialize_to_proto(pb);
    Person deserialized;
    deserialized.parse_from_proto(pb);
    EXPECT_TRUE(PersonsEqual(original, deserialized));
}

// -------------------------------------------------------------------
// JSON serialization (using Person without Any fields to avoid type registration issues).
// -------------------------------------------------------------------
TEST(PersonTest, JsonRoundTrip) {
    Person original = CreatePersonWithoutAny();  // No Any fields
    std::string json;
    auto rs = original.serialize_to_json(json);
    ASSERT_TRUE(rs.ok()) << "serialize_to_json failed: " << rs.message();
    EXPECT_FALSE(json.empty());

    Person deserialized;
    rs = deserialized.parse_from_json(json);
    ASSERT_TRUE(rs.ok()) << "parse_from_json failed: " << rs.message();
    EXPECT_TRUE(PersonsEqual(original, deserialized));
}

TEST(PersonTest, JsonRoundTripEmpty) {
    Person original;
    std::string json;
    auto rs = original.serialize_to_json(json);
    ASSERT_TRUE(rs.ok()) << rs.message();
    Person deserialized;
    rs = deserialized.parse_from_json(json);
    ASSERT_TRUE(rs.ok()) << rs.message();
    EXPECT_TRUE(PersonsEqual(original, deserialized));
}

TEST(PersonTest, ToStringMethod) {
    Person p = CreatePersonWithoutAny();
    std::string str = p.to_string();
    EXPECT_FALSE(str.empty());
    Person p2;
    auto rs = p2.parse_from_json(str);
    ASSERT_TRUE(rs.ok()) << "parse_from_json failed: " << rs.message();
    EXPECT_TRUE(PersonsEqual(p, p2));
}

// -------------------------------------------------------------------
// Edge cases.
// -------------------------------------------------------------------
TEST(PersonTest, ClearKindOnOneof) {
    Person p;
    p.set_aaa("test");
    EXPECT_TRUE(p.has_kind());
    p.clear_kind();
    EXPECT_FALSE(p.has_kind());
    EXPECT_EQ(p.kind_case(), Person::kindCase::NONE);
    p.set_bbb(100);
    EXPECT_EQ(p.bbb(), 100);
}

TEST(PersonTest, LargeRepeatedFields) {
    Person p;
    const size_t kLarge = 1000;
    for (size_t i = 0; i < kLarge; ++i) {
        p.mutable_emails().push_back("user" + std::to_string(i) + "@example.com");
        p.mutable_ages().push_back(static_cast<int32_t>(i));
    }
    ASSERT_EQ(p.emails().size(), kLarge);
    ASSERT_EQ(p.ages().size(), kLarge);
    EXPECT_EQ(p.emails()[0], "user0@example.com");
    EXPECT_EQ(p.ages()[kLarge - 1], static_cast<int32_t>(kLarge - 1));
}

TEST(PersonTest, MapFieldWithManyEntries) {
    Person p;
    absl::flat_hash_map<std::string, int32_t> scores;
    for (int i = 0; i < 500; ++i) {
        scores["key_" + std::to_string(i)] = i;
    }
    p.set_scores(scores);
    EXPECT_EQ(p.scores().size(), 500);
    EXPECT_EQ(p.scores().at("key_123"), 123);
}

}  // namespace
}  // namespace my::custom::ns