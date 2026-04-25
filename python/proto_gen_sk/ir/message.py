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

# message.py
from dataclasses import dataclass, field
from typing import List, Optional
from .field import Field
from .enum import Enum

@dataclass
class Message:
    """Represents a C++ class generated from a protobuf message."""
    name: str
    fields: List[Field] = field(default_factory=list)
    nested_enums: List[Enum] = field(default_factory=list)
    nested_messages: List['Message'] = field(default_factory=list)
    virtual_dtor: bool = False
    private_ctor: bool = False
    is_instance: bool = False   # from shark_message.is_instance
