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


#include <cstdint>
#include <memory>
#include <set>
#include <vector>

#include <float.h>
#include <stdio.h>		// for snprintf

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>

#include <shark/utility/helpers.h>
#include <shark/utility/compat.h>
#include <turbo/strings/str_split.h>
#include <turbo/strings/str_replace.h>

namespace shark {
#if defined(_MSC_VER)
    // FIXME: In the case where the generated string is longer than the buffer,
    // _snprint() returns a negative value, where snprintf() returns the number
    // of characters that *would* have been stored, had there been room.
    // That is fundamental, as it allows snprintf() to be used to find the size
    // necessary for the buffer, simply by calling it with the size of the buffer
    // passed in as zero.
    // Note: at the present moment return value isn't used in the code.
#define snprintf _snprintf
#pragma warning(disable:4800)
#pragma warning(disable:4996)
#endif


    std::string CamelToUpper(compat::StringView name) {
        bool was_upper = true; // suppress initial _
        std::string rv = "";
        int len = name.length();
        for (int i = 0; i < len; i++) {
            bool is_upper = isupper(name[i]);
            if (is_upper) {
                if (!was_upper)
                    rv += '_';
                rv += name[i];
            } else {
                rv += toupper(name[i]);
            }
            was_upper = is_upper;
        }
        return rv;
    }

    std::string CamelToLower(compat::StringView name) {
        bool was_upper = true; // suppress initial _
        std::string rv = "";
        int len = name.length();
        for (int i = 0; i < len; i++) {
            bool is_upper = isupper(name[i]);
            if (is_upper) {
                if (!was_upper)
                    rv += '_';
                rv += tolower(name[i]);
            } else {
                rv += name[i];
            }
            was_upper = is_upper;
        }
        return rv;
    }


    std::string ToCamel(compat::StringView name) {
        std::string rv = "";
        int len = name.length();
        bool next_is_upper = true;
        for (int i = 0; i < len; i++) {
            if (name[i] == '_') {
                next_is_upper = true;
            } else if (next_is_upper) {
                rv += toupper(name[i]);
                next_is_upper = false;
            } else {
                rv += name[i];
            }
        }
        return rv;
    }

    std::string OverrideFullName(compat::StringView full_name, const google::protobuf::FileDescriptor *file) {
        const idl::SharkFileOptions opt = file->options().GetExtension(idl::shark_file);
        if (!opt.has_c_package())
            return std::string(full_name);

        std::string new_name = opt.c_package();
        if (file->package().empty())
            new_name += ".";

        return new_name + std::string(full_name.substr(file->package().length()));
    }

    std::string FullNameToLower(compat::StringView full_name, const google::protobuf::FileDescriptor *file) {
        std::vector<std::string> pieces = turbo::str_split(full_name, ".");
        std::string rv = "";
        for (unsigned i = 0; i < pieces.size(); i++) {
            if (pieces[i] == "") continue;
            if (rv != "") rv += "__";
            rv += CamelToLower(pieces[i]);
        }
        return rv;
    }

    std::string FullNameToUpper(compat::StringView full_name, const google::protobuf::FileDescriptor *file) {
        std::vector<std::string> pieces = turbo::str_split(full_name, ".");
        std::string rv = "";
        for (unsigned i = 0; i < pieces.size(); i++) {
            if (pieces[i] == "") continue;
            if (rv != "") rv += "__";
            rv += CamelToUpper(pieces[i]);
        }
        return rv;
    }

    std::string FullNameToC(compat::StringView full_name, const google::protobuf::FileDescriptor *file) {
        std::vector<std::string> pieces = turbo::str_split(full_name, ".");
        std::string rv = "";
        for (unsigned i = 0; i < pieces.size(); i++) {
            if (pieces[i] == "") continue;
            if (rv != "") rv += "__";
            rv += ToCamel(pieces[i]);
        }
        return rv;
    }

    void PrintComment(google::protobuf::io::Printer *printer, std::string comment) {
        if (comment.empty()) {
            return ;
        }

        std::vector<std::string> comment_lines = turbo::str_split(comment, turbo::ByAnyChar("\r\n"), turbo::SkipEmpty());

        for (size_t i = 0; i < comment_lines.size(); i++) {
            if (comment_lines[i].empty()) {
                continue;
            }
            /* Make sure we don't inadvertently close the comment block */
            while (turbo::starts_with(comment_lines[i], "///"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "///");

            while (turbo::starts_with(comment_lines[i], "//"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "//");
            while (turbo::starts_with(comment_lines[i], "/"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "/");

            while (turbo::ends_with(comment_lines[i], "\n"))
                comment_lines[i] = turbo::strip_suffix(comment_lines[i], "\n");
            while (turbo::ends_with(comment_lines[i], "\r"))
                comment_lines[i] = turbo::strip_suffix(comment_lines[i], "\r");


            size_t delim_i;
            while ((delim_i = comment_lines[i].find("/*")) != std::string::npos)
                comment_lines[i][delim_i] = ' ';
            while ((delim_i = comment_lines[i].find("*/")) != std::string::npos)
                comment_lines[i][delim_i + 1] = ' ';
            auto trimd = turbo::trim_all(comment_lines[i]);
            printer->Print("/// $comment$\n", "comment", trimd);
        }
    }

    void print_toml_comment(google::protobuf::io::Printer *printer, std::string comment) {
        if (comment.empty()) {
            return ;
        }

        std::vector<std::string> comment_lines = turbo::str_split(comment, turbo::ByAnyChar("\r\n"), turbo::SkipEmpty());

        for (size_t i = 0; i < comment_lines.size(); i++) {
            if (comment_lines[i].empty()) {
                continue;
            }
            /* Make sure we don't inadvertently close the comment block */
            while (turbo::starts_with(comment_lines[i], "///"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "///");

            while (turbo::starts_with(comment_lines[i], "//"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "//");
            while (turbo::starts_with(comment_lines[i], "/"))
                comment_lines[i] = turbo::strip_prefix(comment_lines[i], "/");

            while (turbo::ends_with(comment_lines[i], "\n"))
                comment_lines[i] = turbo::strip_suffix(comment_lines[i], "\n");
            while (turbo::ends_with(comment_lines[i], "\r"))
                comment_lines[i] = turbo::strip_suffix(comment_lines[i], "\r");


            size_t delim_i;
            while ((delim_i = comment_lines[i].find("/*")) != std::string::npos)
                comment_lines[i][delim_i] = ' ';
            while ((delim_i = comment_lines[i].find("*/")) != std::string::npos)
                comment_lines[i][delim_i + 1] = ' ';
            auto trimd = turbo::trim_all(comment_lines[i]);
            printer->Print("\"# $comment$\\n\"\n", "comment", trimd);
        }
    }

    std::string ConvertToSpaces(compat::StringView input) {
        return std::string(input.size(), ' ');
    }

    int compare_name_indices_by_name(const void *a, const void *b) {
        const NameIndex *ni_a = (const NameIndex *) a;
        const NameIndex *ni_b = (const NameIndex *) b;
        return ni_a->name.compare(ni_b->name);
    }

    std::string CEscape(compat::StringView src);


    std::string StripProto(compat::StringView filename) {
        if (turbo::ends_with(filename, ".protodevel")) {
            return StripSuffixString(filename, ".protodevel");
        } else {
            return StripSuffixString(filename, ".proto");
        }
    }

    // Convert a file name into a valid identifier.
    std::string FilenameIdentifier(compat::StringView filename) {
        std::string result;
        for (unsigned i = 0; i < filename.size(); i++) {
            if (isalnum(filename[i])) {
                result.push_back(filename[i]);
            } else {
                // Not alphanumeric.  To avoid any possibility of name conflicts we
                // use the hex code for the character.
                result.push_back('_');
                char buffer[32];
                result.append(FastHexToBuffer(static_cast<std::uint8_t>(filename[i]), buffer));
            }
        }
        return result;
    }

    std::string GetLabelName(google::protobuf::FieldDescriptor::Label label) {
        switch (label) {
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL: return "optional";
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED: return "required";
            case google::protobuf::FieldDescriptor::LABEL_REPEATED: return "repeated";
        }
        return "bad-label";
    }

    unsigned
    WriteIntRanges(google::protobuf::io::Printer *printer, int n_values, const int *values, compat::StringView name) {
        std::map<std::string, std::string> vars;
        vars["name"] = std::string(name);
        if (n_values > 0) {
            int n_ranges = 1;
            for (int i = 1; i < n_values; i++) {
                if (values[i - 1] + 1 != values[i])
                    n_ranges++;
            }
            vars["n_ranges"] = turbo::str_cat(n_ranges);
            printer->Print(vars, "static const ProtobufCIntRange $name$[$n_ranges$ + 1] =\n"
                           "{\n");
            int last_range_start = 0;
            for (int i = 1; i < n_values; i++) {
                if (values[i - 1] + 1 != values[i]) {
                    int count = i - last_range_start;
                    int expected = values[i - 1] + 1;
                    vars["start_value"] = turbo::str_cat(expected - count);
                    vars["orig_offset"] = turbo::str_cat(last_range_start);
                    printer->Print(vars, "  { $start_value$, $orig_offset$ },\n");
                    last_range_start = i;
                }
            }
            // write last real entry
            {
                int i = n_values;
                int count = i - last_range_start;
                int expected = values[i - 1] + 1;
                vars["start_value"] = turbo::str_cat(expected - count);
                vars["orig_offset"] = turbo::str_cat(last_range_start);
                printer->Print(vars, "  { $start_value$, $orig_offset$ },\n");
            }
            // write sentinel entry
            vars["n_entries"] = turbo::str_cat(n_values);
            printer->Print(vars, "  { 0, $n_entries$ }\n");
            printer->Print(vars, "};\n");
            return n_ranges;
        } else {
            printer->Print(vars, "#define $name$ NULL\n");
            return 0;
        }
    }

    // ----------------------------------------------------------------------
    // SplitStringUsing()
    //    Split a string using a character delimiter. Append the components
    //    to 'result'.
    //
    // Note: For multi-character delimiters, this routine will split on *ANY* of
    // the characters in the string, not the entire string as a single delimiter.
    // ----------------------------------------------------------------------
    template<typename ITR>
    static inline
    void SplitStringToIteratorUsing(compat::StringView full,
                                    const char *delim,
                                    ITR &result) {
        // Optimize the common case where delim is a single character.
        if (delim[0] != '\0' && delim[1] == '\0') {
            char c = delim[0];
            const char *p = full.data();
            const char *end = p + full.size();
            while (p != end) {
                if (*p == c) {
                    ++p;
                } else {
                    const char *start = p;
                    while (++p != end && *p != c);
                    *result++ = std::string(start, p - start);
                }
            }
            return;
        }

        std::string::size_type begin_index, end_index;
        begin_index = full.find_first_not_of(delim);
        while (begin_index != std::string::npos) {
            end_index = full.find_first_of(delim, begin_index);
            if (end_index == std::string::npos) {
                *result++ = std::string(full.substr(begin_index));
                return;
            }
            *result++ = std::string(full.substr(begin_index, (end_index - begin_index)));
            begin_index = full.find_first_not_of(delim, end_index);
        }
    }

    void SplitStringUsing(compat::StringView full,
                          const char *delim,
                          std::vector<std::string> *result) {
        std::back_insert_iterator<std::vector<std::string> > it(*result);
        SplitStringToIteratorUsing(full, delim, it);
    }

    char *FastHexToBuffer(int i, char *buffer) {
        snprintf(buffer, 16, "%x", i);
        return buffer;
    }

    static int CEscapeInternal(const char *src, int src_len, char *dest,
                               int dest_len, bool use_hex) {
        const char *src_end = src + src_len;
        int used = 0;
        bool last_hex_escape = false; // true if last output char was \xNN

        for (; src < src_end; src++) {
            if (dest_len - used < 2) // Need space for two letter escape
                return -1;

            bool is_hex_escape = false;
            switch (*src) {
                case '\n': dest[used++] = '\\';
                    dest[used++] = 'n';
                    break;
                case '\r': dest[used++] = '\\';
                    dest[used++] = 'r';
                    break;
                case '\t': dest[used++] = '\\';
                    dest[used++] = 't';
                    break;
                case '\"': dest[used++] = '\\';
                    dest[used++] = '\"';
                    break;
                case '\'': dest[used++] = '\\';
                    dest[used++] = '\'';
                    break;
                case '\\': dest[used++] = '\\';
                    dest[used++] = '\\';
                    break;
                default:
                    // Note that if we emit \xNN and the src character after that is a hex
                    // digit then that digit must be escaped too to prevent it being
                    // interpreted as part of the character code by C.
                    if (!isprint(*src) || (last_hex_escape && isxdigit(*src))) {
                        if (dest_len - used < 4) // need space for 4 letter escape
                            return -1;
                        snprintf(dest + used, dest_len - used, (use_hex ? "\\x%02x" : "\\%03o"),
                                 static_cast<std::uint8_t>(*src));
                        is_hex_escape = use_hex;
                        used += 4;
                    } else {
                        dest[used++] = *src;
                        break;
                    }
            }
            last_hex_escape = is_hex_escape;
        }

        if (dest_len - used < 1) // make sure that there is room for \0
            return -1;

        dest[used] = '\0'; // doesn't count towards return value though
        return used;
    }

    std::string CEscape(compat::StringView src) {
        const int dest_length = src.size() * 4 + 1; // Maximum possible expansion
        std::unique_ptr<char[]> dest(new char[dest_length]);
        const int len = CEscapeInternal(src.data(), src.size(),
                                        dest.get(), dest_length, false);
        GOOGLE_DCHECK_GE(len, 0);
        return std::string(dest.get(), len);
    }
} // namespace shark
