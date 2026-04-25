#!/usr/bin/env bash
set -e

# Get the directory where this script resides
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT=${SCRIPT_DIR}/../
PYDIR=${ROOT}/python/proto_gen_sk
PROTOC=${ROOT}/build/kmpkg_installed/x64-linux/tools/protobuf/protoc
cd ${PYDIR}

PROTO_FILE="shark_options.proto"

if [ ! -f "$PROTO_FILE" ]; then
    echo "Error: $PROTO_FILE not found in $SCRIPT_DIR"
    exit 1
fi

# Generate Python bindings for the custom options
${PROTOC} --proto_path=. --proto_path="$SCRIPT_DIR" --experimental_allow_proto3_optional --python_out=. "$PROTO_FILE"

echo "Successfully generated shark_options_pb2.py in ${PYDIR}"
