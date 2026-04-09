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
#include <google/protobuf/repeated_ptr_field.h>
#include <google/protobuf/descriptor.h>
#include <shark/shark.pb.h>

namespace shark {
    class KmgenOption;
    class MessageOption;

    class FieldOption {
    public:
        FieldOption() = default;

        ~FieldOption() = default;

        bool init(const KmgenOption *kmgen_option, const MessageOption *message_option,
                  const ::google::protobuf::FieldDescriptor *field);

        bool process_type();

        bool process_default();

        std::string cpp_type() const;

        std::string cpp_getter_type() const {
            return _kmg_getter_type;
        }

        std::string km_type() const {
            return _km_type;
        }

        std::string name() const {
            return _name;
        }

        bool is_repeated() const {
            return _is_repeated;
        }

        bool is_map() const {
            return _is_map;
        }

        bool is_atomic() const {
            return _option.atomic();
        }

        bool can_be_atomic() const {
            return _can_be_atomic;
        }

        bool is_mutex() const {
            return _option.protected_mutex();
        }

        const ::google::protobuf::FieldDescriptor *descriptor() const {
            return _field;
        }

        std::string get_default_as_string() const {
            return _default_as_string;
        }

        bool need_mutable() const {
            return _need_mutable;
        }

        const std::vector<std::string> &includes() const {
            return _includes;
        }

    private:
        void process_repeated();
        bool process_map();
    private:
        const KmgenOption *_kmgen_option;
        const MessageOption *_message_option;
        const ::google::protobuf::FieldDescriptor *_field;

        shark::option::FieldOptions _option;

        std::string _pb_type;
        std::string _km_type;

        std::string _kmg_getter_type;

        bool _is_repeated{false};

        bool _is_map{false};

        int64_t _default_int;
        double _default_double;
        bool _default_bool;
        std::string _default_string;
        std::string _default_as_string;

        std::string _name;

        std::string _key_type;
        std::string _value_type;
        std::string _map_type;

        std::string _rtype;
        std::vector<std::string> _includes;

        bool _can_be_atomic{false};
        bool _need_mutable{false};
    };
} // namespace shark
