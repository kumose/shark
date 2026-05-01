#include <xtoml/toml.h>
#include <iostream>
#include <string>
#include <sstream>

std::string to_toml_string(const xtoml::Value& v, bool in_array_context = false);

std::string format_inline_table(const xtoml::Table& tbl) {
    std::stringstream ss;
    ss << "{ ";
    bool first = true;
    for (const auto& [key, val] : tbl) {
        if (!first) ss << ", ";
        first = false;
        ss << key << " = " << to_toml_string(val, false);
    }
    ss << " }";
    return ss.str();
}

std::string format_standard_table(const xtoml::Table& tbl, const std::string& path_prefix) {
    std::stringstream ss;
    for (const auto& [key, val] : tbl) {
        std::string new_path = path_prefix.empty() ? key : path_prefix + "." + key;
        if (val.is_table()) {
            // Output Table header
            ss << "\n[" << new_path << "]\n";
            ss << to_toml_string(val, false);
        } else if (val.is_array()) {
            ss << key << " = " << to_toml_string(val, false) << "\n";
        } else {
            ss << key << " = " << to_toml_string(val, false) << "\n";
        }
    }
    return ss.str();
}

std::string to_toml_string(const xtoml::Value& v, bool in_array_context) {
    if (v.is_string()) {
        return "\"" + v.as_string() + "\"";
    } else if (v.is_integer()) {
        return std::to_string(v.as_integer());
    } else if (v.is_floating()) {
        return std::to_string(v.is_floating());
    } else if (v.is_boolean()) {
        return v.as_boolean() ? "true" : "false";
    } else if (v.is_array()) {
        const auto& arr = v.as_array();
        std::stringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& elem : arr) {
            if (!first) ss << ", ";
            first = false;
            // For Array elements that are tables, force inline output
            if (elem.is_table()) {
                ss << format_inline_table(elem.as_table());
            } else {
                ss << to_toml_string(elem, true);
            }
        }
        ss << "]";
        return ss.str();
    } else if (v.is_table()) {
        if (in_array_context) {
            // Inline Table inside an Array
            return format_inline_table(v.as_table());
        } else {
            // Standard multi-line Table
            return format_standard_table(v.as_table(), "");
        }
    }
    return "";
}

int main() {
    // Build the data structure using Shark's API
    xtoml::Table root;
    root["name"] = "Lothar";
    root["age"] = 42;
    root["long_name"] = "This is a long string";

    xtoml::Array emails;
    emails.push_back("alice@example.com");
    emails.push_back("bob@example.com");
    root["emails"] = std::move(emails);

    xtoml::Array ages;
    ages.push_back(25);
    ages.push_back(30);
    ages.push_back(35);
    root["ages"] = std::move(ages);

    root["favorite_color"] = "GREEN";

    xtoml::Table address;
    address["street"] = "123 Main St";
    address["number"] = 100;
    xtoml::Table detail;
    detail["region"] = "North";
    detail["prcode"] = "12345";
    address["detail"] = std::move(detail);
    root["address"] = std::move(address);

    xtoml::Array address2_arr;
    xtoml::Table addr1;
    addr1["street"] = "456 Oak Ave";
    addr1["number"] = 200;
    xtoml::Table detail1;
    detail1["region"] = "South";
    detail1["prcode"] = "67890";
    addr1["detail"] = std::move(detail1);
    address2_arr.push_back(std::move(addr1));

    xtoml::Table addr2;
    addr2["street"] = "789 Pine Rd";
    addr2["number"] = 300;
    xtoml::Table detail2;
    detail2["region"] = "East";
    detail2["prcode"] = "11223";
    addr2["detail"] = std::move(detail2);
    address2_arr.push_back(std::move(addr2));

    root["address2"] = std::move(address2_arr);

    // Output with custom formatter
    std::cout << to_toml_string(root, false) << std::endl;

    return 0;
}
