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
        base.proto
        inner.proto
        extern.proto
)
kmcmake_cc_tml(
        NAME enum_sk_gen
        PROTOS
        ${SHARK_IDL_TEST_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)



kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME enum_test
        SOURCES
        ${enum_sk_gen_SRCS}
        PINCLUDES
        ${PROTOBUF_INCLUDE_DIRS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}

)

kmcmake_cc_test(
        NAMESPACE ${PROJECT_NAME}
        MODULE enum
        NAME base_test
        SOURCES
        base_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::enum_test_static

        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAMESPACE ${PROJECT_NAME}
        MODULE enum
        NAME inner_test
        SOURCES
        inner_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::enum_test_static

        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAMESPACE ${PROJECT_NAME}
        MODULE enum
        NAME ext_test
        SOURCES
        ext_test.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        shark::enum_test_static

        GTest::gtest_main
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
)
