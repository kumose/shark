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


# formatter.py
from typing import List, Optional

class CodeFormatter:
    """Handles indentation, newlines, and whitespace for code generation."""
    def __init__(self, indent_size: int = 4):
        self._lines: List[str] = []
        self._indent_level: int = 0
        self._indent_size: int = indent_size
        self._needs_indent: bool = True

    def indent(self):
        """Increase indentation level."""
        self._indent_level += 1

    def unindent(self):
        """Decrease indentation level."""
        if self._indent_level > 0:
            self._indent_level -= 1

    def _write_indent(self):
        if self._needs_indent:
            self._lines.append(' ' * (self._indent_level * self._indent_size))
            self._needs_indent = False

    def write(self, text: str = ""):
        """Write text at current position (no newline)."""
        self._write_indent()
        self._lines.append(text)

    def write_line(self, line: str = ""):
        """Write a full line, adding newline and resetting indent flag."""
        self._write_indent()
        self._lines.append(line + '\n')
        self._needs_indent = True

    def newline(self):
        """Add an empty line."""
        self._lines.append('\n')
        self._needs_indent = True

    def str(self) -> str:
        """Return the accumulated code as a string."""
        return ''.join(self._lines)
