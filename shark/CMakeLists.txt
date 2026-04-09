#
# Copyright 2024 Kumo.
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
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###kmcmake_example


set(PROTO_FILES
        option/enum.proto
        option/message.proto
        option/file.proto
        shark.proto
)

kmcmake_cc_proto(
        NAME proto_obj
        PROTOS
        ${PROTO_FILES}
        OUTDIR
        ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_object(
        NAMESPACE shark
        NAME proto_obj
        SOURCES
        ${proto_obj_SRCS}
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
)


kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME shark
        SOURCES
        cpp/code_block.cc
        cpp/code_writer.cc
        cpp/km_option.cc
        cpp/message_option.cc
        cpp/field_option.cc
        cpp/generator.cc
        cpp/field_generator.cc
        cpp/message_generator.cc
        names.cc
        OBJECTS
        proto_obj
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME protoc-gen-shark
        SOURCES
        cpp_generator.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        DEPS
        ${PROJECT_NAME}::shark_static
        LINKS
        ${KMCMAKE_DEPS_LINK}
        ${PROJECT_NAME}::shark_static
        ${KMCMAKE_STATIC_BIN_OPTION}
        PUBLIC
)


###kmcmake_example