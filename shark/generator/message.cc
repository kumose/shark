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

#include <shark/generator/message.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_replace.h>
#include <shark/uri.h>
#include <shark/utility/helpers.h>

namespace shark {
    MessageGeneratorBase::MessageGeneratorBase(const google::protobuf::Descriptor *descriptor,
                                               const std::string &dllexport_decl) : _descriptor(descriptor),
        _dllexport_decl(dllexport_decl) {
        _variables["classname"] = _descriptor->name();
        _variables["domain"] = message_type(_descriptor);
        _variables["lcclassname"] = FullNameToLower(_descriptor->full_name(), _descriptor->file());
        _variables["ucclassname"] = FullNameToUpper(_descriptor->full_name(), _descriptor->file());
        _variables["field_count"] = turbo::str_cat(_descriptor->field_count());
        _variables["PBTYPE"] = turbo::str_replace_all(_descriptor->full_name(), {{".", "::"}});
        if (_dllexport_decl.empty()) {
            _variables["dllexport"] = "";
        } else {
            _variables["dllexport"] = _dllexport_decl + " ";
        }
    }
} // namespace shark
