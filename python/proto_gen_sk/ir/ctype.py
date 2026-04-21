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
#

# ctype.py
from dataclasses import dataclass
from typing import List, Optional

class CppType:
    """Base class for all C++ types."""
    def get_cpp_type(self) -> str:
        raise NotImplementedError
    def get_includes(self) -> List[str]:
        return []

@dataclass
class PrimitiveType(CppType):
    """Primitive built-in type (int32_t, bool, std::string, etc.)"""
    name: str
    def get_cpp_type(self) -> str:
        return self.name
    def get_includes(self) -> List[str]:
        if self.name == "std::string":
            return ["<string>"]
        return []

@dataclass
class EnumType(CppType):
    """Enum type defined in the same generated header."""
    name: str
    def get_cpp_type(self) -> str:
        return self.name

@dataclass
class MessageType(CppType):
    """Nested or top-level message type."""
    name: str
    def get_cpp_type(self) -> str:
        return self.name

# ctype.py (partial)
@dataclass
class ContainerType(CppType):
    """
    Represents a C++ container type (e.g., vector, map, custom containers).
    The `container` string may contain '%s' placeholders that will be replaced
    by the comma-separated list of element type strings.
    Example: "std::vector<%s>" or "absl::flat_hash_map<%s, %s>"
    """
    container: str                # Container template pattern, may contain %s
    element_types: List[CppType]  # One element for vector, two for map

    def get_cpp_type(self) -> str:
        # Build the inner types string
        inner = ', '.join(t.get_cpp_type() for t in self.element_types)
        # Replace all occurrences of '%s' with the inner types string
        return self.container.replace('%s', inner)

    def get_includes(self) -> List[str]:
        includes = []
        # Try to infer include headers based on container name
        if self.container.startswith('std::vector'):
            includes.append("<vector>")
        elif self.container.startswith('std::unordered_map'):
            includes.append("<unordered_map>")
        elif self.container.startswith('absl::'):
            # Extract the container name, e.g., "absl::flat_hash_map" -> "flat_hash_map"
            container_name = self.container.split('::')[-1].split('<')[0]
            includes.append(f"<absl/container/{container_name}.h>")
        # For other containers, user must provide includes via cpp_includes
        for elem in self.element_types:
            includes.extend(elem.get_includes())
        return includes

@dataclass
class AtomicType(CppType):
    """std::atomic wrapper around another type."""
    wrapped: CppType
    def get_cpp_type(self) -> str:
        return f"std::atomic<{self.wrapped.get_cpp_type()}>"
    def get_includes(self) -> List[str]:
        return ["<atomic>"] + self.wrapped.get_includes()

@dataclass
class UserDefinedType(CppType):
    """User-defined type via ctype option (e.g., custom string class)."""
    name: str
    includes: Optional[List[str]] = None
    def get_cpp_type(self) -> str:
        return self.name
    def get_includes(self) -> List[str]:
        return self.includes or []

