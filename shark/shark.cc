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

#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <shark/generator/file_generator.h>
#include <turbo/log/logging.h>
class SharkGenerator : public google::protobuf::compiler::CodeGenerator {
public:
    bool Generate(const google::protobuf::FileDescriptor *file,
                  const std::string &parameter,
                  google::protobuf::compiler::GeneratorContext *context,
                  std::string *error) const override {
        KLOG(ERROR) << "Shark Generator 1";
        shark::generator::FileGenerator file_gen(file);
        KLOG(ERROR) << "Shark Generator 2";
        std::string header_code = file_gen.generate_header();
        std::string source_code = file_gen.generate_source();
        KLOG(ERROR) << "Shark Generator 3";
        std::string base_name = file->name();
        size_t dot = base_name.find_last_of('.');
        KLOG(ERROR) << "Shark Generator 4";
        if (dot != std::string::npos) base_name = base_name.substr(0, dot);
        std::string header_name = base_name + ".sk.h";
        std::string source_name = base_name + ".sk.cc";
        KLOG(ERROR) << "Shark Generator 5";

        std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> header_out(
            context->Open(header_name));
        KLOG(ERROR) << "Shark Generator 6";
        std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> source_out(
            context->Open(source_name));
        KLOG(ERROR) << "Shark Generator 7";

        google::protobuf::io::CodedOutputStream header_coded(header_out.get());
        KLOG(ERROR) << "Shark Generator 8";
        header_coded.WriteRaw(header_code.data(), header_code.size());
        KLOG(ERROR) << "Shark Generator 9";
        google::protobuf::io::CodedOutputStream source_coded(source_out.get());
        KLOG(ERROR) << "Shark Generator 10";
        source_coded.WriteRaw(source_code.data(), source_code.size());
        KLOG(ERROR) << "Shark Generator 11";

        return true;
    }
};

int main(int argc, char *argv[]) {
    turbo::setup_rotating_file_sink("1.txt");
    KLOG(WARNING)<< "Shark Generator 1";
    SharkGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
