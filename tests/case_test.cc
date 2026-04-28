#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "tests/test.tml.h"
#include "tests/config.tml.h"

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
    auto val = shark::parse_str(empty_toml);
    my::custom::ns::Config cfg;
    auto status = cfg.parse_toml(val, {});
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

    std::string toml_str = original.serialize_to_string();
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