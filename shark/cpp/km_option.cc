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
#include <turbo/strings/str_replace.h>
#include <turbo/strings/str_format.h>
#include <turbo/strings/str_join.h>
#include <turbo/strings/match.h>

namespace shark {

    bool KmgenOption::init(const google::protobuf::FileDescriptor *file) {
        _file = file;
        if (_file->options().HasExtension(shark::km_file)) {
            _opts = _file->options().GetExtension(shark::km_file);
        }
        _pb_ns = turbo::str_replace_all(_file->package(),{{".", "::"}});
        _km_ns = _pb_ns;
        if (!_opts.c_namespace().empty()) {
            _km_ns = _opts.c_namespace();
        }

        for (auto i = 0; i < _opts.c_include_size(); i++) {
            _c_includes.push_back(_opts.c_include(i));
        }
        _repeat_type = "std::vector";
        _c_includes.emplace_back("vector");
        for (auto i = 0; i < _file->message_type_count(); i++) {
            MessageOption mop;
            if (!mop.init(this, _file->message_type(i))) {
                return false;
            }
            _messages.push_back(mop);
        }

        return true;
    }

    const std::string &KmgenOption::pb_namespace() const {
        return _pb_ns;
    }
    const std::string &KmgenOption::km_namespace() const {
        return _km_ns;
    }

    const std::vector<std::string>& KmgenOption::c_includes() const {
        return _c_includes;
    }

    const std::vector<MessageOption> &KmgenOption::messages() const {
        return _messages;
    }

}  // namespace shark
