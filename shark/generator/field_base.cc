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

#include <shark/generator/field_base.h>
#include <shark/utility/uri.h>

namespace shark {

    FieldGeneratorBase::FieldGeneratorBase(const google::protobuf::FieldDescriptor *descriptor) : descriptor_(descriptor) {

    }

    void FieldGeneratorBase::initialize() {

        auto d = get_default_value();
        if (!d.empty()) {
            _variables["default_init"] = d;
        } else {
            _variables["default_init"] = "";
        }

        _option = descriptor_->options();
        if (descriptor_->options().HasExtension(idl::shark_field)) {
            _ext_option = descriptor_->options().GetExtension(idl::shark_field);
        }

        if (descriptor_->options().deprecated()) {
            _variables["deprecated"] = "[[deprecated]] ";
        }

        _variables["name"] = varify_field_name(descriptor_);
        _variables["domain"] = message_type(descriptor_->containing_type());
        _variables["cnamespace"] = GlobalState::instance().cnamespace;

        do_initialize();

    }
}  // namespace shark
