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

#include <shark/toml.h>
#include <iostream>
#include <sstream>

int main() {
    // Parse TOML content
    const std::string toml_content = R"(
# This comment will be changed
[server]
host = "127.0.0.1"  # inline comment
port = 8080
)";

    auto config = xconfig::parse_str(toml_content);
    
    // 1. Access the "server" table
    auto& server = xconfig::find<xconfig::value>(config, "server");
    
    // 2. Read existing comments on the table
    std::cout << "Original table comments:\n";
    for (const auto& c : server.comments()) {
        std::cout << c << '\n';
    }
    
    // 3. **Modify the table's comment** (replace with new ones)
    server.comments().clear();
    server.comments().push_back("# Modified table comment");
    server.comments().push_back("# Second line");
    
    // 4. Modify a field's inline comment by accessing the field value
    auto& port = xconfig::find<xconfig::value>(server, "port");
    port.comments().clear();
    port.comments().push_back(" # Modified inline comment for port");
    
    // 5. Output modified TOML
    std::cout << "\n--- Modified TOML ---\n";
    std::cout << xconfig::format(config);
    
    return 0;
}