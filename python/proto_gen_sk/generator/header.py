# header.py
from typing import List
from ..ir import FileIR, Enum, Message, Field
from ..ir.ctype import CppType, PrimitiveType, EnumType, MessageType, ContainerType, AtomicType
from .formatter import CodeFormatter

class HeaderGenerator:
    def __init__(self, ir: FileIR):
        self.ir = ir
        self.fmt = CodeFormatter()

    def generate(self) -> str:
        self._write_pragma_once()
        self._write_includes()
        self.fmt.newline()

        # Handle namespace
        ns = self.ir.runtime_namespace or self.ir.package.replace('.', '::')
        if ns:
            parts = ns.split('::')
            for part in parts:
                self.fmt.write_line(f"namespace {part} {{")
                self.fmt.indent()
            self.fmt.newline()

        # Generate enums and messages
        for enum in self.ir.enums:
            self._generate_enum(enum)
        for msg in self.ir.messages:
            self._generate_message(msg)

        # Close namespaces
        if ns:
            for part in reversed(parts):
                self.fmt.unindent()
                self.fmt.write_line(f"}}  // namespace {part}")
            self.fmt.newline()

        return self.fmt.str()

    def _write_pragma_once(self):
        self.fmt.write_line("#pragma once")

    def _write_includes(self):
        # Add system includes from file options
        for inc in self.ir.cpp_includes:
            if inc.startswith('<') or inc.startswith('"'):
                self.fmt.write_line(f"#include {inc}")
            else:
                self.fmt.write_line(f"#include <{inc}>")
        # Add required standard includes (can be extended)
        needed = set()
        # Collect includes from types (simplified: add common ones)
        # In real implementation, we would traverse all types and call type.get_includes()
        needed.add("<cstdint>")
        needed.add("<string>")
        # We'll add them unconditionally for now; later we can optimize.
        for inc in sorted(needed):
            self.fmt.write_line(f"#include {inc}")

    def _generate_enum(self, enum: Enum):
        underlying = enum.underlying_type or "uint32_t"
        self.fmt.write_line(f"enum class {enum.name} : {underlying} {{")
        self.fmt.indent()
        for name, value in enum.values:
            self.fmt.write_line(f"{name} = {value},")
        self.fmt.unindent()
        self.fmt.write_line("};")
        self.fmt.newline()
        if enum.bit_flags:
            self._generate_bit_ops(enum.name)

    def _generate_bit_ops(self, enum_name: str):
        self.fmt.write_line(f"inline {enum_name} operator|({enum_name} lhs, {enum_name} rhs) {{")
        self.fmt.indent()
        self.fmt.write_line(f"using U = std::underlying_type_t<{enum_name}>;")
        self.fmt.write_line(f"return static_cast<{enum_name}>(static_cast<U>(lhs) | static_cast<U>(rhs));")
        self.fmt.unindent()
        self.fmt.write_line("}")
        # Similarly for &, ^, ~ (omitted for brevity)
        self.fmt.newline()

    def _generate_message(self, msg: Message, indent: int = 0):
        self.fmt.write_line(f"class {msg.name} {{")
        self.fmt.indent()
        # First generate nested enums and messages (so they are defined before use)
        for nested_enum in msg.nested_enums:
            self._generate_enum(nested_enum)
        for nested_msg in msg.nested_messages:
            self._generate_message(nested_msg)
        # Public section
        self.fmt.write_line("public:")
        self.fmt.indent()
        # Member variables (private, but we put them in private section for simplicity)
        # Actually, we need a private section before public. For simplicity, we'll put them in private.
        # Let's restructure: private members first, then public.
        # Better: write private section explicitly.
        # I'll adjust: after nested types, write private: then members, then public: getters/setters.
        # For clarity, we'll use two separate sections.
        # For now, keep it simple: put members in private (default) and getters/setters in public.
        # So we need to add a private: label.
        self.fmt.unindent()  # go back to class level
        self.fmt.write_line("private:")
        self.fmt.indent()
        for field in msg.fields:
            self._write_member(field)
        self.fmt.unindent()
        self.fmt.write_line("public:")
        self.fmt.indent()
        for field in msg.fields:
            self._write_getter(field)
            self._write_setter(field)
        self.fmt.write_line("bool FromProto(const google::protobuf::Message& proto);")
        self.fmt.write_line("bool ToProto(google::protobuf::Message* proto) const;")
        self.fmt.unindent()
        self.fmt.unindent()
        self.fmt.write_line("};")
        self.fmt.newline()

    def _write_member(self, field: Field):
        typ = field.type.get_cpp_type()
        self.fmt.write_line(f"{typ} {field.name}_;")

    def _write_getter(self, field: Field):
        typ = field.type.get_cpp_type()
        # For containers, return const&; for scalars, return by value; for strings and messages, return const&
        if field.is_repeated or field.is_map:
            ret = f"const {typ}&"
        elif field.type.__class__.__name__ in ("PrimitiveType", "EnumType"):
            if typ == "std::string":
                ret = "const std::string&"
            else:
                ret = typ
        else:
            ret = f"const {typ}&"
        self.fmt.write_line(f"{ret} {field.name}() const;")

    def _write_setter(self, field: Field):
        if field.is_repeated or field.is_map:
            # For repeated and map, we often provide add_xxx, mutable_xxx, etc. Not generating now.
            return
        typ = field.type.get_cpp_type()
        if typ == "std::string":
            param = "const std::string&"
        elif typ in ("int32_t", "int64_t", "uint32_t", "uint64_t", "float", "double", "bool"):
            param = typ
        else:
            param = f"const {typ}&"
        self.fmt.write_line(f"void set_{field.name}({param} value);")

