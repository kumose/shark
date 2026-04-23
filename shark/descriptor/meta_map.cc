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



#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>

#include <shark/idl/shark_options.pb.h>
#include <shark/descriptor/meta_map.h>
#include <shark/utility/helpers.h>
#include <shark/utility/compat.h>

namespace shark {

    FieldMetaMap::FieldMetaMap(const google::protobuf::Descriptor *descriptor)
        : descriptor_(descriptor) {
        // Construct all the FieldGenerators.
        process_message();
    }

    std::unique_ptr<FieldGeneratorBase> FieldMetaMap::make_generator(const google::protobuf::FieldDescriptor *field) {
        const idl::SharkFieldOptions opt = field->options().GetExtension(idl::shark_field);
        switch (field->type()) {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                    return std::make_unique<MapFieldGenerator>(field);
                }
                if (is_protobuf_any(field)) {
                    return std::make_unique<AnyFieldGenerator>(field);
                }
                return std::make_unique<MessageFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                if (opt.string_as_bytes())
                    return std::make_unique<BytesFieldGenerator>(field);
                else
                    return std::make_unique<StringFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return std::make_unique<BytesFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
                return std::make_unique<EnumFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
                return nullptr; // XXX
            default:
                return std::make_unique<PrimitiveFieldGenerator>(field);
        }
    }

    const FieldGeneratorBase &FieldMetaMap::get(
        const google::protobuf::FieldDescriptor *field) const {
        KCHECK_EQ(field->containing_type(), descriptor_);
        return *field_generators_[field->index()];
    }



     void FieldMetaMap::process_message() {
        auto r = std::make_unique<FieldMeta>();

        auto rptr = r.get();
        field_meta_map["root"] = rptr;
        field_metas.push_back(std::move(r));
        rptr->index = _index++;
        rptr->data_index = _data_index++;
        std::deque<FieldMeta*> que;
        for (auto i = 0; i < descriptor_->field_count(); ++i ) {
            auto field = descriptor_->field(i);

            auto field_ptr = make_generator(field);
            field_ptr->initialize();
            field_generators_.push_back(std::move(field_ptr));

            auto meta = std::make_unique<FieldMeta>();
            meta->field = field;
            meta->repeated = field->is_repeated();
            meta->path =field->name();
            meta->index = _index++;
            meta->root = descriptor_;
            auto ptr = meta.get();
            field_map[meta->path] = ptr;
            field_meta_map[meta->path] = ptr;
            field_metas.push_back(std::move(meta));

            if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
                ptr->data_index = -1;
                que.push_back(ptr);
                continue;
            }
            ptr->data_index = _data_index++;
            if (field->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {
                ptr->cpp_type = cpp_type(field,true);
            } else {
                ptr->cpp_type = "enum";
            }
            KLOG(INFO)<<"uri:"<<field->full_name()<<"cpp type:"<<ptr->cpp_type;
        }

        while (!que.empty()) {
            auto entity = que.front();
            que.pop_front();
            for (auto i = 0; i < entity->field->message_type()->field_count(); ++i) {
                auto field = entity->field->message_type()->field(i);

                auto field_ptr = make_generator(field);
                field_ptr->initialize();
                field_generators_.push_back(std::move(field_ptr));

                auto meta = std::make_unique<FieldMeta>();
                meta->field = field;
                meta->path =entity->path + "." + field->name();
                meta->index = _index++;
                meta->root = descriptor_;
                entity->column.push_back(meta->index);
                auto ptr = meta.get();
                field_map[meta->path] = ptr;
                field_meta_map[meta->path] = ptr;
                field_metas.push_back(std::move(meta));

                if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
                    ptr->data_index  = -1;
                    que.push_back(ptr);
                    continue;
                }
                ptr->data_index = _data_index++;
                if (field->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {
                    ptr->cpp_type = cpp_type(field,true);
                } else {
                    ptr->cpp_type = "enum";
                }
            }
        }
    }
} // namespace shark
