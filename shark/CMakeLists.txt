# Copyright (C) Kumo inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###kmcmake_example
set(SHARK_IDL_PROTO_FILES
        idl/shark_options.proto
)

kmcmake_cc_proto(
        NAME shark_idl_proto_gen
        PROTOS
        ${SHARK_IDL_PROTO_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)


kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME idl_proto
        SOURCES
        ${shark_idl_proto_gen_SRCS}
        ast/code_generator.cc
        ast/cpp_alias_template.cc
        ast/cpp_attribute.cc
        ast/cpp_class.cc
        ast/cpp_class_template.cc
        ast/cpp_concept.cc
        ast/cpp_entity.cc
        ast/cpp_entity_index.cc
        ast/cpp_entity_kind.cc
        ast/cpp_enum.cc
        ast/cpp_expression.cc
        ast/cpp_file.cc
        ast/cpp_forward_declarable.cc
        ast/cpp_friend.cc
        ast/cpp_function.cc
        ast/cpp_function_template.cc
        ast/cpp_language_linkage.cc
        ast/cpp_member_function.cc
        ast/cpp_member_variable.cc
        ast/cpp_namespace.cc
        ast/cpp_preprocessor.cc
        ast/cpp_static_assert.cc
        ast/cpp_template_parameter.cc
        ast/cpp_token.cc
        ast/cpp_type.cc
        ast/cpp_type_alias.cc
        ast/cpp_variable.cc
        ast/cpp_variable_template.cc
        ast/diagnostic_logger.cc
        ast/visitor.cc
        generator/type.cc
        generator/type_mapper.cc
        generator/include_manager.cc
        generator/message_generator.cc
        generator/enum_generator.cc
        generator/file_generator.cc
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME generator_cc
        SOURCES
        generator/enum_codegen.cc
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${PROJECT_NAME}::idl_proto_static
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc_gen_shark
        SOURCES
        cpp_generator.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        DEPS
        ${PROJECT_NAME}::generator_cc_static
        LINKS
        ${PROJECT_NAME}::generator_cc_static
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc-gen-sk
        SOURCES
        shark.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        DEPS
        ${PROJECT_NAME}::generator_cc_static
        LINKS
        ${PROJECT_NAME}::generator_cc_static
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

###kmcmake_example