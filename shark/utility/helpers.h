// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
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
//



#pragma once


#include <string>
#include <vector>
#include <sstream>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_format.h>
#include <turbo/strings/match.h>
#include <shark/utility/compat.h>
#include <shark/type.h>

namespace shark {

    std::string CEscape(compat::StringView src);


    inline std::string StripSuffixString(compat::StringView str, compat::StringView suffix) {
        if (turbo::ends_with(str, suffix)) { return std::string(str.substr(0, str.size() - suffix.size())); } else {
            return std::string(str);
        }
    }

    char *FastHexToBuffer(int i, char *buffer);



    // Returns the scope where the field was defined (for extensions, this is
    // different from the message type to which the field applies).
    inline const google::protobuf::Descriptor *FieldScope(const google::protobuf::FieldDescriptor *field) {
        return field->is_extension() ? field->extension_scope() : field->containing_type();
    }

    // convert a CamelCase class name into an all uppercase affair
    // with underscores separating words, e.g. MyClass becomes MY_CLASS.
    std::string CamelToUpper(compat::StringView class_name);

    std::string CamelToLower(compat::StringView class_name);

    // lowercased, underscored name to camel case
    std::string ToCamel(compat::StringView name);

    // full_name() to lowercase with underscores
    std::string FullNameToLower(compat::StringView full_name, const google::protobuf::FileDescriptor *file);

    std::string FullNameToUpper(compat::StringView full_name, const google::protobuf::FileDescriptor *file);

    // full_name() to c-typename (with underscores for packages, otherwise camel case)
    std::string FullNameToC(compat::StringView class_name, const google::protobuf::FileDescriptor *file);

    // Splits, indents, formats, and prints comment lines
    void PrintComment(google::protobuf::io::Printer *printer, std::string comment);

    // make a string of spaces as long as input
    std::string ConvertToSpaces(compat::StringView input);

    // Strips ".proto" or ".protodevel" from the end of a filename.
    std::string StripProto(compat::StringView filename);

    // Get the C++ type name for a primitive type (e.g. "double", "::google::protobuf::int32", etc.).
    // Note:  non-built-in type names will be qualified, meaning they will start
    // with a ::.  If you are using the type as a template parameter, you will
    // need to insure there is a space between the < and the ::, because the
    // ridiculous C++ standard defines "<:" to be a synonym for "[".
    const char *PrimitiveTypeName(google::protobuf::FieldDescriptor::CppType type);

    // Get the declared type name in CamelCase format, as is used e.g. for the
    // methods of WireFormat.  For example, TYPE_INT32 becomes "Int32".
    const char *DeclaredTypeMethodName(google::protobuf::FieldDescriptor::Type type);

    // Convert a file name into a valid identifier.
    std::string FilenameIdentifier(compat::StringView filename);

    // return 'required', 'optional', or 'repeated'
    std::string GetLabelName(google::protobuf::FieldDescriptor::Label label);

    // write IntRanges entries for a bunch of sorted values.
    // returns the number of ranges there are to bsearch.
    unsigned WriteIntRanges(google::protobuf::io::Printer *printer, int n_values, const int *values,
                            compat::StringView name);

    struct NameIndex {
        unsigned index;
        compat::StringView name;
    };

    int compare_name_indices_by_name(const void *, const void *);

    // Return the syntax version of the file containing the field.
    inline int FieldSyntax(const google::protobuf::FieldDescriptor *field) {
        auto proto = google::protobuf::FileDescriptorProto();
        field->file()->CopyTo(&proto);

        if (proto.has_syntax()) {
            auto syntax = proto.syntax();
            assert(syntax == "proto2" || syntax == "proto3");
            if (syntax == "proto2") {
                return 2;
            } else if (syntax == "proto3") {
                return 3;
            }
        }

        return 2;
    }
} // namespace shark
