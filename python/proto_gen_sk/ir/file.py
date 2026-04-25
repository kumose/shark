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

# file.py
from dataclasses import dataclass, field
from typing import List, Optional
from .enum import Enum
from .message import Message

@dataclass
class FileIR:
    """Top-level IR for a .proto file."""
    name: str                       # base name (without .proto)
    package: str                    # proto package name
    runtime_namespace: Optional[str] = None   # C++ namespace override
    cpp_includes: List[str] = field(default_factory=list)
    sk_map_type: str = "std::unordered_map"
    sk_vector: str = "std::vector"
    sk_stand: int = 17              # 17 for C++17, 20 for C++20, 23 for C++23
    edition: int = 2026
    enums: List[Enum] = field(default_factory=list)
    messages: List[Message] = field(default_factory=list)


