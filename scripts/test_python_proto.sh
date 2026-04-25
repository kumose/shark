#!/usr/bin/env bash
set -e

# Get the directory where this script resides
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT=${SCRIPT_DIR}/../
PLUGIN=${ROOT}/build/shark/protoc-gen-sk
PROTOC=${ROOT}/build/kmpkg_installed/x64-linux/tools/protobuf/protoc
INS=${ROOT}/shark/idl
cd ${ROOT}

PROTO_FILE="tests/test.proto"

if [ ! -f "$PROTO_FILE" ]; then
    echo "Error: $PROTO_FILE not found in $SCRIPT_DIR"
    exit 1
fi

# Generate Python bindings for the custom options
${PROTOC} --proto_path=. --proto_path="${INS}" --plugin=${PLUGIN} --sk_out=. "$PROTO_FILE"
${PROTOC} --proto_path=. --proto_path="${INS}" --cpp_out=. "$PROTO_FILE"

echo "Successfully generated sk and pb in $SCRIPT_DIR"
