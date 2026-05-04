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

#include <shark/generator/oneof_field.h>
#include <shark/utility/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    OneofFieldGeneratorBase::OneofFieldGeneratorBase(const google::protobuf::OneofDescriptor *oneof)
        : _oneof(oneof) {
        _descriptor = _oneof->containing_type();
        _variable["domain"] = get_message_type(_descriptor);
        _variable["opt_comma"] = ",";

        _variable["oneofname"] = CamelToUpper(oneof->name());
        _variable["foneofname"] = oneof->name();
        auto pb_uri = turbo::str_replace_all(_descriptor->full_name(), {{".", "::"}});
        _variable["pb_uri"] = pb_uri;
        _variable["Coneofname"] = ToCamel(_oneof->name());
    }

} // namespace shark
