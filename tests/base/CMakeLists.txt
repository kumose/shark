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


set(SHARK_IDL_TEST_FILES
        test.proto
        config.proto
)
kmcmake_cc_tml(
        NAME test_sk_gen
        PROTOS
        ${SHARK_IDL_TEST_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)



kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME sk_test
        SOURCES
        ${test_sk_gen_SRCS}
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}

)


kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME cmm
        SOURCES
        comment.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        -fsanitize=address
        LINKS
        shark::sk_test_static

        ${KMCMAKE_DEPS_LINK}
        LINKOPTS
        -fsanitize=address
        ${KMCMAKE_STATIC_BIN_OPTION}
)
kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME format
        SOURCES
        format.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        -fsanitize=address
        LINKS

        ${KMCMAKE_DEPS_LINK}
        LINKOPTS
        -fsanitize=address
        ${KMCMAKE_STATIC_BIN_OPTION}
)

kmcmake_cc_test(
        NAME case_test
        MODULE runtime
        SOURCES
        case_test.cc
        INCLUDES
        ${SHARK_ENUM_GEN_DIR}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::sk_test_static

        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)