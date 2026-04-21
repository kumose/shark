#include "google/protobuf/descriptor.runtime.h"

namespace google {
    namespace protobuf {

        const std::vector& FileDescriptorSet::file() const {
            return file_;
        }

        bool FileDescriptorSet::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FileDescriptorSet&>(proto);
            // TODO: repeated field file
            return true;
        }

        bool FileDescriptorSet::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FileDescriptorSet*>(proto);
            // TODO: repeated field file
            return true;
        }

        const std::string& FileDescriptorProto::name() const {
            return name_;
        }

        void FileDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const std::string& FileDescriptorProto::package() const {
            return package_;
        }

        void FileDescriptorProto::set_package(const std::string& value) {
            package_ = value;
        }

        const std::vector& FileDescriptorProto::dependency() const {
            return dependency_;
        }

        const std::vector& FileDescriptorProto::public_dependency() const {
            return public_dependency_;
        }

        const std::vector& FileDescriptorProto::weak_dependency() const {
            return weak_dependency_;
        }

        const std::vector& FileDescriptorProto::message_type() const {
            return message_type_;
        }

        const std::vector& FileDescriptorProto::enum_type() const {
            return enum_type_;
        }

        const std::vector& FileDescriptorProto::service() const {
            return service_;
        }

        const std::vector& FileDescriptorProto::extension() const {
            return extension_;
        }

        const FileOptions& FileDescriptorProto::options() const {
            return options_;
        }

        void FileDescriptorProto::set_options(const FileOptions& value) {
            options_ = value;
        }

        const SourceCodeInfo& FileDescriptorProto::source_code_info() const {
            return source_code_info_;
        }

        void FileDescriptorProto::set_source_code_info(const SourceCodeInfo& value) {
            source_code_info_ = value;
        }

        const std::string& FileDescriptorProto::syntax() const {
            return syntax_;
        }

        void FileDescriptorProto::set_syntax(const std::string& value) {
            syntax_ = value;
        }

        Edition FileDescriptorProto::edition() const {
            return edition_;
        }

        void FileDescriptorProto::set_edition(const Edition& value) {
            edition_ = value;
        }

        bool FileDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FileDescriptorProto&>(proto);
            name_ = msg.name();
            package_ = msg.package();
            // TODO: repeated field dependency
            // TODO: repeated field public_dependency
            // TODO: repeated field weak_dependency
            // TODO: repeated field message_type
            // TODO: repeated field enum_type
            // TODO: repeated field service
            // TODO: repeated field extension
            options_ = static_cast<FileOptions>(msg.options());
            source_code_info_ = static_cast<SourceCodeInfo>(msg.source_code_info());
            syntax_ = msg.syntax();
            edition_ = static_cast<Edition>(msg.edition());
            return true;
        }

        bool FileDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FileDescriptorProto*>(proto);
            msg->set_name(name_);
            msg->set_package(package_);
            // TODO: repeated field dependency
            // TODO: repeated field public_dependency
            // TODO: repeated field weak_dependency
            // TODO: repeated field message_type
            // TODO: repeated field enum_type
            // TODO: repeated field service
            // TODO: repeated field extension
            msg->set_options(static_cast<int32_t>(options_));
            msg->set_source_code_info(static_cast<int32_t>(source_code_info_));
            msg->set_syntax(syntax_);
            msg->set_edition(static_cast<int32_t>(edition_));
            return true;
        }

        const std::string& DescriptorProto::name() const {
            return name_;
        }

        void DescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const std::vector& DescriptorProto::field() const {
            return field_;
        }

        const std::vector& DescriptorProto::extension() const {
            return extension_;
        }

        const std::vector& DescriptorProto::nested_type() const {
            return nested_type_;
        }

        const std::vector& DescriptorProto::enum_type() const {
            return enum_type_;
        }

        const std::vector& DescriptorProto::extension_range() const {
            return extension_range_;
        }

        const std::vector& DescriptorProto::oneof_decl() const {
            return oneof_decl_;
        }

        const MessageOptions& DescriptorProto::options() const {
            return options_;
        }

        void DescriptorProto::set_options(const MessageOptions& value) {
            options_ = value;
        }

        const std::vector& DescriptorProto::reserved_range() const {
            return reserved_range_;
        }

        const std::vector& DescriptorProto::reserved_name() const {
            return reserved_name_;
        }

        bool DescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const DescriptorProto&>(proto);
            name_ = msg.name();
            // TODO: repeated field field
            // TODO: repeated field extension
            // TODO: repeated field nested_type
            // TODO: repeated field enum_type
            // TODO: repeated field extension_range
            // TODO: repeated field oneof_decl
            options_ = static_cast<MessageOptions>(msg.options());
            // TODO: repeated field reserved_range
            // TODO: repeated field reserved_name
            return true;
        }

        bool DescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<DescriptorProto*>(proto);
            msg->set_name(name_);
            // TODO: repeated field field
            // TODO: repeated field extension
            // TODO: repeated field nested_type
            // TODO: repeated field enum_type
            // TODO: repeated field extension_range
            // TODO: repeated field oneof_decl
            msg->set_options(static_cast<int32_t>(options_));
            // TODO: repeated field reserved_range
            // TODO: repeated field reserved_name
            return true;
        }

        int32_t DescriptorProto::ExtensionRange::start() const {
            return start_;
        }

        void DescriptorProto::ExtensionRange::set_start(int32_t value) {
            start_ = value;
        }

        int32_t DescriptorProto::ExtensionRange::end() const {
            return end_;
        }

        void DescriptorProto::ExtensionRange::set_end(int32_t value) {
            end_ = value;
        }

        const ExtensionRangeOptions& DescriptorProto::ExtensionRange::options() const {
            return options_;
        }

        void DescriptorProto::ExtensionRange::set_options(const ExtensionRangeOptions& value) {
            options_ = value;
        }

        bool DescriptorProto::ExtensionRange::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const ExtensionRange&>(proto);
            start_ = msg.start();
            end_ = msg.end();
            options_ = static_cast<ExtensionRangeOptions>(msg.options());
            return true;
        }

        bool DescriptorProto::ExtensionRange::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<ExtensionRange*>(proto);
            msg->set_start(start_);
            msg->set_end(end_);
            msg->set_options(static_cast<int32_t>(options_));
            return true;
        }

        int32_t DescriptorProto::ReservedRange::start() const {
            return start_;
        }

        void DescriptorProto::ReservedRange::set_start(int32_t value) {
            start_ = value;
        }

        int32_t DescriptorProto::ReservedRange::end() const {
            return end_;
        }

        void DescriptorProto::ReservedRange::set_end(int32_t value) {
            end_ = value;
        }

        bool DescriptorProto::ReservedRange::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const ReservedRange&>(proto);
            start_ = msg.start();
            end_ = msg.end();
            return true;
        }

        bool DescriptorProto::ReservedRange::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<ReservedRange*>(proto);
            msg->set_start(start_);
            msg->set_end(end_);
            return true;
        }

        const std::vector& ExtensionRangeOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        const std::vector& ExtensionRangeOptions::declaration() const {
            return declaration_;
        }

        const FeatureSet& ExtensionRangeOptions::features() const {
            return features_;
        }

        void ExtensionRangeOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        VerificationState ExtensionRangeOptions::verification() const {
            return verification_;
        }

        void ExtensionRangeOptions::set_verification(const VerificationState& value) {
            verification_ = value;
        }

        bool ExtensionRangeOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const ExtensionRangeOptions&>(proto);
            // TODO: repeated field uninterpreted_option
            // TODO: repeated field declaration
            features_ = static_cast<FeatureSet>(msg.features());
            verification_ = static_cast<VerificationState>(msg.verification());
            return true;
        }

        bool ExtensionRangeOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<ExtensionRangeOptions*>(proto);
            // TODO: repeated field uninterpreted_option
            // TODO: repeated field declaration
            msg->set_features(static_cast<int32_t>(features_));
            msg->set_verification(static_cast<int32_t>(verification_));
            return true;
        }

        int32_t ExtensionRangeOptions::Declaration::number() const {
            return number_;
        }

        void ExtensionRangeOptions::Declaration::set_number(int32_t value) {
            number_ = value;
        }

        const std::string& ExtensionRangeOptions::Declaration::full_name() const {
            return full_name_;
        }

        void ExtensionRangeOptions::Declaration::set_full_name(const std::string& value) {
            full_name_ = value;
        }

        const std::string& ExtensionRangeOptions::Declaration::type() const {
            return type_;
        }

        void ExtensionRangeOptions::Declaration::set_type(const std::string& value) {
            type_ = value;
        }

        bool ExtensionRangeOptions::Declaration::reserved() const {
            return reserved_;
        }

        void ExtensionRangeOptions::Declaration::set_reserved(bool value) {
            reserved_ = value;
        }

        bool ExtensionRangeOptions::Declaration::repeated() const {
            return repeated_;
        }

        void ExtensionRangeOptions::Declaration::set_repeated(bool value) {
            repeated_ = value;
        }

        bool ExtensionRangeOptions::Declaration::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const Declaration&>(proto);
            number_ = msg.number();
            full_name_ = msg.full_name();
            type_ = msg.type();
            reserved_ = msg.reserved();
            repeated_ = msg.repeated();
            return true;
        }

        bool ExtensionRangeOptions::Declaration::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<Declaration*>(proto);
            msg->set_number(number_);
            msg->set_full_name(full_name_);
            msg->set_type(type_);
            msg->set_reserved(reserved_);
            msg->set_repeated(repeated_);
            return true;
        }

        const std::string& FieldDescriptorProto::name() const {
            return name_;
        }

        void FieldDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        int32_t FieldDescriptorProto::number() const {
            return number_;
        }

        void FieldDescriptorProto::set_number(int32_t value) {
            number_ = value;
        }

        Label FieldDescriptorProto::label() const {
            return label_;
        }

        void FieldDescriptorProto::set_label(const Label& value) {
            label_ = value;
        }

        Type FieldDescriptorProto::type() const {
            return type_;
        }

        void FieldDescriptorProto::set_type(const Type& value) {
            type_ = value;
        }

        const std::string& FieldDescriptorProto::type_name() const {
            return type_name_;
        }

        void FieldDescriptorProto::set_type_name(const std::string& value) {
            type_name_ = value;
        }

        const std::string& FieldDescriptorProto::extendee() const {
            return extendee_;
        }

        void FieldDescriptorProto::set_extendee(const std::string& value) {
            extendee_ = value;
        }

        const std::string& FieldDescriptorProto::default_value() const {
            return default_value_;
        }

        void FieldDescriptorProto::set_default_value(const std::string& value) {
            default_value_ = value;
        }

        int32_t FieldDescriptorProto::oneof_index() const {
            return oneof_index_;
        }

        void FieldDescriptorProto::set_oneof_index(int32_t value) {
            oneof_index_ = value;
        }

        const std::string& FieldDescriptorProto::json_name() const {
            return json_name_;
        }

        void FieldDescriptorProto::set_json_name(const std::string& value) {
            json_name_ = value;
        }

        const FieldOptions& FieldDescriptorProto::options() const {
            return options_;
        }

        void FieldDescriptorProto::set_options(const FieldOptions& value) {
            options_ = value;
        }

        bool FieldDescriptorProto::proto3_optional() const {
            return proto3_optional_;
        }

        void FieldDescriptorProto::set_proto3_optional(bool value) {
            proto3_optional_ = value;
        }

        bool FieldDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FieldDescriptorProto&>(proto);
            name_ = msg.name();
            number_ = msg.number();
            label_ = static_cast<Label>(msg.label());
            type_ = static_cast<Type>(msg.type());
            type_name_ = msg.type_name();
            extendee_ = msg.extendee();
            default_value_ = msg.default_value();
            oneof_index_ = msg.oneof_index();
            json_name_ = msg.json_name();
            options_ = static_cast<FieldOptions>(msg.options());
            proto3_optional_ = msg.proto3_optional();
            return true;
        }

        bool FieldDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FieldDescriptorProto*>(proto);
            msg->set_name(name_);
            msg->set_number(number_);
            msg->set_label(static_cast<int32_t>(label_));
            msg->set_type(static_cast<int32_t>(type_));
            msg->set_type_name(type_name_);
            msg->set_extendee(extendee_);
            msg->set_default_value(default_value_);
            msg->set_oneof_index(oneof_index_);
            msg->set_json_name(json_name_);
            msg->set_options(static_cast<int32_t>(options_));
            msg->set_proto3_optional(proto3_optional_);
            return true;
        }

        const std::string& OneofDescriptorProto::name() const {
            return name_;
        }

        void OneofDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const OneofOptions& OneofDescriptorProto::options() const {
            return options_;
        }

        void OneofDescriptorProto::set_options(const OneofOptions& value) {
            options_ = value;
        }

        bool OneofDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const OneofDescriptorProto&>(proto);
            name_ = msg.name();
            options_ = static_cast<OneofOptions>(msg.options());
            return true;
        }

        bool OneofDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<OneofDescriptorProto*>(proto);
            msg->set_name(name_);
            msg->set_options(static_cast<int32_t>(options_));
            return true;
        }

        const std::string& EnumDescriptorProto::name() const {
            return name_;
        }

        void EnumDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const std::vector& EnumDescriptorProto::value() const {
            return value_;
        }

        const EnumOptions& EnumDescriptorProto::options() const {
            return options_;
        }

        void EnumDescriptorProto::set_options(const EnumOptions& value) {
            options_ = value;
        }

        const std::vector& EnumDescriptorProto::reserved_range() const {
            return reserved_range_;
        }

        const std::vector& EnumDescriptorProto::reserved_name() const {
            return reserved_name_;
        }

        bool EnumDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EnumDescriptorProto&>(proto);
            name_ = msg.name();
            // TODO: repeated field value
            options_ = static_cast<EnumOptions>(msg.options());
            // TODO: repeated field reserved_range
            // TODO: repeated field reserved_name
            return true;
        }

        bool EnumDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EnumDescriptorProto*>(proto);
            msg->set_name(name_);
            // TODO: repeated field value
            msg->set_options(static_cast<int32_t>(options_));
            // TODO: repeated field reserved_range
            // TODO: repeated field reserved_name
            return true;
        }

        int32_t EnumDescriptorProto::EnumReservedRange::start() const {
            return start_;
        }

        void EnumDescriptorProto::EnumReservedRange::set_start(int32_t value) {
            start_ = value;
        }

        int32_t EnumDescriptorProto::EnumReservedRange::end() const {
            return end_;
        }

        void EnumDescriptorProto::EnumReservedRange::set_end(int32_t value) {
            end_ = value;
        }

        bool EnumDescriptorProto::EnumReservedRange::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EnumReservedRange&>(proto);
            start_ = msg.start();
            end_ = msg.end();
            return true;
        }

        bool EnumDescriptorProto::EnumReservedRange::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EnumReservedRange*>(proto);
            msg->set_start(start_);
            msg->set_end(end_);
            return true;
        }

        const std::string& EnumValueDescriptorProto::name() const {
            return name_;
        }

        void EnumValueDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        int32_t EnumValueDescriptorProto::number() const {
            return number_;
        }

        void EnumValueDescriptorProto::set_number(int32_t value) {
            number_ = value;
        }

        const EnumValueOptions& EnumValueDescriptorProto::options() const {
            return options_;
        }

        void EnumValueDescriptorProto::set_options(const EnumValueOptions& value) {
            options_ = value;
        }

        bool EnumValueDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EnumValueDescriptorProto&>(proto);
            name_ = msg.name();
            number_ = msg.number();
            options_ = static_cast<EnumValueOptions>(msg.options());
            return true;
        }

        bool EnumValueDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EnumValueDescriptorProto*>(proto);
            msg->set_name(name_);
            msg->set_number(number_);
            msg->set_options(static_cast<int32_t>(options_));
            return true;
        }

        const std::string& ServiceDescriptorProto::name() const {
            return name_;
        }

        void ServiceDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const std::vector& ServiceDescriptorProto::method() const {
            return method_;
        }

        const ServiceOptions& ServiceDescriptorProto::options() const {
            return options_;
        }

        void ServiceDescriptorProto::set_options(const ServiceOptions& value) {
            options_ = value;
        }

        bool ServiceDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const ServiceDescriptorProto&>(proto);
            name_ = msg.name();
            // TODO: repeated field method
            options_ = static_cast<ServiceOptions>(msg.options());
            return true;
        }

        bool ServiceDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<ServiceDescriptorProto*>(proto);
            msg->set_name(name_);
            // TODO: repeated field method
            msg->set_options(static_cast<int32_t>(options_));
            return true;
        }

        const std::string& MethodDescriptorProto::name() const {
            return name_;
        }

        void MethodDescriptorProto::set_name(const std::string& value) {
            name_ = value;
        }

        const std::string& MethodDescriptorProto::input_type() const {
            return input_type_;
        }

        void MethodDescriptorProto::set_input_type(const std::string& value) {
            input_type_ = value;
        }

        const std::string& MethodDescriptorProto::output_type() const {
            return output_type_;
        }

        void MethodDescriptorProto::set_output_type(const std::string& value) {
            output_type_ = value;
        }

        const MethodOptions& MethodDescriptorProto::options() const {
            return options_;
        }

        void MethodDescriptorProto::set_options(const MethodOptions& value) {
            options_ = value;
        }

        bool MethodDescriptorProto::client_streaming() const {
            return client_streaming_;
        }

        void MethodDescriptorProto::set_client_streaming(bool value) {
            client_streaming_ = value;
        }

        bool MethodDescriptorProto::server_streaming() const {
            return server_streaming_;
        }

        void MethodDescriptorProto::set_server_streaming(bool value) {
            server_streaming_ = value;
        }

        bool MethodDescriptorProto::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const MethodDescriptorProto&>(proto);
            name_ = msg.name();
            input_type_ = msg.input_type();
            output_type_ = msg.output_type();
            options_ = static_cast<MethodOptions>(msg.options());
            client_streaming_ = msg.client_streaming();
            server_streaming_ = msg.server_streaming();
            return true;
        }

        bool MethodDescriptorProto::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<MethodDescriptorProto*>(proto);
            msg->set_name(name_);
            msg->set_input_type(input_type_);
            msg->set_output_type(output_type_);
            msg->set_options(static_cast<int32_t>(options_));
            msg->set_client_streaming(client_streaming_);
            msg->set_server_streaming(server_streaming_);
            return true;
        }

        const std::string& FileOptions::java_package() const {
            return java_package_;
        }

        void FileOptions::set_java_package(const std::string& value) {
            java_package_ = value;
        }

        const std::string& FileOptions::java_outer_classname() const {
            return java_outer_classname_;
        }

        void FileOptions::set_java_outer_classname(const std::string& value) {
            java_outer_classname_ = value;
        }

        bool FileOptions::java_multiple_files() const {
            return java_multiple_files_;
        }

        void FileOptions::set_java_multiple_files(bool value) {
            java_multiple_files_ = value;
        }

        bool FileOptions::java_generate_equals_and_hash() const {
            return java_generate_equals_and_hash_;
        }

        void FileOptions::set_java_generate_equals_and_hash(bool value) {
            java_generate_equals_and_hash_ = value;
        }

        bool FileOptions::java_string_check_utf8() const {
            return java_string_check_utf8_;
        }

        void FileOptions::set_java_string_check_utf8(bool value) {
            java_string_check_utf8_ = value;
        }

        OptimizeMode FileOptions::optimize_for() const {
            return optimize_for_;
        }

        void FileOptions::set_optimize_for(const OptimizeMode& value) {
            optimize_for_ = value;
        }

        const std::string& FileOptions::go_package() const {
            return go_package_;
        }

        void FileOptions::set_go_package(const std::string& value) {
            go_package_ = value;
        }

        bool FileOptions::cc_generic_services() const {
            return cc_generic_services_;
        }

        void FileOptions::set_cc_generic_services(bool value) {
            cc_generic_services_ = value;
        }

        bool FileOptions::java_generic_services() const {
            return java_generic_services_;
        }

        void FileOptions::set_java_generic_services(bool value) {
            java_generic_services_ = value;
        }

        bool FileOptions::py_generic_services() const {
            return py_generic_services_;
        }

        void FileOptions::set_py_generic_services(bool value) {
            py_generic_services_ = value;
        }

        bool FileOptions::deprecated() const {
            return deprecated_;
        }

        void FileOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        bool FileOptions::cc_enable_arenas() const {
            return cc_enable_arenas_;
        }

        void FileOptions::set_cc_enable_arenas(bool value) {
            cc_enable_arenas_ = value;
        }

        const std::string& FileOptions::objc_class_prefix() const {
            return objc_class_prefix_;
        }

        void FileOptions::set_objc_class_prefix(const std::string& value) {
            objc_class_prefix_ = value;
        }

        const std::string& FileOptions::csharp_namespace() const {
            return csharp_namespace_;
        }

        void FileOptions::set_csharp_namespace(const std::string& value) {
            csharp_namespace_ = value;
        }

        const std::string& FileOptions::swift_prefix() const {
            return swift_prefix_;
        }

        void FileOptions::set_swift_prefix(const std::string& value) {
            swift_prefix_ = value;
        }

        const std::string& FileOptions::php_class_prefix() const {
            return php_class_prefix_;
        }

        void FileOptions::set_php_class_prefix(const std::string& value) {
            php_class_prefix_ = value;
        }

        const std::string& FileOptions::php_namespace() const {
            return php_namespace_;
        }

        void FileOptions::set_php_namespace(const std::string& value) {
            php_namespace_ = value;
        }

        const std::string& FileOptions::php_metadata_namespace() const {
            return php_metadata_namespace_;
        }

        void FileOptions::set_php_metadata_namespace(const std::string& value) {
            php_metadata_namespace_ = value;
        }

        const std::string& FileOptions::ruby_package() const {
            return ruby_package_;
        }

        void FileOptions::set_ruby_package(const std::string& value) {
            ruby_package_ = value;
        }

        const FeatureSet& FileOptions::features() const {
            return features_;
        }

        void FileOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const std::vector& FileOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool FileOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FileOptions&>(proto);
            java_package_ = msg.java_package();
            java_outer_classname_ = msg.java_outer_classname();
            java_multiple_files_ = msg.java_multiple_files();
            java_generate_equals_and_hash_ = msg.java_generate_equals_and_hash();
            java_string_check_utf8_ = msg.java_string_check_utf8();
            optimize_for_ = static_cast<OptimizeMode>(msg.optimize_for());
            go_package_ = msg.go_package();
            cc_generic_services_ = msg.cc_generic_services();
            java_generic_services_ = msg.java_generic_services();
            py_generic_services_ = msg.py_generic_services();
            deprecated_ = msg.deprecated();
            cc_enable_arenas_ = msg.cc_enable_arenas();
            objc_class_prefix_ = msg.objc_class_prefix();
            csharp_namespace_ = msg.csharp_namespace();
            swift_prefix_ = msg.swift_prefix();
            php_class_prefix_ = msg.php_class_prefix();
            php_namespace_ = msg.php_namespace();
            php_metadata_namespace_ = msg.php_metadata_namespace();
            ruby_package_ = msg.ruby_package();
            features_ = static_cast<FeatureSet>(msg.features());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool FileOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FileOptions*>(proto);
            msg->set_java_package(java_package_);
            msg->set_java_outer_classname(java_outer_classname_);
            msg->set_java_multiple_files(java_multiple_files_);
            msg->set_java_generate_equals_and_hash(java_generate_equals_and_hash_);
            msg->set_java_string_check_utf8(java_string_check_utf8_);
            msg->set_optimize_for(static_cast<int32_t>(optimize_for_));
            msg->set_go_package(go_package_);
            msg->set_cc_generic_services(cc_generic_services_);
            msg->set_java_generic_services(java_generic_services_);
            msg->set_py_generic_services(py_generic_services_);
            msg->set_deprecated(deprecated_);
            msg->set_cc_enable_arenas(cc_enable_arenas_);
            msg->set_objc_class_prefix(objc_class_prefix_);
            msg->set_csharp_namespace(csharp_namespace_);
            msg->set_swift_prefix(swift_prefix_);
            msg->set_php_class_prefix(php_class_prefix_);
            msg->set_php_namespace(php_namespace_);
            msg->set_php_metadata_namespace(php_metadata_namespace_);
            msg->set_ruby_package(ruby_package_);
            msg->set_features(static_cast<int32_t>(features_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool MessageOptions::message_set_wire_format() const {
            return message_set_wire_format_;
        }

        void MessageOptions::set_message_set_wire_format(bool value) {
            message_set_wire_format_ = value;
        }

        bool MessageOptions::no_standard_descriptor_accessor() const {
            return no_standard_descriptor_accessor_;
        }

        void MessageOptions::set_no_standard_descriptor_accessor(bool value) {
            no_standard_descriptor_accessor_ = value;
        }

        bool MessageOptions::deprecated() const {
            return deprecated_;
        }

        void MessageOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        bool MessageOptions::map_entry() const {
            return map_entry_;
        }

        void MessageOptions::set_map_entry(bool value) {
            map_entry_ = value;
        }

        bool MessageOptions::deprecated_legacy_json_field_conflicts() const {
            return deprecated_legacy_json_field_conflicts_;
        }

        void MessageOptions::set_deprecated_legacy_json_field_conflicts(bool value) {
            deprecated_legacy_json_field_conflicts_ = value;
        }

        const FeatureSet& MessageOptions::features() const {
            return features_;
        }

        void MessageOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const std::vector& MessageOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool MessageOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const MessageOptions&>(proto);
            message_set_wire_format_ = msg.message_set_wire_format();
            no_standard_descriptor_accessor_ = msg.no_standard_descriptor_accessor();
            deprecated_ = msg.deprecated();
            map_entry_ = msg.map_entry();
            deprecated_legacy_json_field_conflicts_ = msg.deprecated_legacy_json_field_conflicts();
            features_ = static_cast<FeatureSet>(msg.features());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool MessageOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<MessageOptions*>(proto);
            msg->set_message_set_wire_format(message_set_wire_format_);
            msg->set_no_standard_descriptor_accessor(no_standard_descriptor_accessor_);
            msg->set_deprecated(deprecated_);
            msg->set_map_entry(map_entry_);
            msg->set_deprecated_legacy_json_field_conflicts(deprecated_legacy_json_field_conflicts_);
            msg->set_features(static_cast<int32_t>(features_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        CType FieldOptions::ctype() const {
            return ctype_;
        }

        void FieldOptions::set_ctype(const CType& value) {
            ctype_ = value;
        }

        bool FieldOptions::packed() const {
            return packed_;
        }

        void FieldOptions::set_packed(bool value) {
            packed_ = value;
        }

        JSType FieldOptions::jstype() const {
            return jstype_;
        }

        void FieldOptions::set_jstype(const JSType& value) {
            jstype_ = value;
        }

        bool FieldOptions::lazy() const {
            return lazy_;
        }

        void FieldOptions::set_lazy(bool value) {
            lazy_ = value;
        }

        bool FieldOptions::unverified_lazy() const {
            return unverified_lazy_;
        }

        void FieldOptions::set_unverified_lazy(bool value) {
            unverified_lazy_ = value;
        }

        bool FieldOptions::deprecated() const {
            return deprecated_;
        }

        void FieldOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        bool FieldOptions::weak() const {
            return weak_;
        }

        void FieldOptions::set_weak(bool value) {
            weak_ = value;
        }

        bool FieldOptions::debug_redact() const {
            return debug_redact_;
        }

        void FieldOptions::set_debug_redact(bool value) {
            debug_redact_ = value;
        }

        OptionRetention FieldOptions::retention() const {
            return retention_;
        }

        void FieldOptions::set_retention(const OptionRetention& value) {
            retention_ = value;
        }

        const std::vector& FieldOptions::targets() const {
            return targets_;
        }

        const std::vector& FieldOptions::edition_defaults() const {
            return edition_defaults_;
        }

        const FeatureSet& FieldOptions::features() const {
            return features_;
        }

        void FieldOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const FeatureSupport& FieldOptions::feature_support() const {
            return feature_support_;
        }

        void FieldOptions::set_feature_support(const FeatureSupport& value) {
            feature_support_ = value;
        }

        const std::vector& FieldOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool FieldOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FieldOptions&>(proto);
            ctype_ = static_cast<CType>(msg.ctype());
            packed_ = msg.packed();
            jstype_ = static_cast<JSType>(msg.jstype());
            lazy_ = msg.lazy();
            unverified_lazy_ = msg.unverified_lazy();
            deprecated_ = msg.deprecated();
            weak_ = msg.weak();
            debug_redact_ = msg.debug_redact();
            retention_ = static_cast<OptionRetention>(msg.retention());
            // TODO: repeated field targets
            // TODO: repeated field edition_defaults
            features_ = static_cast<FeatureSet>(msg.features());
            feature_support_ = static_cast<FeatureSupport>(msg.feature_support());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool FieldOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FieldOptions*>(proto);
            msg->set_ctype(static_cast<int32_t>(ctype_));
            msg->set_packed(packed_);
            msg->set_jstype(static_cast<int32_t>(jstype_));
            msg->set_lazy(lazy_);
            msg->set_unverified_lazy(unverified_lazy_);
            msg->set_deprecated(deprecated_);
            msg->set_weak(weak_);
            msg->set_debug_redact(debug_redact_);
            msg->set_retention(static_cast<int32_t>(retention_));
            // TODO: repeated field targets
            // TODO: repeated field edition_defaults
            msg->set_features(static_cast<int32_t>(features_));
            msg->set_feature_support(static_cast<int32_t>(feature_support_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        Edition FieldOptions::EditionDefault::edition() const {
            return edition_;
        }

        void FieldOptions::EditionDefault::set_edition(const Edition& value) {
            edition_ = value;
        }

        const std::string& FieldOptions::EditionDefault::value() const {
            return value_;
        }

        void FieldOptions::EditionDefault::set_value(const std::string& value) {
            value_ = value;
        }

        bool FieldOptions::EditionDefault::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EditionDefault&>(proto);
            edition_ = static_cast<Edition>(msg.edition());
            value_ = msg.value();
            return true;
        }

        bool FieldOptions::EditionDefault::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EditionDefault*>(proto);
            msg->set_edition(static_cast<int32_t>(edition_));
            msg->set_value(value_);
            return true;
        }

        Edition FieldOptions::FeatureSupport::edition_introduced() const {
            return edition_introduced_;
        }

        void FieldOptions::FeatureSupport::set_edition_introduced(const Edition& value) {
            edition_introduced_ = value;
        }

        Edition FieldOptions::FeatureSupport::edition_deprecated() const {
            return edition_deprecated_;
        }

        void FieldOptions::FeatureSupport::set_edition_deprecated(const Edition& value) {
            edition_deprecated_ = value;
        }

        const std::string& FieldOptions::FeatureSupport::deprecation_warning() const {
            return deprecation_warning_;
        }

        void FieldOptions::FeatureSupport::set_deprecation_warning(const std::string& value) {
            deprecation_warning_ = value;
        }

        Edition FieldOptions::FeatureSupport::edition_removed() const {
            return edition_removed_;
        }

        void FieldOptions::FeatureSupport::set_edition_removed(const Edition& value) {
            edition_removed_ = value;
        }

        bool FieldOptions::FeatureSupport::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FeatureSupport&>(proto);
            edition_introduced_ = static_cast<Edition>(msg.edition_introduced());
            edition_deprecated_ = static_cast<Edition>(msg.edition_deprecated());
            deprecation_warning_ = msg.deprecation_warning();
            edition_removed_ = static_cast<Edition>(msg.edition_removed());
            return true;
        }

        bool FieldOptions::FeatureSupport::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FeatureSupport*>(proto);
            msg->set_edition_introduced(static_cast<int32_t>(edition_introduced_));
            msg->set_edition_deprecated(static_cast<int32_t>(edition_deprecated_));
            msg->set_deprecation_warning(deprecation_warning_);
            msg->set_edition_removed(static_cast<int32_t>(edition_removed_));
            return true;
        }

        const FeatureSet& OneofOptions::features() const {
            return features_;
        }

        void OneofOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const std::vector& OneofOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool OneofOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const OneofOptions&>(proto);
            features_ = static_cast<FeatureSet>(msg.features());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool OneofOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<OneofOptions*>(proto);
            msg->set_features(static_cast<int32_t>(features_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool EnumOptions::allow_alias() const {
            return allow_alias_;
        }

        void EnumOptions::set_allow_alias(bool value) {
            allow_alias_ = value;
        }

        bool EnumOptions::deprecated() const {
            return deprecated_;
        }

        void EnumOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        bool EnumOptions::deprecated_legacy_json_field_conflicts() const {
            return deprecated_legacy_json_field_conflicts_;
        }

        void EnumOptions::set_deprecated_legacy_json_field_conflicts(bool value) {
            deprecated_legacy_json_field_conflicts_ = value;
        }

        const FeatureSet& EnumOptions::features() const {
            return features_;
        }

        void EnumOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const std::vector& EnumOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool EnumOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EnumOptions&>(proto);
            allow_alias_ = msg.allow_alias();
            deprecated_ = msg.deprecated();
            deprecated_legacy_json_field_conflicts_ = msg.deprecated_legacy_json_field_conflicts();
            features_ = static_cast<FeatureSet>(msg.features());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool EnumOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EnumOptions*>(proto);
            msg->set_allow_alias(allow_alias_);
            msg->set_deprecated(deprecated_);
            msg->set_deprecated_legacy_json_field_conflicts(deprecated_legacy_json_field_conflicts_);
            msg->set_features(static_cast<int32_t>(features_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool EnumValueOptions::deprecated() const {
            return deprecated_;
        }

        void EnumValueOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        const FeatureSet& EnumValueOptions::features() const {
            return features_;
        }

        void EnumValueOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        bool EnumValueOptions::debug_redact() const {
            return debug_redact_;
        }

        void EnumValueOptions::set_debug_redact(bool value) {
            debug_redact_ = value;
        }

        const FeatureSupport& EnumValueOptions::feature_support() const {
            return feature_support_;
        }

        void EnumValueOptions::set_feature_support(const FeatureSupport& value) {
            feature_support_ = value;
        }

        const std::vector& EnumValueOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool EnumValueOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const EnumValueOptions&>(proto);
            deprecated_ = msg.deprecated();
            features_ = static_cast<FeatureSet>(msg.features());
            debug_redact_ = msg.debug_redact();
            feature_support_ = static_cast<FeatureSupport>(msg.feature_support());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool EnumValueOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<EnumValueOptions*>(proto);
            msg->set_deprecated(deprecated_);
            msg->set_features(static_cast<int32_t>(features_));
            msg->set_debug_redact(debug_redact_);
            msg->set_feature_support(static_cast<int32_t>(feature_support_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        const FeatureSet& ServiceOptions::features() const {
            return features_;
        }

        void ServiceOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        bool ServiceOptions::deprecated() const {
            return deprecated_;
        }

        void ServiceOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        const std::vector& ServiceOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool ServiceOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const ServiceOptions&>(proto);
            features_ = static_cast<FeatureSet>(msg.features());
            deprecated_ = msg.deprecated();
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool ServiceOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<ServiceOptions*>(proto);
            msg->set_features(static_cast<int32_t>(features_));
            msg->set_deprecated(deprecated_);
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool MethodOptions::deprecated() const {
            return deprecated_;
        }

        void MethodOptions::set_deprecated(bool value) {
            deprecated_ = value;
        }

        IdempotencyLevel MethodOptions::idempotency_level() const {
            return idempotency_level_;
        }

        void MethodOptions::set_idempotency_level(const IdempotencyLevel& value) {
            idempotency_level_ = value;
        }

        const FeatureSet& MethodOptions::features() const {
            return features_;
        }

        void MethodOptions::set_features(const FeatureSet& value) {
            features_ = value;
        }

        const std::vector& MethodOptions::uninterpreted_option() const {
            return uninterpreted_option_;
        }

        bool MethodOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const MethodOptions&>(proto);
            deprecated_ = msg.deprecated();
            idempotency_level_ = static_cast<IdempotencyLevel>(msg.idempotency_level());
            features_ = static_cast<FeatureSet>(msg.features());
            // TODO: repeated field uninterpreted_option
            return true;
        }

        bool MethodOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<MethodOptions*>(proto);
            msg->set_deprecated(deprecated_);
            msg->set_idempotency_level(static_cast<int32_t>(idempotency_level_));
            msg->set_features(static_cast<int32_t>(features_));
            // TODO: repeated field uninterpreted_option
            return true;
        }

        const std::vector& UninterpretedOption::name() const {
            return name_;
        }

        const std::string& UninterpretedOption::identifier_value() const {
            return identifier_value_;
        }

        void UninterpretedOption::set_identifier_value(const std::string& value) {
            identifier_value_ = value;
        }

        uint64_t UninterpretedOption::positive_int_value() const {
            return positive_int_value_;
        }

        void UninterpretedOption::set_positive_int_value(uint64_t value) {
            positive_int_value_ = value;
        }

        int64_t UninterpretedOption::negative_int_value() const {
            return negative_int_value_;
        }

        void UninterpretedOption::set_negative_int_value(int64_t value) {
            negative_int_value_ = value;
        }

        double UninterpretedOption::double_value() const {
            return double_value_;
        }

        void UninterpretedOption::set_double_value(double value) {
            double_value_ = value;
        }

        const std::string& UninterpretedOption::string_value() const {
            return string_value_;
        }

        void UninterpretedOption::set_string_value(const std::string& value) {
            string_value_ = value;
        }

        const std::string& UninterpretedOption::aggregate_value() const {
            return aggregate_value_;
        }

        void UninterpretedOption::set_aggregate_value(const std::string& value) {
            aggregate_value_ = value;
        }

        bool UninterpretedOption::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const UninterpretedOption&>(proto);
            // TODO: repeated field name
            identifier_value_ = msg.identifier_value();
            positive_int_value_ = msg.positive_int_value();
            negative_int_value_ = msg.negative_int_value();
            double_value_ = msg.double_value();
            string_value_ = msg.string_value();
            aggregate_value_ = msg.aggregate_value();
            return true;
        }

        bool UninterpretedOption::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<UninterpretedOption*>(proto);
            // TODO: repeated field name
            msg->set_identifier_value(identifier_value_);
            msg->set_positive_int_value(positive_int_value_);
            msg->set_negative_int_value(negative_int_value_);
            msg->set_double_value(double_value_);
            msg->set_string_value(string_value_);
            msg->set_aggregate_value(aggregate_value_);
            return true;
        }

        const std::string& UninterpretedOption::NamePart::name_part() const {
            return name_part_;
        }

        void UninterpretedOption::NamePart::set_name_part(const std::string& value) {
            name_part_ = value;
        }

        bool UninterpretedOption::NamePart::is_extension() const {
            return is_extension_;
        }

        void UninterpretedOption::NamePart::set_is_extension(bool value) {
            is_extension_ = value;
        }

        bool UninterpretedOption::NamePart::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const NamePart&>(proto);
            name_part_ = msg.name_part();
            is_extension_ = msg.is_extension();
            return true;
        }

        bool UninterpretedOption::NamePart::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<NamePart*>(proto);
            msg->set_name_part(name_part_);
            msg->set_is_extension(is_extension_);
            return true;
        }

        FieldPresence FeatureSet::field_presence() const {
            return field_presence_;
        }

        void FeatureSet::set_field_presence(const FieldPresence& value) {
            field_presence_ = value;
        }

        EnumType FeatureSet::enum_type() const {
            return enum_type_;
        }

        void FeatureSet::set_enum_type(const EnumType& value) {
            enum_type_ = value;
        }

        RepeatedFieldEncoding FeatureSet::repeated_field_encoding() const {
            return repeated_field_encoding_;
        }

        void FeatureSet::set_repeated_field_encoding(const RepeatedFieldEncoding& value) {
            repeated_field_encoding_ = value;
        }

        Utf8Validation FeatureSet::utf8_validation() const {
            return utf8_validation_;
        }

        void FeatureSet::set_utf8_validation(const Utf8Validation& value) {
            utf8_validation_ = value;
        }

        MessageEncoding FeatureSet::message_encoding() const {
            return message_encoding_;
        }

        void FeatureSet::set_message_encoding(const MessageEncoding& value) {
            message_encoding_ = value;
        }

        JsonFormat FeatureSet::json_format() const {
            return json_format_;
        }

        void FeatureSet::set_json_format(const JsonFormat& value) {
            json_format_ = value;
        }

        bool FeatureSet::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FeatureSet&>(proto);
            field_presence_ = static_cast<FieldPresence>(msg.field_presence());
            enum_type_ = static_cast<EnumType>(msg.enum_type());
            repeated_field_encoding_ = static_cast<RepeatedFieldEncoding>(msg.repeated_field_encoding());
            utf8_validation_ = static_cast<Utf8Validation>(msg.utf8_validation());
            message_encoding_ = static_cast<MessageEncoding>(msg.message_encoding());
            json_format_ = static_cast<JsonFormat>(msg.json_format());
            return true;
        }

        bool FeatureSet::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FeatureSet*>(proto);
            msg->set_field_presence(static_cast<int32_t>(field_presence_));
            msg->set_enum_type(static_cast<int32_t>(enum_type_));
            msg->set_repeated_field_encoding(static_cast<int32_t>(repeated_field_encoding_));
            msg->set_utf8_validation(static_cast<int32_t>(utf8_validation_));
            msg->set_message_encoding(static_cast<int32_t>(message_encoding_));
            msg->set_json_format(static_cast<int32_t>(json_format_));
            return true;
        }

        const std::vector& FeatureSetDefaults::defaults() const {
            return defaults_;
        }

        Edition FeatureSetDefaults::minimum_edition() const {
            return minimum_edition_;
        }

        void FeatureSetDefaults::set_minimum_edition(const Edition& value) {
            minimum_edition_ = value;
        }

        Edition FeatureSetDefaults::maximum_edition() const {
            return maximum_edition_;
        }

        void FeatureSetDefaults::set_maximum_edition(const Edition& value) {
            maximum_edition_ = value;
        }

        bool FeatureSetDefaults::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FeatureSetDefaults&>(proto);
            // TODO: repeated field defaults
            minimum_edition_ = static_cast<Edition>(msg.minimum_edition());
            maximum_edition_ = static_cast<Edition>(msg.maximum_edition());
            return true;
        }

        bool FeatureSetDefaults::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FeatureSetDefaults*>(proto);
            // TODO: repeated field defaults
            msg->set_minimum_edition(static_cast<int32_t>(minimum_edition_));
            msg->set_maximum_edition(static_cast<int32_t>(maximum_edition_));
            return true;
        }

        Edition FeatureSetDefaults::FeatureSetEditionDefault::edition() const {
            return edition_;
        }

        void FeatureSetDefaults::FeatureSetEditionDefault::set_edition(const Edition& value) {
            edition_ = value;
        }

        const FeatureSet& FeatureSetDefaults::FeatureSetEditionDefault::overridable_features() const {
            return overridable_features_;
        }

        void FeatureSetDefaults::FeatureSetEditionDefault::set_overridable_features(const FeatureSet& value) {
            overridable_features_ = value;
        }

        const FeatureSet& FeatureSetDefaults::FeatureSetEditionDefault::fixed_features() const {
            return fixed_features_;
        }

        void FeatureSetDefaults::FeatureSetEditionDefault::set_fixed_features(const FeatureSet& value) {
            fixed_features_ = value;
        }

        bool FeatureSetDefaults::FeatureSetEditionDefault::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const FeatureSetEditionDefault&>(proto);
            edition_ = static_cast<Edition>(msg.edition());
            overridable_features_ = static_cast<FeatureSet>(msg.overridable_features());
            fixed_features_ = static_cast<FeatureSet>(msg.fixed_features());
            return true;
        }

        bool FeatureSetDefaults::FeatureSetEditionDefault::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<FeatureSetEditionDefault*>(proto);
            msg->set_edition(static_cast<int32_t>(edition_));
            msg->set_overridable_features(static_cast<int32_t>(overridable_features_));
            msg->set_fixed_features(static_cast<int32_t>(fixed_features_));
            return true;
        }

        const std::vector& SourceCodeInfo::location() const {
            return location_;
        }

        bool SourceCodeInfo::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SourceCodeInfo&>(proto);
            // TODO: repeated field location
            return true;
        }

        bool SourceCodeInfo::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SourceCodeInfo*>(proto);
            // TODO: repeated field location
            return true;
        }

        const std::vector& SourceCodeInfo::Location::path() const {
            return path_;
        }

        const std::vector& SourceCodeInfo::Location::span() const {
            return span_;
        }

        const std::string& SourceCodeInfo::Location::leading_comments() const {
            return leading_comments_;
        }

        void SourceCodeInfo::Location::set_leading_comments(const std::string& value) {
            leading_comments_ = value;
        }

        const std::string& SourceCodeInfo::Location::trailing_comments() const {
            return trailing_comments_;
        }

        void SourceCodeInfo::Location::set_trailing_comments(const std::string& value) {
            trailing_comments_ = value;
        }

        const std::vector& SourceCodeInfo::Location::leading_detached_comments() const {
            return leading_detached_comments_;
        }

        bool SourceCodeInfo::Location::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const Location&>(proto);
            // TODO: repeated field path
            // TODO: repeated field span
            leading_comments_ = msg.leading_comments();
            trailing_comments_ = msg.trailing_comments();
            // TODO: repeated field leading_detached_comments
            return true;
        }

        bool SourceCodeInfo::Location::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<Location*>(proto);
            // TODO: repeated field path
            // TODO: repeated field span
            msg->set_leading_comments(leading_comments_);
            msg->set_trailing_comments(trailing_comments_);
            // TODO: repeated field leading_detached_comments
            return true;
        }

        const std::vector& GeneratedCodeInfo::annotation() const {
            return annotation_;
        }

        bool GeneratedCodeInfo::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const GeneratedCodeInfo&>(proto);
            // TODO: repeated field annotation
            return true;
        }

        bool GeneratedCodeInfo::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<GeneratedCodeInfo*>(proto);
            // TODO: repeated field annotation
            return true;
        }

        const std::vector& GeneratedCodeInfo::Annotation::path() const {
            return path_;
        }

        const std::string& GeneratedCodeInfo::Annotation::source_file() const {
            return source_file_;
        }

        void GeneratedCodeInfo::Annotation::set_source_file(const std::string& value) {
            source_file_ = value;
        }

        int32_t GeneratedCodeInfo::Annotation::begin() const {
            return begin_;
        }

        void GeneratedCodeInfo::Annotation::set_begin(int32_t value) {
            begin_ = value;
        }

        int32_t GeneratedCodeInfo::Annotation::end() const {
            return end_;
        }

        void GeneratedCodeInfo::Annotation::set_end(int32_t value) {
            end_ = value;
        }

        Semantic GeneratedCodeInfo::Annotation::semantic() const {
            return semantic_;
        }

        void GeneratedCodeInfo::Annotation::set_semantic(const Semantic& value) {
            semantic_ = value;
        }

        bool GeneratedCodeInfo::Annotation::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const Annotation&>(proto);
            // TODO: repeated field path
            source_file_ = msg.source_file();
            begin_ = msg.begin();
            end_ = msg.end();
            semantic_ = static_cast<Semantic>(msg.semantic());
            return true;
        }

        bool GeneratedCodeInfo::Annotation::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<Annotation*>(proto);
            // TODO: repeated field path
            msg->set_source_file(source_file_);
            msg->set_begin(begin_);
            msg->set_end(end_);
            msg->set_semantic(static_cast<int32_t>(semantic_));
            return true;
        }

    }  // namespace protobuf
}  // namespace google

