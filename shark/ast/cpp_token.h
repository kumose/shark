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

#include <typesafe/reference.hpp>

#include <shark/ast/cppast_fwd.h>

namespace cppast {
    /// The kinds of C++ tokens.
    enum class CppTokenKind {
        identifier, //< Any identifier.
        keyword, //< Any keyword.
        int_literal, //< An integer literal.
        float_literal, //< A floating point literal.
        char_literal, //< A character literal.
        string_literal, //< A string literal.
        punctuation //< Any other punctuation.
    };

    /// A C++ token.
    struct CppToken {
        std::string spelling;
        CppTokenKind kind;

        CppToken(CppTokenKind kind, std::string spelling) : spelling(std::move(spelling)), kind(kind) {
        }

        friend bool operator==(const CppToken &lhs, const CppToken &rhs) noexcept {
            return lhs.spelling == rhs.spelling;
        }

        friend bool operator!=(const CppToken &lhs, const CppToken &rhs) noexcept {
            return !(rhs == lhs);
        }
    };

    /// A combination of multiple C++ tokens.
    class CppTokenString {
    public:
        /// Builds a token string.
        class builder {
        public:
            builder() = default;

            /// \effects Adds a token.
            void add_token(CppToken tok) {
                tokens_.push_back(std::move(tok));
            }

            /// \effects Converts a trailing `>>` to `>` token.
            void unmunch();

            /// \returns The finished string.
            CppTokenString finish() {
                return CppTokenString(std::move(tokens_));
            }

        private:
            std::vector<CppToken> tokens_;
        };

        /// Tokenizes a string.
        /// \effects Splits the string into C++ tokens.
        /// The string must contain valid tokens and must already be preprocessed (i.e. translation
        /// phase 6 is already done). \returns The tokenized string.
        static CppTokenString tokenize(std::string str);

        /// \effects Creates it from a sequence of tokens.
        CppTokenString(std::vector<CppToken> tokens) : tokens_(std::move(tokens)) {
        }

        /// \exclude target
        using iterator = std::vector<CppToken>::const_iterator;

        /// \returns An iterator to the first token.
        iterator begin() const noexcept {
            return tokens_.begin();
        }

        /// \returns An iterator one past the last token.
        iterator end() const noexcept {
            return tokens_.end();
        }

        /// \returns Whether or not the string is empty.
        bool empty() const noexcept {
            return tokens_.empty();
        }

        /// \returns A reference to the first token.
        const CppToken &front() const noexcept {
            return tokens_.front();
        }

        /// \returns A reference to the last token.
        const CppToken &back() const noexcept {
            return tokens_.back();
        }

        /// \returns The string representation of the tokens, without any whitespace.
        std::string as_string() const;

    private:
        std::vector<CppToken> tokens_;

        friend bool operator==(const CppTokenString &lhs, const CppTokenString &rhs);
    };

    bool operator==(const CppTokenString &lhs, const CppTokenString &rhs);

    inline bool operator!=(const CppTokenString &lhs, const CppTokenString &rhs) {
        return !(lhs == rhs);
    }
} // namespace cppast
