#pragma once

#include <shark/ast/code_generator.h>
#include <sstream>
#include <string>

namespace shark::generator {

    class StringCodeGenerator : public cppast::CodeGenerator {
    public:
        std::string str() const { return oss_.str(); }

    protected:
        void do_indent() override { indent_level_++; }
        void do_unindent() override { indent_level_--; }
        void do_write_newline() override {
            oss_ << '\n';
            for (int i = 0; i < indent_level_; ++i) oss_ << "    ";
        }
        void do_write_whitespace() override { oss_ << ' '; }
        void do_write_token_seq(cppast::string_view tokens) override {
            oss_ << tokens.c_str();
        }

    private:
        std::ostringstream oss_;
        int indent_level_ = 0;
    };

} // namespace shark::generator