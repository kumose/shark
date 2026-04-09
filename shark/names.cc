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


#include <string>
#include <shark/names.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/str_format.h>

namespace shark {


    std::string ToNativeName(const std::string& s) { return turbo::str_replace_all(s,{{".", "::"}}); }

    std::string GetInputType(const google::protobuf::MethodDescriptor* method) {
    return "::" + ToNativeName(method->input_type()->full_name());
    }

    std::string GetOutputType(const google::protobuf::MethodDescriptor* method) {
    return "::" + ToNativeName(method->output_type()->full_name());
    }

    std::string GetBasicServiceName(const google::protobuf::ServiceDescriptor* s) {
    return "Basic" + s->name();
    }

    std::string GetBasicStubName(const google::protobuf::ServiceDescriptor* s) {
    return s->name() + "_BasicStub";
    }

    std::string GetSyncServiceName(const google::protobuf::ServiceDescriptor* s) {
    return "Krpc" + s->name();
    }

    std::string GetFullServiceName(const google::protobuf::ServiceDescriptor* s) {
        return "::" + turbo::str_replace_all(s->full_name(), {{".", "::"}});
    }

    std::string GetFullServiceStubName(const google::protobuf::ServiceDescriptor* s) {
        return "::" + turbo::str_replace_all(s->full_name(), {{".", "::"}}) + "_Stub";
    }

    std::string GetSyncStubName(const google::protobuf::ServiceDescriptor* s) {
    return s->name() + "KrpcStub";
    }

    std::string GetAsyncServiceName(const google::protobuf::ServiceDescriptor* s) {
    return "Async" + s->name();
    }

    std::string GetAsyncStubName(const google::protobuf::ServiceDescriptor* s) {
    return s->name() + "_AsyncStub";
    }

    std::string GetGdtCompatibleFutureServiceName(
        const google::protobuf::ServiceDescriptor* s) {
    return s->name() + "Future";
    }

    std::string GetGdtCompatibleFutureStubName(
        const google::protobuf::ServiceDescriptor* s) {
    return s->name() + "Future_Stub";
    }


}  // namespace shark
