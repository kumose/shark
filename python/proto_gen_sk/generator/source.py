# source.py
from ..ir import FileIR, Message, Field
from .formatter import CodeFormatter

class SourceGenerator:
    def __init__(self, ir: FileIR):
        self.ir = ir
        self.fmt = CodeFormatter()

    def generate(self) -> str:
        header_name = f"{self.ir.name}.runtime.h"
        self.fmt.write_line(f'#include "{header_name}"')
        self.fmt.newline()

        ns = self.ir.runtime_namespace or self.ir.package.replace('.', '::')
        if ns:
            parts = ns.split('::')
            for part in parts:
                self.fmt.write_line(f"namespace {part} {{")
                self.fmt.indent()
            self.fmt.newline()

        for msg in self.ir.messages:
            self._generate_message_impls(msg, "")

        if ns:
            for part in reversed(parts):
                self.fmt.unindent()
                self.fmt.write_line(f"}}  // namespace {part}")
            self.fmt.newline()

        return self.fmt.str()

    def _generate_message_impls(self, msg: Message, outer_scope: str):
        class_scope = f"{outer_scope}::{msg.name}" if outer_scope else msg.name
        for field in msg.fields:
            self._generate_getter_impl(class_scope, field)
            self._generate_setter_impl(class_scope, field)
        self._generate_from_proto(class_scope, msg)
        self._generate_to_proto(class_scope, msg)
        for nested in msg.nested_messages:
            self._generate_message_impls(nested, class_scope)

    def _generate_getter_impl(self, class_scope: str, field: Field):
        typ = field.type.get_cpp_type()
        if field.is_repeated or field.is_map:
            ret = f"const {typ}&"
        elif field.type.__class__.__name__ in ("PrimitiveType", "EnumType"):
            if typ == "std::string":
                ret = "const std::string&"
            else:
                ret = typ
        else:
            ret = f"const {typ}&"
        self.fmt.write_line(f"{ret} {class_scope}::{field.name}() const {{")
        self.fmt.indent()
        self.fmt.write_line(f"return {field.name}_;")
        self.fmt.unindent()
        self.fmt.write_line("}")
        self.fmt.newline()

    def _generate_setter_impl(self, class_scope: str, field: Field):
        if field.is_repeated or field.is_map:
            return
        typ = field.type.get_cpp_type()
        if typ == "std::string":
            param = "const std::string&"
        elif typ in ("int32_t", "int64_t", "uint32_t", "uint64_t", "float", "double", "bool"):
            param = typ
        else:
            param = f"const {typ}&"
        self.fmt.write_line(f"void {class_scope}::set_{field.name}({param} value) {{")
        self.fmt.indent()
        self.fmt.write_line(f"{field.name}_ = value;")
        self.fmt.unindent()
        self.fmt.write_line("}")
        self.fmt.newline()

    def _generate_from_proto(self, class_scope: str, msg: Message):
        self.fmt.write_line(f"bool {class_scope}::FromProto(const google::protobuf::Message& proto) {{")
        self.fmt.indent()
        # 假设 protobuf 消息类型与类名相同（需根据实际情况调整）
        proto_type = msg.name
        self.fmt.write_line(f"const auto& msg = static_cast<const {proto_type}&>(proto);")
        for field in msg.fields:
            self._write_fromproto_field(field)
        self.fmt.write_line("return true;")
        self.fmt.unindent()
        self.fmt.write_line("}")
        self.fmt.newline()

    def _generate_to_proto(self, class_scope: str, msg: Message):
        self.fmt.write_line(f"bool {class_scope}::ToProto(google::protobuf::Message* proto) const {{")
        self.fmt.indent()
        proto_type = msg.name
        self.fmt.write_line(f"auto* msg = static_cast<{proto_type}*>(proto);")
        for field in msg.fields:
            self._write_topProto_field(field)
        self.fmt.write_line("return true;")
        self.fmt.unindent()
        self.fmt.write_line("}")
        self.fmt.newline()

    def _write_fromproto_field(self, field: Field):
        # Simplified: just assign scalar and string; for containers and nested, need more logic
        if field.is_map:
            self.fmt.write_line(f"// TODO: map field {field.name}")
        elif field.is_repeated:
            self.fmt.write_line(f"// TODO: repeated field {field.name}")
        else:
            typ = field.type.get_cpp_type()
            if typ == "std::string":
                self.fmt.write_line(f"{field.name}_ = msg.{field.name}();")
            elif typ in ("int32_t", "int64_t", "uint32_t", "uint64_t", "float", "double", "bool"):
                self.fmt.write_line(f"{field.name}_ = msg.{field.name}();")
            else:
                # Enum or message
                self.fmt.write_line(f"{field.name}_ = static_cast<{typ}>(msg.{field.name}());")

    def _write_topProto_field(self, field: Field):
        if field.is_map:
            self.fmt.write_line(f"// TODO: map field {field.name}")
        elif field.is_repeated:
            self.fmt.write_line(f"// TODO: repeated field {field.name}")
        else:
            typ = field.type.get_cpp_type()
            if typ == "std::string":
                self.fmt.write_line(f"msg->set_{field.name}({field.name}_);")
            elif typ in ("int32_t", "int64_t", "uint32_t", "uint64_t", "float", "double", "bool"):
                self.fmt.write_line(f"msg->set_{field.name}({field.name}_);")
            else:
                self.fmt.write_line(f"msg->set_{field.name}(static_cast<int32_t>({field.name}_));")

