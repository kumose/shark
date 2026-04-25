#!/usr/bin/env python3

import sys
from google.protobuf.compiler import plugin_pb2 as plugin
from google.protobuf.descriptor_pb2 import FileDescriptorProto
from .builder import build_ir
from .generator.header import HeaderGenerator
from .generator.source import SourceGenerator

def main():
    # Read CodeGeneratorRequest from stdin
    data = sys.stdin.buffer.read()
    request = plugin.CodeGeneratorRequest()
    request.ParseFromString(data)

    response = plugin.CodeGeneratorResponse()

    for proto_file in request.proto_file:
        # Build intermediate representation
        ir = build_ir(proto_file)

        # Generate header file
        header_gen = HeaderGenerator(ir)
        header_code = header_gen.generate()
        header_file = response.file.add()
        header_file.name = f"{ir.name}.runtime.h"
        header_file.content = header_code

        # Generate source file
        source_gen = SourceGenerator(ir)
        source_code = source_gen.generate()
        source_file = response.file.add()
        source_file.name = f"{ir.name}.runtime.cc"
        source_file.content = source_code

    # Write response to stdout
    sys.stdout.buffer.write(response.SerializeToString())

if __name__ == "__main__":
    main()

