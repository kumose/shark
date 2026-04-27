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
file(GLOB_RECURSE SK_SRC "lib/*.cc")
include_directories(lib)
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME shark
        SOURCES
        ${SK_SRC}
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
        generator/tml_file.cc
        generator/message.cc
        generator/oneof_field.cc
        tml/enum.cc
        tml/enum_field.cc
        tml/extension.cc
        tml/file.cc
        tml/message.cc
        tml/message_field.cc
        tml/primitive_field.cc
        tml/service.cc
        tml/string_field.cc
        utility/helpers.cc
        utility/uri.cc
        utility/type.cc
        ${shark_idl_proto_gen_SRCS}
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
        ${KMCMAKE_DEPS_LINK}
        UNITY
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc-gen-tml
        SOURCES
        main.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        -fsanitize=address
        LINKS
        ${PROJECT_NAME}::inner_lib_static
        ${KMCMAKE_DEPS_LINK}
        LINKOPTS
        -fsanitize=address
        ${KMCMAKE_STATIC_BIN_OPTION}
        PUBLIC
)



###kmcmake_example
