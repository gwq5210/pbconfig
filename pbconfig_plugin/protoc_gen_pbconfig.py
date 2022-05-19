#!/usr/bin/python3

from email.policy import default
import json
import logging
import sys
from unicodedata import name
from typing import List

from google.protobuf.compiler import plugin_pb2 as plugin
from google.protobuf.descriptor_pb2 import FileDescriptorProto
from google.protobuf.descriptor_pb2 import DescriptorProto
from google.protobuf.descriptor_pb2 import FieldDescriptorProto


logger = logging.getLogger(__name__)
logging.basicConfig(
    level=logging.DEBUG, format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)

cpp_types = {
    FieldDescriptorProto.TYPE_DOUBLE: 'double',
    FieldDescriptorProto.TYPE_FLOAT: 'float',
    FieldDescriptorProto.TYPE_INT64: 'int64_t',
    FieldDescriptorProto.TYPE_UINT64: 'uint64_t',
    FieldDescriptorProto.TYPE_INT32: 'int32_t',
    FieldDescriptorProto.TYPE_FIXED64: 'uint64_t',
    FieldDescriptorProto.TYPE_FIXED32: 'uint32_t',
    FieldDescriptorProto.TYPE_BOOL: 'bool',
    FieldDescriptorProto.TYPE_STRING: 'std::string',
    FieldDescriptorProto.TYPE_GROUP: 'deprecated_type',
    FieldDescriptorProto.TYPE_MESSAGE: '{message_name}',
    FieldDescriptorProto.TYPE_BYTES: 'std::string',
    FieldDescriptorProto.TYPE_UINT32: 'uint32_t',
    FieldDescriptorProto.TYPE_ENUM: '{enum_name}',
    FieldDescriptorProto.TYPE_SFIXED32: 'int32_t',
    FieldDescriptorProto.TYPE_SFIXED64: 'int64_t',
    FieldDescriptorProto.TYPE_SINT32: 'int32_t',
    FieldDescriptorProto.TYPE_SINT64: 'int64_t',
}

pbconfig_field_types = {
    FieldDescriptorProto.TYPE_DOUBLE: 'kDouble',
    FieldDescriptorProto.TYPE_FLOAT: 'kFloat',
    FieldDescriptorProto.TYPE_INT64: 'kInt64',
    FieldDescriptorProto.TYPE_UINT64: 'kUint64',
    FieldDescriptorProto.TYPE_INT32: 'kInt32',
    FieldDescriptorProto.TYPE_FIXED64: 'kUint64',
    FieldDescriptorProto.TYPE_FIXED32: 'kUint32',
    FieldDescriptorProto.TYPE_BOOL: 'kBool',
    FieldDescriptorProto.TYPE_STRING: 'kString',
    FieldDescriptorProto.TYPE_GROUP: '',
    FieldDescriptorProto.TYPE_MESSAGE: 'kMessage',
    FieldDescriptorProto.TYPE_BYTES: 'kString',
    FieldDescriptorProto.TYPE_UINT32: 'kUint32',
    FieldDescriptorProto.TYPE_ENUM: '',
    FieldDescriptorProto.TYPE_SFIXED32: 'kInt32',
    FieldDescriptorProto.TYPE_SFIXED64: 'kInt64',
    FieldDescriptorProto.TYPE_SINT32: 'kInt32',
    FieldDescriptorProto.TYPE_SINT64: 'kInt64',
}

default_values = {
    FieldDescriptorProto.TYPE_DOUBLE: '0.0',
    FieldDescriptorProto.TYPE_FLOAT: '0.0f',
    FieldDescriptorProto.TYPE_INT64: '0',
    FieldDescriptorProto.TYPE_UINT64: '0',
    FieldDescriptorProto.TYPE_INT32: '0',
    FieldDescriptorProto.TYPE_FIXED64: '0',
    FieldDescriptorProto.TYPE_FIXED32: '0',
    FieldDescriptorProto.TYPE_BOOL: 'false',
    FieldDescriptorProto.TYPE_STRING: '',
    FieldDescriptorProto.TYPE_GROUP: '',
    FieldDescriptorProto.TYPE_MESSAGE: '',
    FieldDescriptorProto.TYPE_BYTES: '',
    FieldDescriptorProto.TYPE_UINT32: '0',
    FieldDescriptorProto.TYPE_ENUM: '',
    FieldDescriptorProto.TYPE_SFIXED32: '0',
    FieldDescriptorProto.TYPE_SFIXED64: '0',
    FieldDescriptorProto.TYPE_SINT32: '0',
    FieldDescriptorProto.TYPE_SINT64: '0',
}

def removesuffix(s, suffix) -> str:
    if s.endswith(suffix):
        return s[:-len(suffix)]
    else:
        return s

class ConfigGenerator():
    def __init__(self, request: plugin.CodeGeneratorRequest):
        self.request = request
        self.response = plugin.CodeGeneratorResponse()


    def get_type_name(self, field_info: FieldDescriptorProto) -> str:
        type_name = ''
        type_name = cpp_types[field_info.type]
        if field_info.type == FieldDescriptorProto.TYPE_MESSAGE:
            type_name = field_info.type_name.replace(".", "::")
        if field_info.label == FieldDescriptorProto.LABEL_REPEATED:
            type_name = f'std::vector<{type_name}>'
        return type_name

    def get_default_value(self, field_info: FieldDescriptorProto) -> str:
        return default_values[field_info.type]

    def generate_field(self, proto_file: FileDescriptorProto, message_info: DescriptorProto, field_info: FieldDescriptorProto) -> str:
        type_name = self.get_type_name(field_info)
        default_value = self.get_default_value(field_info)
        if default_value:
            field_str = f'  {type_name} {field_info.name} = {default_value};'
        else:
            field_str = f'  {type_name} {field_info.name};'
        logger.debug(f'process field: {field_str}')
        return field_str


    def generate_message_definition(self, proto_file: FileDescriptorProto, message_info: DescriptorProto) -> str:
        logger.info(f'generate message definition: {message_info.name}')
        message_content = f'\nstruct {message_info.name} : public pbconfig::PbConfig {{'
        message_content += f'\n  {message_info.name}(): pbconfig::PbConfig("{message_info.name}", "{self.get_message_full_type_name(proto_file, message_info)}") {{}}\n'
        for field_info in message_info.field:
            message_content += '\n' + self.generate_field(proto_file, message_info, field_info)
        message_content += '\n};\n'
        message_content += f'\nextern pbconfig::TypeInfo* {message_info.name}_type_info;\n'
        return message_content

    def generate_common_header(self):
        result = """#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "config.h"
#include "type_info_manager.h"
"""
        return result

    def get_header_filename(self, proto_file: FileDescriptorProto) -> str:
        return removesuffix(proto_file.name, ".proto") + ".cfg.h"


    def get_source_filename(self, proto_file: FileDescriptorProto) -> str:
        return removesuffix(proto_file.name, ".proto") + ".cfg.cc"


    def get_message_full_type_name(self, proto_file: FileDescriptorProto, message_info: DescriptorProto) -> str:
        return proto_file.package.replace('.', '::') + f'::{message_info.name}'


    def get_field_full_type_name(self, field_info: FieldDescriptorProto) -> str:
        return field_info.type_name.replace('.', '::')


    def get_field_type_name(self, field_info: FieldDescriptorProto) -> str:
        return field_info.type_name.split('.')[-1]


    def generate_header_file(self, proto_file: FileDescriptorProto) -> None:
        file_content = self.generate_common_header()
        file_content += self.namespace_begin(proto_file)
        for message_info in proto_file.message_type:
            file_content += self.generate_message_definition(proto_file, message_info)
        file_content += self.namespace_end(proto_file)

        file = self.response.file.add()
        file.name = self.get_header_filename(proto_file)
        logger.info(f"Creating header file: {file.name}")
        file.content = file_content

    def get_pbconfig_field_type(self, field_info: FieldDescriptorProto) -> str:
        return pbconfig_field_types[field_info.type]


    def generate_message_type_info(self, proto_file: FileDescriptorProto, message_info: DescriptorProto) -> str:
        logger.info(f'generate message type info: {message_info.name}')
        message_content = f"""
pbconfig::TypeInfo* {message_info.name}_type_info = nullptr;

pbconfig::TypeInfo* Init{message_info.name}TypeInfo() {{
  if ({message_info.name}_type_info) {{
    return {message_info.name}_type_info;
  }}

  pbconfig::TypeInfo* type_info = new pbconfig::TypeInfo("{message_info.name}", "{self.get_message_full_type_name(proto_file, message_info)}");
"""
        index = 0
        for field_info in message_info.field:
            message_content += f'  type_info->AddFieldInfo("{field_info.name}", pbconfig::FieldInfo::New({index}, "{field_info.name}", pbconfig::FieldType::{self.get_pbconfig_field_type(field_info)}, offsetof({message_info.name}, {field_info.name})'
            if field_info.label == FieldDescriptorProto.LABEL_REPEATED:
                message_content += ', true'
            else:
                message_content += ', false'
            message_content += f', "{self.get_field_type_name(field_info)}"'
            message_content += f', "{self.get_field_full_type_name(field_info)}"));\n'
            index += 1
        message_content += f"""
  {message_info.name}_type_info = type_info;
  return type_info;
}}

REGISTER_TYPE_INFO(Init{message_info.name}TypeInfo);
"""
        return message_content


    def generate_source_file(self, proto_file: FileDescriptorProto) -> None:
        file_content = f'#include "{self.get_header_filename(proto_file)}"\n'
        file_content += self.namespace_begin(proto_file)
        for message_info in proto_file.message_type:
            file_content += self.generate_message_type_info(proto_file, message_info)
        file_content += self.namespace_end(proto_file)

        file = self.response.file.add()
        file.name = self.get_source_filename(proto_file)
        logger.info(f"Creating source file: {file.name}")
        file.content = file_content

    def process_file(self, proto_file: FileDescriptorProto) -> None:
        logger.info(f"Processing proto_file: {proto_file.name}")
        self.generate_header_file(proto_file)
        self.generate_source_file(proto_file)


    def get_namespace_list(self, proto_file: FileDescriptorProto) -> List[str]:
        return proto_file.package.split(".")


    def namespace_begin(self, proto_file: FileDescriptorProto) -> str:
        namespace_list = self.get_namespace_list(proto_file)
        result = ''
        for namespace in namespace_list:
            result += f'\nnamespace {namespace} {{\n'
        return result

    def namespace_end(self, proto_file: FileDescriptorProto) -> str:
        namespace_list = self.get_namespace_list(proto_file)
        namespace_list.reverse()
        result = ''
        for namespace in namespace_list:
            result += f'\n}}  // namespace {namespace}\n'
        return result

    def process(self) -> None:
        for proto_file in self.request.proto_file:
            self.process_file(proto_file)


def main() -> None:
    # Load the request from stdin
    request = plugin.CodeGeneratorRequest.FromString(sys.stdin.buffer.read())

    generator = ConfigGenerator(request)
    generator.process()

    # Serialize response and write to stdout
    sys.stdout.buffer.write(generator.response.SerializeToString())


if __name__ == "__main__":
    main()
