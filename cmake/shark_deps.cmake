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
############################################################
# system pthread and rt, dl
############################################################
set(KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS "")

# Record a private find_package(...) call for generated <Project>Config.cmake.
# Usage:
#   kmcmake_private_find_package(ZLIB REQUIRED)
function(kmcmake_private_find_package)
    string(JOIN " " _KMCMAKE_PRIVATE_FIND_PACKAGE_ARGS ${ARGV})
    string(APPEND KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS
            "find_dependency(${_KMCMAKE_PRIVATE_FIND_PACKAGE_ARGS})\n")
    set(KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS "${KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS}" PARENT_SCOPE)
endfunction()

# Record a private find_library(...) call for generated <Project>Config.cmake.
# Usage:
#   kmcmake_private_find_library(MYLIB NAMES mylib PATHS /opt/mylib/lib)
function(kmcmake_private_find_library)
    string(JOIN " " _KMCMAKE_PRIVATE_FIND_LIBRARY_ARGS ${ARGV})
    string(APPEND KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS
            "find_library(${_KMCMAKE_PRIVATE_FIND_LIBRARY_ARGS})\n")
    set(KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS "${KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS}" PARENT_SCOPE)
endfunction()

set(KMCMAKE_SYSTEM_DYLINK)
if (APPLE)
    find_library(CoreFoundation CoreFoundation)
    list(APPEND KMCMAKE_SYSTEM_DYLINK ${CoreFoundation})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND KMCMAKE_SYSTEM_DYLINK rt dl)
endif ()

if (KMCMAKE_BUILD_TEST)
    enable_testing()
    #include(require_gtest)
    #include(require_gmock)
    #include(require_doctest)
endif (KMCMAKE_BUILD_TEST)

if (KMCMAKE_BUILD_BENCHMARK)
    #include(require_benchmark)
endif ()

find_package(Threads REQUIRED)
kmcmake_private_find_package(Threads REQUIRED)
list(APPEND KMCMAKE_SYSTEM_DYLINK Threads::Threads)

find_package(protobuf CONFIG QUIET)
if (protobuf_FOUND)
    # Prefer protobuf config packages (vcpkg/conan/system modern installs).
    kmcmake_private_find_package(protobuf CONFIG REQUIRED)
    if (NOT Protobuf_VERSION AND protobuf_VERSION)
        set(Protobuf_VERSION "${protobuf_VERSION}")
    endif ()

    # Backward-compatible variables required by kmcmake_cc_proto.
    if (TARGET protobuf::libprotobuf)
        get_target_property(_PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)
        set(PROTOBUF_INCLUDE_DIRS "${_PROTOBUF_INCLUDE_DIRS}")
        set(Protobuf_INCLUDE_DIRS "${_PROTOBUF_INCLUDE_DIRS}")
    endif ()

    if (TARGET protobuf::protoc)
        set(PROTOBUF_PROTOC_EXECUTABLE "$<TARGET_FILE:protobuf::protoc>")
    else ()
        find_program(PROTOBUF_PROTOC_EXECUTABLE protoc REQUIRED)
    endif ()
else ()
    # Fallback to classic FindProtobuf module mode.
    find_package(Protobuf REQUIRED)
    kmcmake_private_find_package(Protobuf REQUIRED)
endif ()
if (Protobuf_VERSION VERSION_GREATER_EQUAL 4.25)
    # Protobuf 4.25+ on some package builds requires explicit absl targets.
    find_package(absl REQUIRED CONFIG)
    kmcmake_private_find_package(absl REQUIRED CONFIG)
    set(protobuf_ABSL_USED_TARGETS
            absl::absl_check
            absl::absl_log
            absl::algorithm
            absl::base
            absl::bind_front
            absl::bits
            absl::btree
            absl::cleanup
            absl::cord
            absl::core_headers
            absl::debugging
            absl::die_if_null
            absl::dynamic_annotations
            absl::flags
            absl::flat_hash_map
            absl::flat_hash_set
            absl::function_ref
            absl::hash
            absl::layout
            absl::log_initialize
            absl::log_severity
            absl::memory
            absl::node_hash_map
            absl::node_hash_set
            absl::optional
            absl::span
            absl::status
            absl::statusor
            absl::strings
            absl::synchronization
            absl::time
            absl::type_traits
            absl::utility
            absl::variant
    )
endif ()

############################################################
#
# add you libs to the KMCMAKE_DEPS_LINK variable eg as turbo
# so you can and system pthread and rt, dl already add to
# KMCMAKE_SYSTEM_DYLINK, using it for fun.
##########################################################
set(KMCMAKE_DEPS_LINK
        protobuf::libprotobuf
        ${protobuf_ABSL_USED_TARGETS}
        protobuf::libprotoc
        ${KMCMAKE_SYSTEM_DYLINK}
        )
list(REMOVE_DUPLICATES KMCMAKE_DEPS_LINK)
kmcmake_print_list_label("Denpendcies:" KMCMAKE_DEPS_LINK)

############################################################
# for binary
############################################################
set(KMCMAKE_STATIC_BIN_OPTION)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND KMCMAKE_STATIC_BIN_OPTION -static-libgcc -static-libstdc++)
endif ()



