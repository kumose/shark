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

#include <string>

#include <google/protobuf/descriptor.h>

namespace shark {
        

    // Convert Protocol Buffer's fully qualified name to cpp style.
    std::string ToNativeName(const std::string& s);

    // Get fully qualified type name of method's input type.
    std::string GetInputType(const google::protobuf::MethodDescriptor* method);

    // Get fully qualified type name of method's output type.
    std::string GetOutputType(const google::protobuf::MethodDescriptor* method);

    // Mangles service / stub's name.

    // Basic service is exactly what `cc_generic_service = true` would generates.
    std::string GetBasicServiceName(const google::protobuf::ServiceDescriptor* s);
    std::string GetBasicStubName(const google::protobuf::ServiceDescriptor* s);

    // Async service / stub?

    // Sync service is not inherently performs bad in krpc since we're using fiber
    // anyway.
    //
    // This stub also provides a better interface for calling streaming methods.
    std::string GetSyncServiceName(const google::protobuf::ServiceDescriptor* s);
    std::string GetFullServiceName(const google::protobuf::ServiceDescriptor* s);
    std::string GetFullServiceStubName(const google::protobuf::ServiceDescriptor* s);
    std::string GetSyncStubName(const google::protobuf::ServiceDescriptor* s);

    // `Future`-based interfaces. They can be handy when calling multiple backends
    // simultaneously.
    std::string GetAsyncServiceName(const google::protobuf::ServiceDescriptor* s);
    std::string GetAsyncStubName(const google::protobuf::ServiceDescriptor* s);

    // This one is API compatible with our old plugin (`gdt_future_rpc`).
    std::string GetGdtCompatibleFutureServiceName(
        const google::protobuf::ServiceDescriptor* s);
    std::string GetGdtCompatibleFutureStubName(
        const google::protobuf::ServiceDescriptor* s);

} // namespace shark

