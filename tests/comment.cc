// main.cpp
// Demo for Shark-generated Person class: construction, TOML serialization, and parsing.

#include <iostream>
#include <fstream>
#include <string>

#include "tests/test.tml.h"      // Generated header
#include <xtoml/toml.h>          // shark::format, shark::parse, etc.

int main() {
    // 1. Create and populate a Person object
    my::custom::ns::Person person;

    person.name = "Lothar";                     // required field
    person.age = 42;                            // stored as int8_t
    person.emails = {"alice@example.com", "bob@example.com"};
    person.ages = {25, 30, 35};
    person.favorite_color = my::custom::ns::Color::GREEN;
    person.long_name = "This is a long string";

    // Fill nested Address (optional field, but default-constructed)
    person.address.street = "123 Main St";
    person.address.number = 100;
    person.address.detail.region = "North";
    person.address.detail.prcode = "12345";

    // Fill repeated Address list (address2)
    my::custom::ns::Person::Address addr1;
    addr1.street = "456 Oak Ave";
    addr1.number = 200;
    addr1.detail.region = "South";
    addr1.detail.prcode = "67890";
    person.address2.push_back(addr1);

    my::custom::ns::Person::Address addr2;
    addr2.street = "789 Pine Rd";
    addr2.number = 300;
    addr2.detail.region = "East";
    addr2.detail.prcode = "11223";
    person.address2.push_back(addr2);

    // 2. Serialize to TOML string (using generated serialize_toml + shark::format)
    xtoml::Value toml_value = person.serialize_toml();
    std::string toml_string = xtoml::format(toml_value);   // Format as TOML text

    // 3. Print and optionally save to file
    std::cout << "=== Serialized TOML ===\n" << toml_string << "\n";

    std::ofstream out("person.shark");
    out << toml_string;
    out.close();

    // 4. Parse back from TOML string
    my::custom::ns::Person parsed_person;
    auto status = parsed_person.parse_toml_str(toml_string);   // or parse_toml (takes xtoml::Value)
    if (!status.ok()) {
        std::cerr << "Failed to parse TOML: " << status.message() << "\n";
        return 1;
    }

    // 5. Verify parsed data
    std::cout << "\n=== Parsed Person ===\n"
              << "Name: " << parsed_person.name << "\n"
              << "Age: " << static_cast<int>(parsed_person.age) << "\n"
              << "Emails: ";
    for (const auto& e : parsed_person.emails) std::cout << e << " ";
    std::cout << "\nFavorite color: " << my::custom::ns::to_string(parsed_person.favorite_color) << "\n"
              << "Long name: " << parsed_person.long_name << "\n";


    ////////////////////
    {
        // Root table
        xtoml::Value root = xtoml::Table{};

        // Scalar fields
        root["name"] = "Lothar";
        root["age"] = 42;
        root["long_name"] = "This is a long string";

        // emails array (basic)
        xtoml::Array emails_arr;
        emails_arr.push_back("alice@example.com");
        emails_arr.push_back("bob@example.com");
        root["emails"] = emails_arr;

        // ages array (basic)
        xtoml::Array ages_arr;
        ages_arr.push_back(25);
        ages_arr.push_back(30);
        ages_arr.push_back(35);
        root["ages"] = ages_arr;

        // favorite_color (enum→string)
        root["favorite_color"] = "GREEN";

        // Nested table "address"
        xtoml::Value address = xtoml::Table{};
        address["street"] = "123 Main St";
        address["number"] = 100;
        xtoml::Value detail = xtoml::Table{};
        detail["region"] = "North";
        detail["prcode"] = "12345";
        address["detail"] = detail;
        root["address"] = address;

        // Table array "address2"
        xtoml::Array address2_arr;

        // First element
        xtoml::Value addr1 = xtoml::Table{};
        addr1["street"] = "456 Oak Ave";
        addr1["number"] = 200;
        xtoml::Value detail1 = xtoml::Table{};
        detail1["region"] = "South";
        detail1["prcode"] = "67890";
        addr1["detail"] = detail1;
        address2_arr.push_back(addr1);

        // Second element
        xtoml::Value addr2 = xtoml::Table{};
        addr2["street"] = "789 Pine Rd";
        addr2["number"] = 300;
        xtoml::Value detail2 = xtoml::Table{};
        detail2["region"] = "East";
        detail2["prcode"] = "11223";
        addr2["detail"] = detail2;
        address2_arr.push_back(addr2);

        root["address2"] = address2_arr;

        // Output TOML string
        std::cout << xtoml::format(root) << std::endl;
    }



    return 0;
}