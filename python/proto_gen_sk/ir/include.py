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

# include.py
from typing import Set, List
from .ctype import CppType

class IncludeManager:
    """Collects and manages #include directives."""
    def __init__(self):
        self._includes: Set[str] = set()

    def add(self, header: str):
        """Add a header, automatically add angle brackets if missing."""
        if not header:
            return
        if header.startswith('<') or header.startswith('"'):
            self._includes.add(header)
        else:
            self._includes.add(f"<{header}>")

    def add_type(self, typ: CppType):
        """Recursively add includes needed by a type."""
        for inc in typ.get_includes():
            self.add(inc)

    def get_includes(self) -> List[str]:
        """Return sorted includes for deterministic output."""
        return sorted(self._includes)

