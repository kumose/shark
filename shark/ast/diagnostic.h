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


#pragma once

#include <sstream>
#include <string>

#include <typesafe/optional.hpp>

#include <shark/ast/cppast_fwd.h>

namespace cppast {
    /// Describes a physical source location attached to a [cppast::Diagnostic]().
    /// \notes All information might be unavailable.
    struct SourceLocation {
        type_safe::optional<std::string> entity;
        type_safe::optional<std::string> file;
        type_safe::optional<unsigned> line, column;

        /// \returns A source location where all information is available.
        static SourceLocation make(std::string entity, std::string file, unsigned line,
                                    unsigned column) {
            return {std::move(entity), std::move(file), line, column};
        }

        /// \returns A source location where only file information is available.
        static SourceLocation make_file(std::string file,
                                         type_safe::optional<unsigned> line = type_safe::nullopt,
                                         type_safe::optional<unsigned> column = type_safe::nullopt) {
            return {type_safe::nullopt, std::move(file), line, column};
        }

        /// \returns A source location where only the entity name is available.
        static SourceLocation make_entity(std::string entity) {
            return {std::move(entity), type_safe::nullopt, type_safe::nullopt, type_safe::nullopt};
        }

        /// \returns A source location where no information is avilable.
        static SourceLocation make_unknown() {
            return {type_safe::nullopt, type_safe::nullopt, type_safe::nullopt, type_safe::nullopt};
        }

        /// \returns A source location where entity and file name is available.
        static SourceLocation make_entity(std::string entity, std::string file) {
            return {std::move(entity), std::move(file), type_safe::nullopt, type_safe::nullopt};
        }

        /// \returns A possible string representation of the source location.
        /// \notes It will include a separator, but no trailing whitespace.
        std::string to_string() const {
            std::string result;
            if (file) {
                result += file.value() + ":";

                if (line) {
                    result += std::to_string(line.value());

                    if (column)
                        result += "," + std::to_string(column.value());

                    result += ":";
                }

                if (entity)
                    result += " (" + entity.value() + "):";
            } else if (entity && !entity.value().empty())
                result += entity.value() + ":";

            return result;
        }
    };

    /// The severity of a [cppast::Diagnostic]().
    enum class severity {
        debug, //< A debug Diagnostic that is just for debugging purposes.
        info, //< An informational message.
        warning, //< A warning that doesn't impact AST generation.
        error, //< A non-critical error that does impact AST generation but not critically.
        critical, //< A critical error where AST generation isn't possible.
        /// \notes This will usually result in an exception being thrown after the Diagnostic has been
        /// displayed.
    };

    /// \returns A human-readable string describing the severity.
    inline const char *to_string(severity s) noexcept {
        switch (s) {
            case severity::debug:
                return "debug";
            case severity::info:
                return "info";
            case severity::warning:
                return "warning";
            case severity::error:
                return "error";
            case severity::critical:
                return "critical";
        }

        return "programmer error";
    }

    /// A Diagnostic.
    ///
    /// It represents an error message from a [cppast::parser]().
    struct Diagnostic {
        std::string message;
        SourceLocation location;
        cppast::severity severity;
    };

    namespace detail {
        template<typename... Args>
        std::string format(Args &&... args) {
            std::ostringstream stream;
            int dummy[] = {(stream << std::forward<Args>(args), 0)...};
            (void) dummy;
            return stream.str();
        }
    } // namespace detail

    /// Creates a Diagnostic.
    /// \returns A Diagnostic with the specified severity and location.
    /// The message is created by streaming each argument in order to a [std::ostringstream]().
    template<typename... Args>
    Diagnostic format_diagnostic(severity sev, SourceLocation loc, Args &&... args) {
        return {detail::format(std::forward<Args>(args)...), std::move(loc), sev};
    }
} // namespace cppast
