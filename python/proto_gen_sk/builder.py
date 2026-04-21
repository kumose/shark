# Copyright (C) Kumo inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# builder.py
import sys
import re
from google.protobuf.descriptor_pb2 import FileDescriptorProto, FieldDescriptorProto
from .ir import (
    FileIR, Enum, Message, Field,
    PrimitiveType, EnumType, MessageType, ContainerType, AtomicType,
)
from . import shark_options_pb2 as opts


def _validate_container_template(template: str, arity: int) -> None:
    """Validate that template contains exactly `arity` occurrences of '%s'.
    Raises ValueError with a helpful message if validation fails.
    """
    count = template.count('%s')
    if count != arity:
        # Provide an example based on arity
        if arity == 1:
            example = "std::vector<%s>"
        elif arity == 2:
            example = "std::unordered_map<%s, %s>"
        else:
            # Generic example for other arities
            example = "<%s>" + (", %s" * (arity - 1))
        raise ValueError(
            f"Invalid container template: '{template}'. "
            f"It must contain exactly {arity} placeholder(s) '%s'. "
            f"Example: '{example}'"
        )

def build_ir(file_desc: FileDescriptorProto) -> FileIR:
    # Parse file-level options
    file_opts = None
    if file_desc.options.HasExtension(opts.shark_file):
        file_opts = file_desc.options.Extensions[opts.shark_file]

    # Helper to check if an optional field is explicitly set
    def is_set(msg, field_name):
        for field, _ in msg.ListFields():
            if field.name == field_name:
                return True
        return False

    # Determine runtime namespace
    runtime_ns = None
    if file_opts and is_set(file_opts, 'runtime_namespace'):
        runtime_ns = file_opts.runtime_namespace
    else:
        runtime_ns = file_desc.package.replace('.', '::') if file_desc.package else None

    # Inside build_ir, after reading file_opts:
    sk_map_type = "std::unordered_map<%s, %s>"  # fallback, but will be overwritten if user provides valid one
    sk_vector = "std::vector<%s>"              # fallback, but will be overwritten if user provides valid one
    if file_opts:
        if is_set(file_opts, 'sk_map_type'):
            raw = file_opts.sk_map_type
            _validate_container_template(raw, 2)
            sk_map_type = raw
        if is_set(file_opts, 'sk_vector'):
            raw = file_opts.sk_vector
            _validate_container_template(raw, 1)
            sk_vector = raw

    # C++ standard (default 17)
    sk_stand = 17
    if file_opts and is_set(file_opts, 'sk_stand'):
        stand_val = file_opts.sk_stand
        if stand_val == opts.CXX_20:
            sk_stand = 20
        elif stand_val == opts.CXX_23:
            sk_stand = 23
        else:
            sk_stand = 17

    edition = 2026
    if file_opts and is_set(file_opts, 'edition'):
        edition_val = file_opts.edition
        if edition_val == opts.SK_EDITION:
            edition = 2026

    cpp_includes = list(getattr(file_opts, 'cpp_includes', [])) if file_opts else []

    ir = FileIR(
        name=file_desc.name.replace('.proto', ''),
        package=file_desc.package,
        runtime_namespace=runtime_ns,
        cpp_includes=cpp_includes,
        sk_map_type=sk_map_type,
        sk_vector=sk_vector,
        sk_stand=sk_stand,
        edition=edition,
    )

    # Helper: convert protobuf field type to C++ type (ignoring repeated/map)
    def to_primitive_type(field_desc: FieldDescriptorProto) -> str:
        type_map = {
            1: "double",
            2: "float",
            3: "int64_t",
            4: "uint64_t",
            5: "int32_t",
            6: "uint64_t",  # fixed64
            7: "int32_t",   # fixed32
            8: "bool",
            9: "std::string",
            12: "std::string",  # bytes
            13: "uint32_t",
            14: "uint32_t",      # enum (placeholder)
            15: "int32_t",       # sfixed32
            16: "int64_t",       # sfixed64
            17: "int32_t",       # sint32
            18: "int64_t",       # sint64
        }
        if field_desc.type in type_map:
            return type_map[field_desc.type]
        if field_desc.type == 11:  # TYPE_MESSAGE
            return field_desc.type_name.split('.')[-1]
        if field_desc.type == 14:  # TYPE_ENUM
            return field_desc.type_name.split('.')[-1]
        return "void"

    def create_type_from_field(field_desc: FieldDescriptorProto, field_opts, file_opts) -> "CppType":
        # Check for map (type_name ends with 'Entry')
        is_map = (field_desc.type == 11 and field_desc.type_name.endswith('Entry'))
        is_repeated = (field_desc.label == 3) and not is_map

        # Determine the base type (before wrapping)
        if field_desc.type == 14:  # enum
            base_type = EnumType(field_desc.type_name.split('.')[-1])
        elif field_desc.type == 11:  # message
            if is_map:
                # Map types are handled separately in build_message; return placeholder
                base_type = PrimitiveType("void")
            else:
                base_type = MessageType(field_desc.type_name.split('.')[-1])
        else:
            type_name = to_primitive_type(field_desc)
            base_type = PrimitiveType(type_name)

        # Wrap repeated fields in a container
        if is_repeated:
            container = "std::vector"
            if file_opts and hasattr(file_opts, 'sk_vector') and file_opts.HasField('sk_vector'):
                container = file_opts.sk_vector
            base_type = ContainerType(container, [base_type])

        # Map handling is deferred to build_message, but here we keep placeholder
        if is_map:
            return PrimitiveType("void")  # placeholder

        # Wrap with atomic if requested
        if field_opts and field_opts.is_atomic:
            base_type = AtomicType(base_type)

        return base_type

    def build_enum(enum_desc):
        enum_opts = None
        if enum_desc.options.HasExtension(opts.shark_enum):
            enum_opts = enum_desc.options.Extensions[opts.shark_enum]
        underlying = None
        if enum_opts and enum_opts.HasField('underlying_type'):
            ut = enum_opts.underlying_type
            if ut == opts.ENUM_UNDERLYING_U8:
                underlying = "uint8_t"
            elif ut == opts.ENUM_UNDERLYING_U16:
                underlying = "uint16_t"
            elif ut == opts.ENUM_UNDERLYING_U32:
                underlying = "uint32_t"
            elif ut == opts.ENUM_UNDERLYING_U64:
                underlying = "uint64_t"
        bit_flags = enum_opts.bit_flags if enum_opts else False
        values = [(v.name, v.number) for v in enum_desc.value]
        return Enum(name=enum_desc.name, values=values, underlying_type=underlying, bit_flags=bit_flags)

    def build_message(msg_desc):
        msg_opts = None
        if msg_desc.options.HasExtension(opts.shark_message):
            msg_opts = msg_desc.options.Extensions[opts.shark_message]
        virtual_dtor = msg_opts.virtual_dtor if msg_opts and msg_opts.HasField('virtual_dtor') else False
        private_ctor = msg_opts.private_ctor if msg_opts and msg_opts.HasField('private_ctor') else False
        is_instance = msg_opts.is_instance if msg_opts and msg_opts.HasField('is_instance') else False

        msg = Message(name=msg_desc.name, virtual_dtor=virtual_dtor, private_ctor=private_ctor, is_instance=is_instance)

        # Process fields
        for field_desc in msg_desc.field:
            field_opts = None
            if field_desc.options.HasExtension(opts.shark_field):
                field_opts = field_desc.options.Extensions[opts.shark_field]

            # Check if this field is a map (message type with map_entry option)
            is_map = False
            if field_desc.type == 11:
                # Find the nested message definition
                nested = None
                for nested_candidate in msg_desc.nested_type:
                    if nested_candidate.name == field_desc.type_name.split('.')[-1]:
                        nested = nested_candidate
                        break
                if nested and nested.options.map_entry:
                    is_map = True

            if is_map:
                # Extract key and value types from the map entry message
                key_desc = nested.field[0]
                value_desc = nested.field[1]
                key_type = create_type_from_field(key_desc, None, file_opts)
                value_type = create_type_from_field(value_desc, None, file_opts)
                container = "std::unordered_map"
                if file_opts and hasattr(file_opts, 'sk_map_type') and file_opts.HasField('sk_map_type'):
                    container = file_opts.sk_map_type
                cpp_type = ContainerType(container, [key_type, value_type])
                if field_opts and field_opts.is_atomic:
                    cpp_type = AtomicType(cpp_type)
                f = Field(name=field_desc.name, type=cpp_type, is_repeated=False, is_map=True)
                msg.fields.append(f)
                continue

            # Normal or repeated field
            base_type = create_type_from_field(field_desc, field_opts, file_opts)
            is_repeated = (field_desc.label == 3)
            f = Field(name=field_desc.name, type=base_type, is_repeated=is_repeated, is_map=False)
            msg.fields.append(f)

        # Process nested enums and messages
        for enum_desc in msg_desc.enum_type:
            msg.nested_enums.append(build_enum(enum_desc))
        for nested_desc in msg_desc.nested_type:
            # Skip map entry messages
            if nested_desc.options.map_entry:
                continue
            msg.nested_messages.append(build_message(nested_desc))
        return msg

    # Build top-level enums and messages
    for enum_desc in file_desc.enum_type:
        ir.enums.append(build_enum(enum_desc))
    for msg_desc in file_desc.message_type:
        ir.messages.append(build_message(msg_desc))

    return ir