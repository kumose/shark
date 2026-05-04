#
# Copyright 2023 The titan-search Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


set(SHARK_TML_EXEC_FILE ${PROJECT_BINARY_DIR}/shark/protoc-gen-tml)
#set(SHAR_EXEC_FILE ${SK_EXE_DIR}/protoc-gen-sk)
message(STATUS ${SHARK_TML_EXEC_FILE})
include(kmcmake_cc_tml)

#find_package(shark REQUIRED)

set(SHARK_IDL_TEST_FILES
        example.proto
)
kmcmake_cc_tml(
        NAME example_gen
        PROTOS
        ${SHARK_IDL_TEST_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)


kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME example_gen_lib
        SOURCES
        ${example_gen_SRCS}
        example.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${KMCMAKE_DEPS_LINK}
)
