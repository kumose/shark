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


set(SHARK_IMPORT_TEST_FILES
        base.proto
        repeat.proto
        enum_global.proto
        enum_repeat.proto
        msg_base.proto
        msg_ext.proto
)
kmcmake_cc_tml(
        NAME require_sk_gen
        PROTOS
        ${SHARK_IMPORT_TEST_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME require_test
        SOURCES
        ${require_sk_gen_SRCS}
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}

)

kmcmake_cc_test(
        NAME case_test
        MODULE require
        SOURCES
        base_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::require_test_static
        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)


kmcmake_cc_test(
        NAME repeat_test
        MODULE require
        SOURCES
        repeat_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::require_test_static
        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)


kmcmake_cc_test(
        NAME enum_test
        MODULE require
        SOURCES
        enum_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::require_test_static
        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)


kmcmake_cc_test(
        NAME message_test
        MODULE require
        SOURCES
        message_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::require_test_static
        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)

