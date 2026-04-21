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
        generator/message.cc
        generator/generator.cc
        generator/field.cc
        generator/enum.cc
        generator/helpers.cc
        generator/extension.cc
        generator/enum_field.cc
        generator/file.cc
        generator/bytes_field.cc
        generator/message_field.cc
        generator/primitive_field.cc
        generator/service.cc
        generator/string_field.cc
        generator/map_field.cc
        generator/global_state.cc
        generator/oneof_field.cc
        generator/any_field.cc
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)


kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc-gen-sk
        SOURCES
        generator/main.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        -fsanitize=address
        LINKS
        ${PROJECT_NAME}::idl_proto_static
        ${KMCMAKE_DEPS_LINK}
        LINKOPTS
        -fsanitize=address
        PUBLIC
)
#[[
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
]]

###kmcmake_example