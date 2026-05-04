// demo_gateway.cc
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

#include <iostream>
#include <turbo/utility/status.h>
#include "example.tml.h"

using demo::gateway::GatewayConfig;
using demo::gateway::LogLevel;
using demo::gateway::Backend;
using demo::gateway::Route;

/// Parses a raw TOML string into a GatewayConfig object.
/// @param toml_content The TOML configuration string.
/// @param config Output configuration object.
/// @return Status indicating success or failure.
turbo::Status parse_config(const std::string& toml_content, GatewayConfig& config) {
    return config.parse_toml_str(toml_content);
}

/// Displays the configuration content to stdout.
/// @param config The configuration to display.
void show_config(const GatewayConfig& config) {
    // Required fields
    std::cout << "Service name: " << config.service_name << std::endl;
    std::cout << "Listen port: " << config.listen_port << std::endl;

    // Optional fields with defaults
    std::cout << "Log level: " << demo::gateway::to_string(config.log_level) << std::endl;
    std::cout << "Worker threads: " << config.worker_threads << std::endl;

    // Optional nested TLS (detect via cert_file non‑empty)
    if (!config.tls.cert_file.empty()) {
        std::cout << "TLS enabled:" << std::endl;
        std::cout << "  cert_file: " << config.tls.cert_file << std::endl;
        std::cout << "  key_file: " << config.tls.key_file << std::endl;
        std::cout << "  min_version: " << config.tls.min_version << std::endl;
    } else {
        std::cout << "TLS not configured" << std::endl;
    }

    // Global rate limit (detect via non‑zero requests_per_sec)
    if (config.global_rate_limit.requests_per_sec != 0) {
        std::cout << "Global rate limit: " << config.global_rate_limit.requests_per_sec
                  << " req/s, burst: " << config.global_rate_limit.burst << std::endl;
    }

    // Repeated backends
    std::cout << "Backends:" << std::endl;
    for (const auto& backend : config.backends) {
        std::cout << "  - " << backend.name << " -> " << backend.address;
        if (backend.mandatory) {
            std::cout << " (mandatory)";
        }
        std::cout << std::endl;
    }

    // Repeated routes
    std::cout << "Routes:" << std::endl;
    for (const auto& route : config.routes) {
        std::cout << "  " << route.pattern << " -> " << route.backend_name;
        if (route.timeout_ms != 0) {
            std::cout << " (timeout " << route.timeout_ms << "ms)";
        }
        if (route.enable_compression) {
            std::cout << " (compression enabled)";
        }
        std::cout << std::endl;
    }
}

/// Modifies the configuration: adds a monitoring backend and a health check route.
/// @param config Configuration to be modified.
void modify_config(GatewayConfig& config) {
    // Add new backend
    Backend monitor;
    monitor.name = "monitor";
    monitor.address = "localhost:9090";
    monitor.mandatory = false;
    config.backends.push_back(monitor);

    // Add new route
    Route health_route;
    health_route.pattern = "/health";
    health_route.backend_name = "monitor";
    config.routes.push_back(health_route);
}

/// Retrieves and prints the required (minimal) and full serialized configurations.
/// @param config The configuration to serialize.
void get_serialized_outputs(const GatewayConfig& config) {
    // Minimal output (only required fields)
    auto minimal_val = config.serialize_required_toml();
    std::string minimal_toml = xtoml::serialize(minimal_val).value_or_die();
    std::cout << "\n=== Minimal required configuration ===\n" << minimal_toml << std::endl;

    // Full output (including optional fields and defaults)
    auto full_val = config.serialize_toml();
    std::string full_toml = xtoml::serialize(full_val).value_or_die();
    std::cout << "\n=== Full configuration (with optional fields) ===\n" << full_toml << std::endl;
}

/// Main function demonstrating the usage.
int main() {
    // Raw TOML configuration string (no external file)
    const char* toml_content = R"(
        service_name = "my-gateway"
        listen_port = 8080
        log_level = "INFO"
        worker_threads = 8

        [tls]
        cert_file = "/etc/certs/server.crt"
        key_file = "/etc/certs/server.key"
        min_version = "1.3"

        [global_rate_limit]
        requests_per_sec = 1000
        burst = 2000

        [[backends]]
        name = "auth"
        address = "localhost:9001"
        mandatory = true
        extra_headers = ["X-Auth: basic"]

        [[backends]]
        name = "api"
        address = "localhost:9002"

        [[routes]]
        pattern = "/auth/*"
        backend_name = "auth"
        timeout_ms = 500
        allowed_methods = ["GET", "POST"]

        [[routes]]
        pattern = "/api/*"
        backend_name = "api"
        enable_compression = true
        [[required_backends]]
        name = "core"
        address = "localhost:8000"
    )";

    GatewayConfig config;

    // Parse configuration
    turbo::Status status = parse_config(toml_content, config);
    if (!status.ok()) {
        std::cerr << "Parsing failed: " << status << std::endl;
        return 1;
    }

    // Show original configuration
    std::cout << "=== Loaded configuration ===\n";
    show_config(config);

    // Modify configuration
    modify_config(config);
    std::cout << "\n=== After programmatic modification ===\n";
    show_config(config);

    // Serialize and display outputs
    get_serialized_outputs(config);

    return 0;
}