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

#include <shark/meta/field_base.h>

namespace shark {

    class FieldMetaGenerator : public FieldGeneratorBase {
    public:
        explicit FieldMetaGenerator(const google::protobuf::FieldDescriptor *descriptor) : FieldGeneratorBase(descriptor) {
        }

        ~FieldMetaGenerator() override =default;

        ///////////////////////////////
        /// constructor and destructor calls
        ///
        void generate_meta_declares(google::protobuf::io::Printer *printer) const override;

        void generate_meta_definition(google::protobuf::io::Printer *printer) const override;
    };

}  // namespace shark
