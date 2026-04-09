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

#pragma once

#include <google/protobuf/descriptor.h>
#include <shark/shark.pb.h>
#include <shark/cpp/field_option.h>
#include <turbo/strings/str_format.h>

namespace shark {

    class KmgenOption;

    class MessageOption {
    public:
        MessageOption() = default;
        ~MessageOption() = default;

        bool init(const KmgenOption * kmgen_option, const google::protobuf::Descriptor *message, const MessageOption * p = nullptr);

        const std::vector<FieldOption> & fields() const;

        size_t field_size() const {
            return _fields.size();
        }

        const FieldOption& field(int index) const {
            return _fields[index];
        }

        const std::string & pb_name() const {
            return _pb_name;
        }

        const std::string & km_name() const {
            return _km_name;
        }
        const std::string & km_full_name() const {
            return _km_full_name;
        }

        bool strict_pod() const {
            return _strict_pod;
        }

        bool singleton() const {
            return _singleton;
        }

        std::vector<MessageOption> subclasses() const {
            return _subclasses;
        }

        const google::protobuf::Descriptor * descriptor() const {
            return _message;
        }
    private:
        const KmgenOption *_kmgen_option{nullptr};
        const google::protobuf::Descriptor *_message{nullptr};
        std::vector<FieldOption> _fields;
        shark::option::MessageOption _option;
        std::string _pb_name;
        std::string _km_name;
        std::string _km_full_name;
        bool _singleton{false};
        bool _strict_pod{false};
        const MessageOption *parent{nullptr};

        std::vector<MessageOption> _subclasses;
    };

}  // namespace shark
