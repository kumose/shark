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

include_directories(include)


kmcmake_cc_proto(
        NAME shark_idl_proto_gen
        PROTOS
        ${SHARK_IDL_PROTO_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME shark
        SOURCES
        lib/shark.cc
        lib/uri.cc
        lib/type.cc
        ${shark_idl_proto_gen_SRCS}
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
        UNITY
)


set(GENERATOR_SRC
        generator/global_state.cc
        generator/field_base.cc
        generator/generator.cc
        generator/field_map.cc
        generator/rt_file.cc
        generator/message.cc
        generator/oneof_field.cc
        rt/any_field.cc
        rt/bytes_field.cc
        rt/enum.cc
        rt/enum_field.cc
        rt/extension.cc
        rt/file.cc
        rt/map_field.cc
        rt/message.cc
        rt/message_field.cc
        rt/oneof_field.cc
        rt/primitive_field.cc
        rt/service.cc
        rt/string_field.cc
        utility/helpers.cc
)

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME inner_lib
        SOURCES
        ${GENERATOR_SRC}
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        shark::shark_static
        ${KMCMAKE_DEPS_LINK}
        UNITY
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc-gen-sk
        SOURCES
        main.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::inner_lib_static
        ${KMCMAKE_DEPS_LINK}
        LINKOPTS
        ${KMCMAKE_STATIC_BIN_OPTION}
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

