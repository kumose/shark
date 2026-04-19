// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
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

// generator_handler.h
#pragma once

#include <string>
#include <vector>
#include <turbo/utility/status.h>

namespace shark {
    enum class GeneratorLevel {
        kGenFile = 0,
        kGenNamespace = 1,
        kGenMessage = 2, // class/struct
        kGenMember = 3, // inside message
        KGenEnum = 4,
        kGenFunc = 5,
        kGenExpr = 6,
    };

    enum class MemberAccessLocation {
        kPrivate,
        kPublic,
        kProtected,
    };
    
    template<typename Sub>
    class GeneratorHandler {
    public:
        virtual ~GeneratorHandler() = default;

        // File level
        turbo::Status start_file() {
            if (!_levels.empty()) {
                return turbo::invalid_argument_error("start_file called while already inside a level");
            }
            _levels.push_back(GeneratorLevel::kGenFile);
            return static_cast<Sub *>(this)->do_start_file();
        }

        turbo::Status end_file() {
            if (_levels.size() != 1 || _levels.back() != GeneratorLevel::kGenFile) {
                return turbo::invalid_argument_error("end_file called outside of file scope");
            }
            _levels.pop_back();
            return static_cast<Sub *>(this)->do_end_file();
        }

        // Namespace level
        turbo::Status start_namespace(const std::string &name) {
            if (_levels.empty() || (_levels.back() != GeneratorLevel::kGenFile && _levels.back() !=
                                    GeneratorLevel::kGenNamespace)) {
                return turbo::invalid_argument_error(
                    "start_namespace can only be called inside a file or another namespace");
            }
            _levels.push_back(GeneratorLevel::kGenNamespace);
            return static_cast<Sub *>(this)->do_start_namespace(name);
        }

        turbo::Status end_namespace() {
            if (_levels.empty() || _levels.back() != GeneratorLevel::kGenNamespace) {
                return turbo::invalid_argument_error("end_namespace called outside of namespace");
            }
            _levels.pop_back();
            return static_cast<Sub *>(this)->do_end_namespace();
        }

        // Message level (class)
        turbo::Status start_message(const std::string &name) {
            if (_levels.empty() || (_levels.back() != GeneratorLevel::kGenFile && _levels.back() !=
            GeneratorLevel::kGenNamespace && _levels.back() !=
            GeneratorLevel::kGenMessage)) {
                return turbo::invalid_argument_error("start_message can only be called inside a file or namespace");
            }
            _levels.push_back(GeneratorLevel::kGenMessage);
            return static_cast<Sub *>(this)->do_start_message(name);
        }

        turbo::Status end_message() {
            if (_levels.empty() || _levels.back() != GeneratorLevel::kGenMessage) {
                return turbo::invalid_argument_error("end_message called outside of message");
            }
            _levels.pop_back();
            return static_cast<Sub *>(this)->do_end_message();
        }

        // Private member variable
        turbo::Status add_member(const std::string &type, const std::string &name, MemberAccessLocation acl) {
            if (_levels.empty() || _levels.back() != GeneratorLevel::kGenMessage) {
                return turbo::invalid_argument_error("add_private_member must be called inside a message");
            }
            return static_cast<Sub *>(this)->do_add_private_member(type, name);
        }

        // Ordinary method (e.g., FromProto/ToProto)
        turbo::Status add_method(const std::string &return_type, const std::string &name,
                                 const std::vector<std::string> &params, bool is_const) {
            if (_levels.empty() || _levels.back() != GeneratorLevel::kGenMessage) {
                return turbo::invalid_argument_error("add_method must be called inside a message");
            }
            return static_cast<Sub *>(this)->do_add_method(return_type, name, params, is_const);
        }

        // Source file only: add a function definition (typically used for FromProto/ToProto implementations)
        turbo::Status add_function_definition(const std::string &return_type, const std::string &name,
                                              const std::vector<std::string> &params, const std::string &body) {
            // Allowed at file or namespace level
            if (_levels.empty() || (_levels.back() != GeneratorLevel::kGenFile && _levels.back() !=
                                    GeneratorLevel::kGenNamespace)) {
                return turbo::invalid_argument_error(
                    "add_function_definition can only be called inside a file or namespace");
            }
            return static_cast<Sub *>(this)->do_add_function_definition(return_type, name, params, body);
        }

    protected:
        // Pure virtual functions implemented by concrete subclasses
        virtual turbo::Status do_start_file() = 0;

        virtual turbo::Status do_end_file() = 0;

        virtual turbo::Status do_start_namespace(const std::string &name) = 0;

        virtual turbo::Status do_end_namespace() = 0;

        virtual turbo::Status do_start_message(const std::string &name) = 0;

        virtual turbo::Status do_end_message() = 0;

        virtual turbo::Status do_add_private_member(const std::string &type, const std::string &name) = 0;

        virtual turbo::Status do_add_getter(const std::string &return_type, const std::string &name, bool is_const) = 0;

        virtual turbo::Status do_add_setter(const std::string &param_type, const std::string &name) = 0;

        virtual turbo::Status do_add_method(const std::string &return_type, const std::string &name,
                                            const std::vector<std::string> &params, bool is_const) = 0;

        virtual turbo::Status do_add_function_definition(const std::string &return_type, const std::string &name,
                                                         const std::vector<std::string> &params,
                                                         const std::string &body) = 0;

    private:
        std::vector<GeneratorLevel> _levels;
    };
} // namespace shark
