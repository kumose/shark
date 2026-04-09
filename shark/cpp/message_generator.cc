// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <shark/cpp/message_generator.h>
#include <shark/cpp/block_writer.h>
#include <shark/cpp/field_generator.h>
#include <google/protobuf/descriptor.h>
#include <turbo/strings/str_format.h>
#include <turbo/strings/str_split.h>
#include <turbo/strings/strip.h>
#include <memory>
#include <string>
#include <sstream>
#include <turbo/strings/str_replace.h>

namespace shark {

    std::string pb_full_name(std::string pkname) {
        return turbo::str_replace_all(pkname, {{".", "::"}});
    }

    void MessageGenerator::generate_message(const KmgenOption *file, const MessageOption *message, NamespaceBlock* hns, NamespaceBlock* sns) {
        // Null check (only use your defined pointers/references)
        if (!file || !message || !hns || !sns) {
            return;
        }


        const std::string &className = message->km_name();
        const auto field_count = message->field_size();
        std::unique_ptr<MessageBlock> hmessage = std::make_unique<MessageBlock>();
        hmessage->name = className;
        // 1. Build header code (class declaration)
        LinesBuilder header_code0;
        // Class definition (indent aligned with CodeWriter::flush_header "    " prefix)

        header_code0.stream() << className << "() = default;\n";
        header_code0.stream() << "~" << className << "() = default;\n";

        hmessage->publics_func.push_back(header_code0.build());

        auto sc = message->subclasses();

        for (auto it : sc) {
            MessageGenerator().generate_subclass_message(file, &it, hmessage.get(), sns);
        }
        // Generate getter/setter declarations (skip repeated fields)
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_getter(file, message, &field, hmessage.get(), sns);
            FieldGenerator{}.generate_field_setter(file, message, &field, hmessage.get(), sns);
        }
        // Generate getter/setter declarations (skip repeated fields)
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_variable(file, message, &field, hmessage.get(), sns);
        }
        generate_codec_decl(message, hmessage.get());
        generate_codec_def(message, sns);
        hns->blocks.push_back(std::move(hmessage));

    }

    void MessageGenerator::generate_subclass_message(const KmgenOption *file,
                             const MessageOption *message,
                             MessageBlock* mns, NamespaceBlock* sns) {
        // Null check (only use your defined pointers/references)
        if (!file || !message || !mns || !sns) {
            return;
        }


        const std::string &className = message->km_name();
        const auto field_count = message->field_size();
        std::unique_ptr<MessageBlock> hmessage = std::make_unique<MessageBlock>();
        hmessage->name = className;
        // 1. Build header code (class declaration)
        LinesBuilder header_code0;
        // Class definition (indent aligned with CodeWriter::flush_header "    " prefix)

        header_code0.stream() << className << "() = default;\n";
        header_code0.stream() << "~" << className << "() = default;\n";

        hmessage->publics_func.push_back(header_code0.build());

        auto sc = message->subclasses();

        for (auto it : sc) {
            MessageGenerator().generate_subclass_message(file, &it, mns, sns);
        }
        // Generate getter/setter declarations (skip repeated fields)
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_getter(file, message, &field, hmessage.get(), sns);
            FieldGenerator{}.generate_field_setter(file, message, &field, hmessage.get(), sns);
        }

        // Generate getter/setter declarations (skip repeated fields)
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_variable(file, message, &field, hmessage.get(), sns);
        }
        generate_codec_decl(message, hmessage.get());
        generate_codec_def(message, sns);
        mns->publics_def.push_back(std::move(hmessage));

    }

    void MessageGenerator::generate_codec_decl(
                          const MessageOption *message,
                          MessageBlock *hmessage) {
        LinesBuilder header_sd;
        auto *desc = message->descriptor();

        auto pbf_name = pb_full_name(desc->full_name());

        header_sd.stream() << "\n";
        header_sd.stream() << "//////////////////////////////////////////////////////////////////";
        header_sd.stream() << "/// serialize/deserialize with pb message";

        header_sd.stream() << pbf_name << " pb_serialize() const;\n";

        header_sd.stream() << "turbo::Status pb_deserialize(const "<<pbf_name<<" &pb);\n";

        header_sd.stream() << "\n";
        header_sd.stream() << "//////////////////////////////////////////////////////////////////";
        header_sd.stream() << "/// serialize/deserialize with km message, the message design for";
        header_sd.stream() << "/// zero copy decode and encode, absolutely for simd computing";

        header_sd.stream() << "std::string serialize() const;\n";

        header_sd.stream() << "turbo::Status deserialize(std::string_view array);\n";


        hmessage->publics_func.push_back(header_sd.build());

    }

    void MessageGenerator::generate_codec_def(const MessageOption *message,NamespaceBlock* sns) {
        /// pb_serialize
        LinesBuilder source_sd;
        auto *desc = message->descriptor();
        auto pbf_name = pb_full_name(desc->full_name());
        auto pb_var = "pb_var";
        source_sd.stream() << pbf_name <<" "<<message->km_full_name()<<"::pb_serialize() const {";
        source_sd.stream() <<"    "<<pbf_name <<" "<<pb_var<<";";
        const auto field_count = message->field_size();
        source_sd.indent();
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_pb_serialize(message, &field, &source_sd, pb_var);
        }
        source_sd.deindent();
        source_sd.stream() <<"    return "<<pb_var<<";";
        source_sd.stream()<<"}\n";

        /// pb_deserialize
        ///
        source_sd.stream() << "turbo::Status "<<message->km_full_name()<<"::pb_deserialize(const "<<pbf_name<<" &"<<pb_var<<") {";
        source_sd.indent();
        for (int i = 0; i < field_count; ++i) {
            const auto &field = message->field(i);
            FieldGenerator{}.generate_field_pb_deserialize(message, &field, &source_sd, pb_var);
        }
        source_sd.deindent();
        source_sd.stream() <<"    return turbo::OkStatus();";
        source_sd.stream()<<"}\n";

        sns->blocks.push_back(source_sd.build());
    }

} // namespace shark
