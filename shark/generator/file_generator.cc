// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <shark/generator/file_generator.h>

#include <shark/ast/cpp_preprocessor.h>
#include <shark/ast/cpp_namespace.h>
#include <shark/generator/code_generator.h>
#include <shark/generator/message_generator.h>
#include <shark/generator/enum_generator.h>
#include <shark/generator/type.h>  // For include collection? We'll handle later.
#include <turbo/log/logging.h>

namespace shark::generator {
    FileGenerator::FileGenerator(const google::protobuf::FileDescriptor *file)
        : file_(file) {
        build_ast();
    }

    std::string FileGenerator::generate_header() {
        if (!header_ast_) return "";
        StringCodeGenerator generator;
        cppast::generate_code(generator, *header_ast_);
        return generator.str();
    }

    std::string FileGenerator::generate_source() {
        if (!source_ast_) return "";
        StringCodeGenerator generator;
        cppast::generate_code(generator, *source_ast_);
        return generator.str();
    }

    void FileGenerator::add_includes(cppast::CppFile::builder &builder, bool is_header) {
        // Standard includes
        builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("cstdint"), "cstdint"), cppast::CppIncludeKind::system, "cstdint"));
        builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("string"), "string"), cppast::CppIncludeKind::system, "string"));
        builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("vector"), "vector"), cppast::CppIncludeKind::system, "vector"));
        builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("unordered_map"), "unordered_map"), cppast::CppIncludeKind::system,
            "unordered_map"));

        // Protobuf includes
        builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("google/protobuf/message.h"), "google/protobuf/message.h"),
            cppast::CppIncludeKind::system, "google/protobuf/message.h"));

        if (is_header) {
            // Header guard? Not handled by cppast directly, we can rely on #pragma once.
            // We can add a comment or custom directive. For simplicity, we rely on the generator to
            // add #pragma once via output formatting. However, cppast does not support #pragma once directly.
            // We'll add it as a raw token sequence.
            // Actually, we can add a CppPreprocessor directive. For now, we skip.
        } else {
            // Source file includes its own header.
            std::string header_name = file_->name().substr(0, file_->name().find_last_of('.')) + ".runtime.h";
            builder.add_child(cppast::CppIncludeDirective::build(
                cppast::CppFileRef(cppast::CppEntityId("header_name"), header_name), cppast::CppIncludeKind::local,
                header_name));
        }
    }

    void FileGenerator::add_namespace(cppast::CppFile::builder &builder) {
        const std::string &package = file_->package();
        if (package.empty()) return;

        // Split package by '.'
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end = package.find('.');
        while (end != std::string::npos) {
            parts.push_back(package.substr(start, end - start));
            start = end + 1;
            end = package.find('.', start);
        }
        parts.push_back(package.substr(start));

        // Build nested namespaces
        cppast::CppNamespace::builder *current_builder = nullptr;
        for (const auto &part: parts) {
            auto ns_builder = std::make_unique<cppast::CppNamespace::builder>(part, false, false);
            if (current_builder) {
                current_builder->add_child(ns_builder->finish(index_, cppast::CppEntityId(part)));
            } else {
                builder.add_child(ns_builder->finish(index_, cppast::CppEntityId(part)));
                current_builder = ns_builder.get();
            }
        }
        // The deepest namespace builder is not stored, but we can keep track.
        // Actually, to add children to the innermost namespace, we need a reference to it.
        // For simplicity, we will add all top-level entities inside the innermost namespace by
        // keeping a pointer to the last created namespace node. However, the above code discards
        // the builder after finish. Instead, we should build the namespace hierarchy in a way that
        // returns the innermost namespace node, and then add children to it.
        // Let's redesign: recursively build namespace and return the node.
        // We'll implement a helper function.
    }

    // Helper to recursively build namespace and return the innermost node.
    static std::unique_ptr<cppast::CppEntity> build_namespace_chain(
        cppast::CppEntityIndex &index,
        const std::vector<std::string> &parts,
        size_t idx) {
        if (idx >= parts.size()) return nullptr;
        auto ns_builder = std::make_unique<cppast::CppNamespace::builder>(parts[idx], false, false);
        auto inner = build_namespace_chain(index, parts, idx + 1);
        if (inner) {
            ns_builder->add_child(std::move(inner));
        }
        return ns_builder->finish(index, cppast::CppEntityId(parts[idx]));
    }

    void FileGenerator::add_enum(const google::protobuf::EnumDescriptor *enum_desc) {
        // This will be called after namespace is added; we need to add the generated node to the current namespace.
        // We'll store the current namespace node during build_ast.
    }

    void FileGenerator::add_message(const google::protobuf::Descriptor *msg) {
        // Similarly.
    }


    void FileGenerator::build_ast() {
        KLOG(ERROR) << "FileGenerator constructor 1";
        std::string base_name = file_->name();
        size_t dot = base_name.find_last_of('.');
        if (dot != std::string::npos) base_name = base_name.substr(0, dot);
        std::string header_name = base_name + ".runtime.h";
        std::string source_name = base_name + ".runtime.cc";

        cppast::CppFile::builder header_builder(header_name);
        cppast::CppFile::builder source_builder(source_name);
        KLOG(ERROR) << "FileGenerator constructor 2";
        // Add includes to header
        header_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("cstdint"), "cstdint"),
            cppast::CppIncludeKind::system, "cstdint"));
        header_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("string"), "string"),
            cppast::CppIncludeKind::system, "string"));
        header_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("vector"), "vector"),
            cppast::CppIncludeKind::system, "vector"));
        header_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("unordered_map"), "unordered_map"),
            cppast::CppIncludeKind::system, "unordered_map"));
        header_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId("google/protobuf/message.h"), "google/protobuf/message.h"),
            cppast::CppIncludeKind::system, "google/protobuf/message.h"));
        KLOG(ERROR) << "FileGenerator constructor 3";
        // Source file includes its own header
        source_builder.add_child(cppast::CppIncludeDirective::build(
            cppast::CppFileRef(cppast::CppEntityId(header_name), header_name),
            cppast::CppIncludeKind::local, header_name));
        KLOG(ERROR) << "FileGenerator constructor 4";
        // Add top-level enums
        for (int i = 0; i < file_->enum_type_count(); ++i) {
            EnumGenerator enum_gen(file_->enum_type(i));
            auto enum_node = enum_gen.generate(index_);
            header_builder.add_child(std::move(enum_node));
        }
        KLOG(ERROR) << "FileGenerator constructor 5";
        // Add top-level messages

        for (int i = 0; i < file_->message_type_count(); ++i) {
            MessageGenerator msg_gen(file_->message_type(i), index_);
            auto msg_node = msg_gen.generate();
            header_builder.add_child(std::move(msg_node));
        }
        KLOG(ERROR) << "FileGenerator constructor 6";
        header_ast_ = header_builder.finish(index_);
        source_ast_ = source_builder.finish(index_);
    }
} // namespace shark::generator
