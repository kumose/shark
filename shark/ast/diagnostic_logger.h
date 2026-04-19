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

#include <typesafe/reference.hpp>

#include <shark/ast/diagnostic.h>

namespace cppast {
    /// Base class for a [cppast::Diagnostic]() logger.
    ///
    /// Its task is controlling how Diagnostic are being displayed.
    class DiagnosticLogger {
    public:
        /// \effects Creates it either as verbose or not.
        explicit DiagnosticLogger(bool is_verbose = false) noexcept : verbose_(is_verbose) {
        }

        DiagnosticLogger(const DiagnosticLogger &) = delete;

        DiagnosticLogger &operator=(const DiagnosticLogger &) = delete;

        virtual ~DiagnosticLogger() noexcept = default;

        /// \effects Logs the Diagnostic by invoking the `do_log()` member function.
        /// \returns Whether or not the Diagnostic was logged.
        /// \notes `source` points to a string literal that gives additional context to what generates
        /// the message.
        bool log(const char *source, const Diagnostic &d) const;

        /// \effects Sets whether or not the logger prints debugging diagnostics.
        void set_verbose(bool value) noexcept {
            verbose_ = value;
        }

        /// \returns Whether or not the logger prints debugging diagnostics.
        bool is_verbose() const noexcept {
            return verbose_;
        }

    private:
        virtual bool do_log(const char *source, const Diagnostic &d) const = 0;

        bool verbose_;
    };

    /// \returns The default logger object.
    type_safe::object_ref<const DiagnosticLogger> default_logger() noexcept;

    /// \returns The default verbose logger object.
    type_safe::object_ref<const DiagnosticLogger> default_verbose_logger() noexcept;

    /// A [cppast::DiagnosticLogger]() that logs to `stderr`.
    ///
    /// It prints all diagnostics in an implementation-defined format.
    class stderr_diagnostic_logger final : public DiagnosticLogger {
    public:
        using DiagnosticLogger::DiagnosticLogger;

    private:
        bool do_log(const char *source, const Diagnostic &d) const override;
    };
} // namespace cppast
