#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "tests/test.tml.h"
#include "tests/config.tml.h"
#include  <turbo/log/logging.h>

namespace fs = std::filesystem;

class PersonTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_dir_ = fs::temp_directory_path() / "shark_test";
        fs::create_directories(temp_dir_);
    }

    void TearDown() override {
        fs::remove_all(temp_dir_);
    }

    std::string createTempFile(const std::string& content) {
        auto path = temp_dir_ / "test.toml";
        std::ofstream ofs(path);
        ofs << content;
        ofs.close();
        return path.string();
    }

    fs::path temp_dir_;
};

TEST(EmptyFileTest, ParsesSuccessfullyWithDefaults) {
    auto empty_toml = "";
    auto val = xtoml::parse_str(empty_toml);
    my::custom::ns::Config cfg;
    auto status = cfg.parse_toml(val, {}, {});
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(cfg.name, "default_name");
    EXPECT_EQ(cfg.age, 0);
    EXPECT_EQ(cfg.enabled, true);
}

TEST_F(PersonTest, RequiredFieldOnly) {
    std::string toml = "name = \"Alice\"";
    auto path = createTempFile(toml);
    my::custom::ns::Person person;
    auto status = person.parse_toml_file(path);
    ASSERT_TRUE(status.ok()) << status.message();

    EXPECT_EQ(person.name, "Alice");
    EXPECT_EQ(person.age, 0);
    EXPECT_TRUE(person.emails.empty());
    EXPECT_EQ(person.favorite_color, my::custom::ns::Color::GREEN);
}

TEST_F(PersonTest, MissingRequiredField) {
    std::string toml = "age = 30";
    auto path = createTempFile(toml);
    my::custom::ns::Person person;
    auto status = person.parse_toml_file(path);
    EXPECT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("name") != std::string::npos);
}

TEST_F(PersonTest, FullValidConfig) {
    std::string toml = R"(
name = "Bob"
age = 42
emails = ["bob@example.com", "bob2@example.com"]
ages = [25, 30, 35]
favorite_color = "GREEN"


address2 = [
    { street = "456 Oak Ave", number = 200, detail = { region = "South", prcode = "67890" } },
    { street = "789 Pine Rd", number = 300, detail = { region = "East", prcode = "11223" } }
]

long_name = "Test long name"

[address]
street = "123 Main St"
number = 100


[address.detail]
region = "North"
prcode = "12345"

[ad]
street = "123 Main St"
number = 100


[ad.detail]
region = "North"
prcode = "12345"


######################


)";

    auto path = createTempFile(toml);
    my::custom::ns::Person person;
    auto status = person.parse_toml_str(toml);
    KLOG(INFO)<<status;
    ASSERT_TRUE(status.ok()) << status.message();
    std::cout<<person.serialize_to_string()<<std::endl;
    EXPECT_EQ(person.name, "Bob");
    EXPECT_EQ(person.age, 42);
    EXPECT_EQ(person.emails.size(), 2);
    EXPECT_EQ(person.emails[0], "bob@example.com");
    EXPECT_EQ(person.ages.size(), 3);
    EXPECT_EQ(person.ages[1], 30);
    EXPECT_EQ(person.favorite_color, my::custom::ns::Color::GREEN);
    EXPECT_EQ(person.address.street, "123 Main St");
    EXPECT_EQ(person.address.number, 100);
    EXPECT_EQ(person.address.detail.region, "North");
    EXPECT_EQ(person.address.detail.prcode, "12345");
    ASSERT_EQ(person.address2.size(), 2);
    EXPECT_EQ(person.address2[0].street, "456 Oak Ave");
    EXPECT_EQ(person.address2[0].detail.region, "South");
    EXPECT_EQ(person.address2[1].street, "789 Pine Rd");
    EXPECT_EQ(person.address2[1].detail.prcode, "11223");
    EXPECT_EQ(person.long_name, "Test long name");
}

TEST_F(PersonTest, RoundTrip) {
    my::custom::ns::Person original;
    original.name = "Charlie";
    original.age = 99;
    original.emails = {"charlie@example.com"};
    original.ages = {11, 22};
    original.favorite_color = my::custom::ns::Color::BLUE;
    original.address.street = "Round St";
    original.address.number = 777;
    original.address.detail.region = "West";
    original.address.detail.prcode = "99999";
    my::custom::ns::Person::Address addr;
    addr.street = "Trip Ave";
    addr.number = 888;
    addr.detail.region = "East";
    addr.detail.prcode = "00000";
    original.address2.push_back(addr);
    original.long_name = "Round trip test";
    auto rs = original.serialize_to_string();
    std::string toml_str = std::move(rs).value_or_die();
    std::cout << std::string('#', 80) << std::endl;
    std::cout << toml_str<<std::endl;
    std::cout << std::string('#', 80) << std::endl;
    auto path = createTempFile(toml_str);
    my::custom::ns::Person parsed;
    auto status = parsed.parse_toml_file(path);
    ASSERT_TRUE(status.ok()) << status.message();

    EXPECT_EQ(parsed.name, original.name);
    EXPECT_EQ(parsed.age, original.age);
    EXPECT_EQ(parsed.emails, original.emails);
    EXPECT_EQ(parsed.ages, original.ages);
    EXPECT_EQ(parsed.favorite_color, original.favorite_color);
    EXPECT_EQ(parsed.address.street, original.address.street);
    EXPECT_EQ(parsed.address.number, original.address.number);
    EXPECT_EQ(parsed.address.detail.region, original.address.detail.region);
    EXPECT_EQ(parsed.address.detail.prcode, original.address.detail.prcode);
    ASSERT_EQ(parsed.address2.size(), 1);
    EXPECT_EQ(parsed.address2[0].street, addr.street);
    EXPECT_EQ(parsed.address2[0].detail.region, addr.detail.region);
    EXPECT_EQ(parsed.long_name, original.long_name);
}


// =============================================================================
// Checker tests for Person configuration
// =============================================================================

/// Test RangeHandler on numeric field (age) with valid value_or_die.
TEST_F(PersonTest, Checker_RangeHandler_Age_Valid) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("age").value_or_die();
    auto handler = xtoml::RangeHandler<int8_t, false, false>::create(
        uri, &person.age, false, 0, 18, 60);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Alice"
        age = 30
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.age, 30);
}

/// Test RangeHandler rejects value_or_die below lower bound.
TEST_F(PersonTest, Checker_RangeHandler_Age_TooLow) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("age").value_or_die();
    auto handler = xtoml::RangeHandler<int8_t, false, false>::create(
        uri, &person.age, false, 0, 18, 60);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Bob"
        age = 10
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find(">= 18") != std::string::npos)<<status.message();
}

/// Test RangeHandler rejects value_or_die above upper bound.
TEST_F(PersonTest, Checker_RangeHandler_Age_TooHigh) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("age").value_or_die();
    auto handler = xtoml::RangeHandler<int8_t, false, false>::create(
        uri, &person.age, false, 0, 18, 60);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Carol"
        age = 70
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("<= 60") != std::string::npos);
}
/*
/// Test InSetHandler (whitelist) on enum field favorite_color.
TEST_F(PersonTest, Checker_InSetHandler_Color_Allowed) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("favorite_color").value_or_die();
    std::unordered_set<my::custom::ns::Color> allowed = {
        my::custom::ns::Color::RED,
        my::custom::ns::Color::BLUE
    };
    auto handler = xtoml::InSetHandler<my::custom::ns::Color, true>::create(
        uri, &person.favorite_color, false, my::custom::ns::Color::GREEN, allowed);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Alice"
        favorite_color = "RED"
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.favorite_color, my::custom::ns::Color::RED);
}
*/
/*
/// Test InSetHandler rejects a color not in whitelist.
TEST_F(PersonTest, Checker_InSetHandler_Color_Disallowed) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("favorite_color").value_or_die();
    std::unordered_set<my::custom::ns::Color> allowed = {my::custom::ns::Color::RED, my::custom::ns::Color::BLUE};
    auto handler = xtoml::InSetHandler<my::custom::ns::Color, true>::create(
        uri, &person.favorite_color, false, my::custom::ns::Color::GREEN, allowed);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Bob"
        favorite_color = "GREEN"
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("not in allowed set") != std::string::npos);
}
*/
/// Test CustomHandler on sub-message Address to enforce non-empty street.
///
/*
TEST_F(PersonTest, Checker_CustomHandler_Address_StreetNotEmpty) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("address").value_or_die();
    auto custom_validator = [](const my::custom::ns::Person::Address& addr,
                               const my::custom::ns::Person::Address*) -> turbo::Status {
        if (addr.street.empty()) {
            return turbo::out_of_range_error("street cannot be empty");
        }
        return turbo::OkStatus();
    };
    auto handler = xtoml::CustomHandler<my::custom::ns::Person::Address>::create(
        uri, &person.address, false, my::custom::ns::Person::Address{}, custom_validator);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Alice"
        [address]
        street = "Main St"
        number = 10
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.address.street, "Main St");
}

/// Test CustomHandler rejects address with empty street.
TEST_F(PersonTest, Checker_CustomHandler_Address_StreetEmpty) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("address").value_or_die();
    auto custom_validator = [](const my::custom::ns::Person::Address& addr,
                               const my::custom::ns::Person::Address*) -> turbo::Status {
        if (addr.street.empty()) {
            return turbo::out_of_range_error("street cannot be empty");
        }
        return turbo::OkStatus();
    };
    auto handler = xtoml::CustomHandler<my::custom::ns::Person::Address>::create(
        uri, &person.address, false, my::custom::ns::Person::Address{}, custom_validator);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Bob"
        [address]
        street = ""
        number = 10
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("street cannot be empty") != std::string::npos);
}
*/
/// Test RepeatedHandler on array 'ages' with element range [0,120] and container non-empty.
TEST_F(PersonTest, Checker_RepeatedHandler_Ages_ElementValid) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("ages").value_or_die();
    auto elem_handler = xtoml::RangeHandler<int32_t, false, false>::create(
        uri, nullptr, false, 0, 0, 120);
    auto container_validator = [](const std::vector<int32_t>& vec) -> turbo::Status {
        if (vec.empty()) return turbo::out_of_range_error("ages cannot be empty");
        return turbo::OkStatus();
    };
    auto repeated_handler = xtoml::RepeatedHandler<std::vector<int32_t>>::create(
        uri, &person.ages, false, std::vector<int32_t>{},
        std::move(elem_handler), container_validator);
    person.add_checker(std::move(repeated_handler));

    std::string toml = R"(
        name = "Alice"
        ages = [25, 30, 45]
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.ages.size(), 3);
}

/// Test RepeatedHandler rejects element out of range and includes index in error.
TEST_F(PersonTest, Checker_RepeatedHandler_Ages_ElementOutOfRange) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("ages").value_or_die();
    auto elem_handler = xtoml::RangeHandler<int32_t, false, false>::create(
        uri, nullptr, false, 0, 0, 120);
    auto container_validator = [](const std::vector<int32_t>&) { return turbo::OkStatus(); };
    auto repeated_handler = xtoml::RepeatedHandler<std::vector<int32_t>>::create(
        uri, &person.ages, false, std::vector<int32_t>{},
        std::move(elem_handler), container_validator);
    person.add_checker(std::move(repeated_handler));

    std::string toml = R"(
        name = "Bob"
        ages = [25, 999, 45]
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("ages[1]") != std::string::npos);
    EXPECT_TRUE(status.message().find("<= 120") != std::string::npos);
}

/// Test RepeatedHandler rejects empty array when container validator requires non-empty.
TEST_F(PersonTest, Checker_RepeatedHandler_Ages_EmptyContainer) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("ages").value_or_die();
    auto elem_handler = xtoml::RangeHandler<int32_t, false, false>::create(
        uri, nullptr, false, 0, 0, 120);
    auto container_validator = [](const std::vector<int32_t>& vec) -> turbo::Status {
        if (vec.empty()) return turbo::out_of_range_error("ages cannot be empty");
        return turbo::OkStatus();
    };
    auto repeated_handler = xtoml::RepeatedHandler<std::vector<int32_t>>::create(
        uri, &person.ages, false, std::vector<int32_t>{},
        std::move(elem_handler), container_validator);
    person.add_checker(std::move(repeated_handler));

    std::string toml = R"(
        name = "Carol"
        ages = []
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("cannot be empty") != std::string::npos);
}

/// Test StringHandler with length constraint on 'long_name'.
TEST_F(PersonTest, Checker_StringHandler_LengthRule) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("long_name").value_or_die();
    auto handler = xtoml::StringHandler<>::length_between(uri, &person.long_name, false, "", 3, 10);
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Alice"
        long_name = "ValidName"
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.long_name, "ValidName");

    toml = R"(
        name = "Bob"
        long_name = "x"
    )";
    status = person.parse_toml_str(toml);
    ASSERT_FALSE(status.ok());
    EXPECT_TRUE(status.message().find("length") != std::string::npos);
}

/// Test read-only field: after first assignment, changes are rejected.
TEST_F(PersonTest, Checker_ReadOnlyField) {
    my::custom::ns::Person person;
    // Mark name as read-only
    auto uri = xtoml::TomlUri::create("name").value_or_die();
    auto handler = xtoml::AllPassHandler<std::string>::create(uri, &person.name, true, "");
    person.add_checker(std::move(handler));

    std::string toml = R"( name = "Initial" )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.name, "Initial");

    // Second parse with different name should fail because read-only and version > 1
    toml = R"( name = "Changed" )";
    status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_TRUE(status.message().find("read-only") == std::string::npos);
}

/// Test that AllPassHandler never rejects a value_or_die.
TEST_F(PersonTest, Checker_AllPassHandler_AlwaysPass) {
    my::custom::ns::Person person;
    auto uri = xtoml::TomlUri::create("long_name").value_or_die();
    auto handler = xtoml::AllPassHandler<std::string>::create(uri, &person.long_name, false, "");
    person.add_checker(std::move(handler));

    std::string toml = R"(
        name = "Alice"
        long_name = "anything_goes_!@#$"
    )";
    auto status = person.parse_toml_str(toml);
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(person.long_name, "anything_goes_!@#$");
}