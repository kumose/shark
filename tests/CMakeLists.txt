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

#[[kmcmake_cc_test(
        NAME custom_comparator_test
        SOURCES custom_comparator_test.cc
        DEPS turbo::turbo ${KMCMAKE_DEPS_LINK} ${GTEST_LIB} ${GTEST_MAIN_LIB}
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)]]
#[[
kmcmake_cc_test(
        NAME foo_test
        SOURCES foo_test.cc
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} gtest gtest_main
        COPTS ${KMCMAKE_CXX_OPTIONS}
)

kmcmake_cc_test(
        NAME foo_doctest
        SOURCES foo_doctest.cc
        DEPS ${PROJECT}::foo ${KMCMAKE_DEPS_LINK} doctest
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
]]

configure_file(${PROJECT_SOURCE_DIR}/tests/config.h.in ${PROJECT_SOURCE_DIR}/tests/config.h @ONLY)
#[[

set(SHARK_ENUM_PROTO "${PROJECT_SOURCE_DIR}/tests/proto/enum_runtime.proto")
set(SHARK_ENUM_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(SHARK_ENUM_GEN_HEADER "${SHARK_ENUM_GEN_DIR}/tests/proto/enum_runtime.runtime.h")
set(SHARK_ENUM_GEN_SOURCE "${SHARK_ENUM_GEN_DIR}/tests/proto/enum_runtime.runtime.cc")
set(SHARK_ENUM_PB_HEADER "${SHARK_ENUM_GEN_DIR}/tests/proto/enum_runtime.pb.h")
set(SHARK_ENUM_PB_SOURCE "${SHARK_ENUM_GEN_DIR}/tests/proto/enum_runtime.pb.cc")

add_custom_command(
        OUTPUT ${SHARK_ENUM_GEN_HEADER} ${SHARK_ENUM_GEN_SOURCE} ${SHARK_ENUM_PB_HEADER} ${SHARK_ENUM_PB_SOURCE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHARK_ENUM_GEN_DIR}/tests/proto"
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
        --plugin=protoc-gen-shark=$<TARGET_FILE:protoc_gen_shark>
        -I${PROJECT_SOURCE_DIR}
        -I${PROTOBUF_INCLUDE_DIRS}
        --shark_out=${SHARK_ENUM_GEN_DIR}
        --cpp_out=${SHARK_ENUM_GEN_DIR}
        ${SHARK_ENUM_PROTO}
        DEPENDS ${SHARK_ENUM_PROTO} protoc_gen_shark
        VERBATIM
)

add_custom_target(enum_runtime_codegen
        DEPENDS ${SHARK_ENUM_GEN_HEADER} ${SHARK_ENUM_GEN_SOURCE} ${SHARK_ENUM_PB_HEADER} ${SHARK_ENUM_PB_SOURCE}
)

kmcmake_cc_test(
        NAME enum_runtime_test
        MODULE runtime
        SOURCES
        enum_runtime_test.cc
        ${SHARK_ENUM_GEN_SOURCE}
        ${SHARK_ENUM_PB_SOURCE}
        DEPS
        enum_runtime_codegen
        INCLUDES
        ${SHARK_ENUM_GEN_DIR}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::idl_proto_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME enum_runtime_golden_test
        MODULE runtime
        SOURCES
        enum_runtime_golden_test.cc
        DEPS
        enum_runtime_codegen
        INCLUDES
        ${SHARK_ENUM_GEN_DIR}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        DEFINES
        SHARK_ENUM_RUNTIME_HEADER="${SHARK_ENUM_GEN_HEADER}"
        SHARK_ENUM_RUNTIME_SOURCE="${SHARK_ENUM_GEN_SOURCE}"
        LINKS
        ${KMCMAKE_DEPS_LINK}
)
]]