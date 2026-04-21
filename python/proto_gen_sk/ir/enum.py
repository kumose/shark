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

# enum.py
from dataclasses import dataclass, field
from typing import List, Tuple, Optional

@dataclass
class Enum:
    """Represents a C++ enum class."""
    name: str
    values: List[Tuple[str, int]]   # (enumerator name, number)
    underlying_type: Optional[str] = None   # e.g., "uint32_t"
    bit_flags: bool = False   # generate bitwise operators
