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

#include <shark/shark.pb.h>
#include <google/protobuf/descriptor.h>
#include <shark/cpp/message_option.h>

namespace shark {
    class KmgenOption {
    public:
        KmgenOption() = default;

        bool init(const google::protobuf::FileDescriptor *file);

        ~KmgenOption() = default;

        const std::string &pb_namespace() const;
        const std::string &km_namespace() const;
        const std::vector<std::string>& c_includes() const;

        const std::vector<MessageOption> &messages() const;

        size_t message_size() const {
            return _messages.size();
        }

        const MessageOption & message(size_t i) const {
            return _messages[i];
        }

    private:
        const google::protobuf::FileDescriptor *_file{nullptr};
        shark::option::KmOptions _opts;
        std::string _pb_ns;
        std::string _km_ns;
        std::vector<std::string> _c_includes;
        std::string _repeat_type;

        std::vector<MessageOption> _messages;
    };
} // namespace shark
