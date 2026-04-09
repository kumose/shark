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

#include <shark/cpp/field_option.h>
#include <shark/cpp/message_generator.h>
#include <shark/cpp/km_option.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/str_format.h>
#include <turbo/strings/str_join.h>
#include <turbo/strings/match.h>
#include <turbo/container/flat_hash_map.h>

namespace shark {
    bool FieldOption::init(const KmgenOption *kmgen_option, const MessageOption *message_option,
                           const ::google::protobuf::FieldDescriptor *field) {
        _kmgen_option = kmgen_option;
        _message_option = message_option;
        _field = field;

        _is_repeated = _field->is_repeated();
        _is_map = _field->is_map();

        if (_field->options().HasExtension(shark::km_field)) {
            _option = _field->options().GetExtension(shark::km_field);
        }
        if (!process_type()) {
            return false;
        }

        if (!process_default()) {
            return false;
        }

        _name = _field->name();
        if (_option.atomic()) {
            _includes.push_back("atomic");
        }
        if (_option.protected_mutex()) {
            _includes.push_back("mutex");
        }
        return true;
    }


    static turbo::flat_hash_map<google::protobuf::FieldDescriptor::Type, turbo::flat_hash_map<shark::option::CType,
        std::string> >
    ctype_map =
    {
        {
            google::protobuf::FieldDescriptor::TYPE_INT32, {
                {shark::option::CAUTO, "int32_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_SINT32, {
                {shark::option::CAUTO, "int32_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_SFIXED32, {
                {shark::option::CAUTO, "int32_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_UINT32, {
                {shark::option::CAUTO, "uint32_t"},
                {shark::option::CUINT8, "uint8_t"},
                {shark::option::CUINT16, "uint16_t"},
                {shark::option::CUINT32, "uint32_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_INT64, {
                {shark::option::CAUTO, "int64_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"},
                {shark::option::CINT64, "int64_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_SINT64, {
                {shark::option::CAUTO, "int64_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"},
                {shark::option::CINT64, "int64_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_SFIXED64, {
                {shark::option::CAUTO, "int64_t"},
                {shark::option::CINT8, "int8_t"},
                {shark::option::CINT16, "int16_t"},
                {shark::option::CINT32, "int32_t"},
                {shark::option::CINT64, "int64_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_UINT64, {
                {shark::option::CAUTO, "uint64_t"},
                {shark::option::CUINT8, "uint8_t"},
                {shark::option::CUINT16, "uint16_t"},
                {shark::option::CUINT32, "uint32_t"},
                {shark::option::CUINT64, "uint64_t"}

            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_FLOAT, {
                {shark::option::CAUTO, "float"},
                {shark::option::CFP16, "float"}
            }
        },
        {
            google::protobuf::FieldDescriptor::TYPE_DOUBLE, {
                {shark::option::CAUTO, "double"},
                {shark::option::CFP16, "float"},
                {shark::option::CFP32, "float"},
                {shark::option::CFP64, "double"}
            }
        },
    };

    bool FieldOption::process_type() {
        using google::protobuf::FieldDescriptor;
        if (_is_repeated && !_is_map) {
            process_repeated();
        }
        auto ctype = _option.ctype();


        auto ctype_process = [](shark::option::CType ctype, FieldDescriptor::Type pbt) -> std::string {
            auto it = ctype_map.find(pbt);
            if (it == ctype_map.end()) {
                return "";
            }
            auto cit = it->second.find(ctype);
            if (cit == it->second.end()) {
                return "";
            }
            return cit->second;
        };

        switch (_field->type()) {
            case FieldDescriptor::TYPE_INT32: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_INT32);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_INT64: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_INT64);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_UINT32: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_UINT32);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_UINT64: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_UINT64);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_FLOAT: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_FLOAT);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_DOUBLE: {
                _pb_type = ctype_process(ctype, FieldDescriptor::TYPE_DOUBLE);
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_BOOL: {
                _pb_type = "bool";
                _can_be_atomic = true;
                _kmg_getter_type = _pb_type;
                break;
            }
            case FieldDescriptor::TYPE_STRING: {
                _pb_type = "std::string";
                _can_be_atomic = false;
                _kmg_getter_type = turbo::str_format("const %s&", _pb_type);
                break;
            }

            case FieldDescriptor::TYPE_BYTES: {
                _pb_type = "std::string";
                _can_be_atomic = false;
                _kmg_getter_type = turbo::str_format("const %s&", _pb_type);
                break;
            }
            case FieldDescriptor::TYPE_MESSAGE: {
                if (_is_map) {
                    return process_map();
                }

                _pb_type = turbo::str_format("%s", _field->message_type()->full_name());
                _can_be_atomic = false;
                _kmg_getter_type = turbo::str_format("const %s&", _pb_type);
                _need_mutable = true;
                break;
            }
            case FieldDescriptor::TYPE_ENUM: {
                _pb_type = turbo::str_format("%s", _field->enum_type()->full_name());
                _can_be_atomic = false;
                _kmg_getter_type = _pb_type;
                break;
            }
            default: return false;
        }
        if (_pb_type.empty()) {
            return false;
        }
        /// TODO add c type for this
        _km_type = _pb_type;
        return true;
    }

    bool FieldOption::process_default() {
        _default_as_string = "";
        if (_option.has_default_bool()) {
            _default_bool = _option.default_bool();
            if (_default_bool) {
                _default_as_string = "true";
            } else {
                _default_as_string = "false";
            }
        }
        if (_option.has_default_string()) {
            _default_string = _option.default_string();
            _default_as_string = turbo::str_format("\"%s\"", _default_string);
        }
        if (_option.has_default_double()) {
            _default_double = _option.default_double();
            _default_as_string = std::to_string(_default_double);
        }
        if (_option.has_default_int()) {
            _default_int = _option.default_int();
            _default_as_string = std::to_string(_option.default_int());
        }
        if (_option.has_default_int64()) {
            _default_int = _option.default_int64();
            _default_as_string = std::to_string(_option.default_int64());
        }
        if (_option.has_default_uint32()) {
            _default_int = _option.default_uint32();
            _default_as_string = std::to_string(_option.default_uint32());
        }
        if (_option.has_default_uint64()) {
            _default_int = _option.default_uint64();
            _default_as_string = std::to_string(_option.default_uint64());
        }
        if (_option.has_default_float()) {
            _default_double = _option.default_float();
            _default_as_string = std::to_string(_option.default_float());
        }
        return true;
    }


    std::string FieldOption::cpp_type() const {
        if (_is_map) {
            auto s = turbo::str_format("%s<%s, %s>", _map_type, _key_type, _value_type);
            return s;
        }
        if (_is_repeated) {
            auto s = turbo::str_format("%s<%s>", _rtype, _km_type);
            return s;
        }
        return _km_type;
    }

    void FieldOption::process_repeated() {
        auto cr = _option.crepeat();
        switch (cr) {
            case option::REP_SET:
                _rtype = "std::set";
                _includes.push_back("set");
                break;
            case option::REP_UNORDERED_SET:
                _rtype = "std::unordered_set";
                _includes.push_back("unordered_set");
                break;
            case option::REP_DEQUE:
                _rtype = "std::deque";
                _includes.push_back("deque");
                break;
            case option::REP_VECTOR:
            default:
                _rtype = "std::vector";
                _includes.push_back("vector");
                break;
        }
    }

    bool FieldOption::process_map() {
        auto d = _field->message_type();
        auto kd = d->FindFieldByName("key");
        if (!kd) {
            return false;
        }

        auto vd = d->FindFieldByName("value");
        if (!vd) {
            return false;
        }

        auto mt = _option.cmap();
        switch (mt) {
            case shark::option::MAP_AUTO:
            case shark::option::STD_MAP:
                _map_type = "std::map";
                _includes.push_back("map");
                break;
            case shark::option::STD_UNORDERED:
                _map_type = "std::unordered_map";
                _includes.push_back("unordered_map");
                break;
            case shark::option::BTREE_MAP:
                _map_type = "turbo::btree_map";
                _includes.push_back("turbo/container/btree_map.h");
                break;
            case shark::option::FLAT_MAP:
                _map_type = "turbo::flat_hash_map";
                _includes.push_back("turbo/container/flat_hash_map.h");
                break;
            default:
                return false;
        }

        auto ctype_func = [](const google::protobuf::FieldDescriptor *t) -> std::string {
            auto et = t->type();
            switch (et) {
                case google::protobuf::FieldDescriptor::TYPE_INT32:
                    return "int32_t";
                case google::protobuf::FieldDescriptor::TYPE_INT64:
                    return "int64_t";
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    return "uint32_t";
                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                    return "uint64_t";
                case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                    return "float";
                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                    return "double";
                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                    return "bool";
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    return "std::string";
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    return t->message_type()->full_name();
                case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
                    return "int32_t";
                case google::protobuf::FieldDescriptor::TYPE_FIXED64:
                    return "int64_t";
                case google::protobuf::FieldDescriptor::TYPE_FIXED32:
                    return "int32_t";
                case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
                    return "int64_t";
                default:
                    return "";
            }
        };
        _key_type = ctype_func(kd);
        _value_type = ctype_func(vd);
        if (_key_type.empty() || _value_type.empty()) {
            return false;
        }
        _km_type = turbo::str_format("%s<%s, %s>", _map_type, _key_type, _value_type);
        return true;
    }
} // namespace shark
