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

#include <shark/generator/message_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_member_variable.h>
#include <shark/ast/cpp_member_function.h>
#include <shark/ast/cpp_function.h>
#include <shark/ast/cpp_type.h>
#include <shark/ast/cpp_entity_index.h>
#include <turbo/log/logging.h>
#include "shark/generator/enum_generator.h"

namespace shark::generator {
    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor *msg,
                                       cppast::CppEntityIndex &index)
        : msg_(msg), index_(index), class_builder_(msg->name(), cppast::CppClassKind::class_t) {
    }

    std::unique_ptr<cppast::CppEntity> MessageGenerator::generate() {
        KLOG(ERROR) << "MessageGenerator::generate start: " << msg_->name();
        add_member_variables();
        KLOG(ERROR) << "MessageGenerator after add_member_variables";
        add_getters();
        KLOG(ERROR) << "MessageGenerator after add_getters";
        add_setters();
        KLOG(ERROR) << "MessageGenerator after add_setters";
        add_from_proto_declaration();
        KLOG(ERROR) << "MessageGenerator after add_from_proto";
        add_to_proto_declaration();
        KLOG(ERROR) << "MessageGenerator after add_to_proto";
        add_nested_types();
        KLOG(ERROR) << "MessageGenerator after add_nested_types";
        auto result = class_builder_.finish(index_, cppast::CppEntityId(msg_->name()), {});
        KLOG(ERROR) << "MessageGenerator finish done";
        return result;
    }

    void MessageGenerator::add_member_variables() {
        KLOG(ERROR) << "MessageGenerator::add_member_variables 1" << " field count:" << msg_->field_count();
        for (int i = 0; i < msg_->field_count(); ++i) {
            KLOG(ERROR) << "MessageGenerator::add_member_variables f1";
            auto field = msg_->field(i);
            KLOG(ERROR) << "MessageGenerator::add_member_variables f2";
            auto type = type_mapper_.map_field(field);
            KLOG(ERROR) << "MessageGenerator::add_member_variables f3";
            auto cpp_type = type.build();
            KLOG(ERROR) << "MessageGenerator::add_member_variables f4";
            // Member variable name: field_name_ (snake_case with trailing underscore)
            std::string member_name = field->name() + "_";
            auto member = cppast::CppMemberVariable::build(
                index_,
                cppast::CppEntityId(msg_->name() + "_" + field->name() + "_var"),
                member_name,
                std::move(cpp_type),
                /*default_value=*/nullptr,
                /*is_mutable=*/false);
            KLOG(ERROR) << "MessageGenerator::add_member_variables f5";
            class_builder_.get().add_child(std::move(member));
            KLOG(ERROR) << "MessageGenerator::add_member_variables f6";
        }
    }

    void MessageGenerator::add_getters() {
        for (int i = 0; i < msg_->field_count(); ++i) {
            auto field = msg_->field(i);
            Type type = type_mapper_.map_field(field);
            std::unique_ptr<cppast::CppType> return_type;

            bool is_scalar = !field->is_repeated() && !field->is_map() &&
                             field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_STRING &&
                             field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE;

            if (is_scalar) {
                // 标量类型：按值返回
                return_type = type.build();
            } else {
                // 非标量（string, message, repeated, map）：返回 const T&
                auto base = type.build();
                auto const_base = cppast::CppCvQualifiedType::build(std::move(base), cppast::cpp_cv_const);
                return_type = cppast::CppReferenceType::build(std::move(const_base), cppast::cpp_ref_lvalue);
            }

            cppast::CppMemberFunction::builder getter_builder(field->name(), std::move(return_type));
            getter_builder.cv_ref_qualifier(cppast::cpp_cv_const, cppast::cpp_ref_none);
            auto getter = getter_builder.finish(index_,
                cppast::CppEntityId(msg_->name() + "_" + field->name() + "_getter"),
                cppast::cpp_function_declaration, {});
            class_builder_.get().add_child(std::move(getter));
        }
    }

    void MessageGenerator::add_setters() {
        for (int i = 0; i < msg_->field_count(); ++i) {
            auto field = msg_->field(i);
            // 第一版暂不为 repeated/map 生成 setter
            if (field->is_repeated() || field->is_map()) continue;

            Type type = type_mapper_.map_field(field);
            std::unique_ptr<cppast::CppType> param_type;
            if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_STRING ||
                field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
                auto base = type.build();
                auto ref = cppast::CppReferenceType::build(std::move(base), cppast::cpp_ref_lvalue);
                param_type = cppast::CppCvQualifiedType::build(std::move(ref), cppast::cpp_cv_const);
            } else {
                param_type = type.build();
            }

            auto param = cppast::CppFunctionParameter::build(
                index_,
                cppast::CppEntityId(msg_->name() + "_" + field->name() + "_setter_param"),
                "value", std::move(param_type), nullptr);

            cppast::CppMemberFunction::builder setter_builder("set_" + field->name(),
                                                              cppast::CppBuiltinType::build(cppast::cpp_void));
            setter_builder.add_parameter(std::move(param));
            auto setter = setter_builder.finish(index_,
                                                cppast::CppEntityId(msg_->name() + "_" + field->name() + "_setter"),
                                                cppast::cpp_function_declaration, {});
            class_builder_.get().add_child(std::move(setter));
        }
    }

    void MessageGenerator::add_from_proto_declaration() {
        // bool FromProto(const google::protobuf::Message& proto);
        auto param_type = cppast::CppReferenceType::build(
            cppast::CppCvQualifiedType::build(
                cppast::CppUserDefinedType::build(
                    cppast::CppTypeRef(cppast::CppEntityId("google::protobuf::Message"), "google::protobuf::Message")),
                cppast::cpp_cv_const),
            cppast::cpp_ref_lvalue);
        auto param = cppast::CppFunctionParameter::build(
            index_,
            cppast::CppEntityId(msg_->name() + "_FromProto_param"),
            "proto", std::move(param_type), nullptr);
        cppast::CppMemberFunction::builder builder("FromProto",
                                                   cppast::CppBuiltinType::build(cppast::cpp_bool));
        builder.add_parameter(std::move(param));
        auto method = builder.finish(index_,
                                     cppast::CppEntityId(msg_->name() + "_FromProto"),
                                     cppast::cpp_function_declaration,
                                     /*semantic_parent=*/{});
        class_builder_.get().add_child(std::move(method));
    }

    void MessageGenerator::add_to_proto_declaration() {
        // bool ToProto(google::protobuf::Message* proto) const;
        auto param_type = cppast::CppPointerType::build(
            cppast::CppUserDefinedType::build(
                cppast::CppTypeRef(cppast::CppEntityId("google::protobuf::Message"), "google::protobuf::Message")));
        auto param = cppast::CppFunctionParameter::build(
            index_,
            cppast::CppEntityId(msg_->name() + "_ToProto_param"),
            "proto", std::move(param_type), nullptr);
        cppast::CppMemberFunction::builder builder("ToProto",
                                                   cppast::CppBuiltinType::build(cppast::cpp_bool));
        builder.add_parameter(std::move(param));
        builder.cv_ref_qualifier(cppast::cpp_cv_const, cppast::cpp_ref_none);
        auto method = builder.finish(index_,
                                     cppast::CppEntityId(msg_->name() + "_ToProto"),
                                     cppast::cpp_function_declaration,
                                     /*semantic_parent=*/{});
        class_builder_.get().add_child(std::move(method));
    }

    void MessageGenerator::add_nested_types() {
        // Nested enums
        for (int i = 0; i < msg_->enum_type_count(); ++i) {
            auto enum_desc = msg_->enum_type(i);
            EnumGenerator enum_gen(enum_desc);
            auto enum_node = enum_gen.generate(index_);
            class_builder_.get().add_child(std::move(enum_node));
        }
        // Nested messages
        for (int i = 0; i < msg_->nested_type_count(); ++i) {
            auto nested = msg_->nested_type(i);
            // 跳过 protobuf 内部用于表示 map 的 entry 消息
            if (nested->options().map_entry()) {
                continue;
            }
            MessageGenerator nested_gen(nested, index_);
            auto nested_class = nested_gen.generate();
            class_builder_.get().add_child(std::move(nested_class));
        }
    }
} // namespace shark::generator
