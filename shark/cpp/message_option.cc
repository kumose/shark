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

#include <shark/cpp/km_option.h>
#include <shark/cpp/message_option.h>

namespace shark {

    bool MessageOption::init(const KmgenOption * kmgen_option, const google::protobuf::Descriptor *message, const MessageOption * p) {
        _kmgen_option = kmgen_option;
        _message = message;
        parent = p;

        if (_message->options().HasExtension(shark::km_message)) {
            _option = _message->options().GetExtension(shark::km_message);
        }

        _pb_name = _message->name();
        _km_name = _pb_name;
        if (parent) {
            _km_full_name = parent->km_full_name() + "::" + _km_name;
        } else {
            _km_full_name = _km_name;
        }

        if (!_option.km_message_name().empty()) {
            _km_name = _option.km_message_name();
        }

        _singleton = _option.singleton();
        _strict_pod = _option.strict_pod();

        for (auto i = 0; i < _message->field_count(); ++i) {
            FieldOption fo;
            if (!fo.init(_kmgen_option,this, _message->field(i))) {
                return false;
            }
            _fields.push_back(fo);
        }

        for (auto i = 0; i < _message->nested_type_count(); ++i) {
            auto nested_type = _message->nested_type(i);
            MessageOption nested_option;
            nested_option.init(kmgen_option, nested_type, this);
            _subclasses.push_back(nested_option);
        }
        return true;
    }

    const std::vector<FieldOption> & MessageOption::fields() const {
        return _fields;
    }
}  // namespace shark
