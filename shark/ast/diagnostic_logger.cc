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



#include <shark/ast/diagnostic_logger.h>

#include <cstdio>
#include <mutex>

using namespace cppast;

bool DiagnosticLogger::log(const char *source, const Diagnostic &d) const {
    if (!verbose_ && d.severity == severity::debug)
        return false;
    return do_log(source, d);
}

type_safe::object_ref<const DiagnosticLogger> cppast::default_logger() noexcept {
    static const stderr_diagnostic_logger logger(false);
    return type_safe::ref(logger);
}

type_safe::object_ref<const DiagnosticLogger> cppast::default_verbose_logger() noexcept {
    static const stderr_diagnostic_logger logger(true);
    return type_safe::ref(logger);
}

bool stderr_diagnostic_logger::do_log(const char *source, const Diagnostic &d) const {
    auto loc = d.location.to_string();
    if (loc.empty())
        std::fprintf(stderr, "[%s] [%s] %s\n", source, to_string(d.severity), d.message.c_str());
    else
        std::fprintf(stderr, "[%s] [%s] %s %s\n", source, to_string(d.severity),
                     d.location.to_string().c_str(), d.message.c_str());
    return true;
}
