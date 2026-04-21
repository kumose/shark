#pragma once
#include <cstdint>
#include <string>

namespace google {
    namespace protobuf {

        enum class Edition : uint32_t {
            EDITION_UNKNOWN = 0,
            EDITION_LEGACY = 900,
            EDITION_PROTO2 = 998,
            EDITION_PROTO3 = 999,
            EDITION_2023 = 1000,
            EDITION_2024 = 1001,
            EDITION_1_TEST_ONLY = 1,
            EDITION_2_TEST_ONLY = 2,
            EDITION_99997_TEST_ONLY = 99997,
            EDITION_99998_TEST_ONLY = 99998,
            EDITION_99999_TEST_ONLY = 99999,
            EDITION_MAX = 2147483647,
        };

        class FileDescriptorSet {
            public:
            private:
                std::vector file_;
            public:
                const std::vector& file() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FileDescriptorProto {
            public:
            private:
                std::string name_;
                std::string package_;
                std::vector dependency_;
                std::vector public_dependency_;
                std::vector weak_dependency_;
                std::vector message_type_;
                std::vector enum_type_;
                std::vector service_;
                std::vector extension_;
                FileOptions options_;
                SourceCodeInfo source_code_info_;
                std::string syntax_;
                Edition edition_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const std::string& package() const;
                void set_package(const std::string& value);
                const std::vector& dependency() const;
                const std::vector& public_dependency() const;
                const std::vector& weak_dependency() const;
                const std::vector& message_type() const;
                const std::vector& enum_type() const;
                const std::vector& service() const;
                const std::vector& extension() const;
                const FileOptions& options() const;
                void set_options(const FileOptions& value);
                const SourceCodeInfo& source_code_info() const;
                void set_source_code_info(const SourceCodeInfo& value);
                const std::string& syntax() const;
                void set_syntax(const std::string& value);
                Edition edition() const;
                void set_edition(const Edition& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class DescriptorProto {
            class ExtensionRange {
                public:
                private:
                    int32_t start_;
                    int32_t end_;
                    ExtensionRangeOptions options_;
                public:
                    int32_t start() const;
                    void set_start(int32_t value);
                    int32_t end() const;
                    void set_end(int32_t value);
                    const ExtensionRangeOptions& options() const;
                    void set_options(const ExtensionRangeOptions& value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            class ReservedRange {
                public:
                private:
                    int32_t start_;
                    int32_t end_;
                public:
                    int32_t start() const;
                    void set_start(int32_t value);
                    int32_t end() const;
                    void set_end(int32_t value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::string name_;
                std::vector field_;
                std::vector extension_;
                std::vector nested_type_;
                std::vector enum_type_;
                std::vector extension_range_;
                std::vector oneof_decl_;
                MessageOptions options_;
                std::vector reserved_range_;
                std::vector reserved_name_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const std::vector& field() const;
                const std::vector& extension() const;
                const std::vector& nested_type() const;
                const std::vector& enum_type() const;
                const std::vector& extension_range() const;
                const std::vector& oneof_decl() const;
                const MessageOptions& options() const;
                void set_options(const MessageOptions& value);
                const std::vector& reserved_range() const;
                const std::vector& reserved_name() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class ExtensionRangeOptions {
            enum class VerificationState : uint32_t {
                DECLARATION = 0,
                UNVERIFIED = 1,
            };

            class Declaration {
                public:
                private:
                    int32_t number_;
                    std::string full_name_;
                    std::string type_;
                    bool reserved_;
                    bool repeated_;
                public:
                    int32_t number() const;
                    void set_number(int32_t value);
                    const std::string& full_name() const;
                    void set_full_name(const std::string& value);
                    const std::string& type() const;
                    void set_type(const std::string& value);
                    bool reserved() const;
                    void set_reserved(bool value);
                    bool repeated() const;
                    void set_repeated(bool value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::vector uninterpreted_option_;
                std::vector declaration_;
                FeatureSet features_;
                VerificationState verification_;
            public:
                const std::vector& uninterpreted_option() const;
                const std::vector& declaration() const;
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                VerificationState verification() const;
                void set_verification(const VerificationState& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FieldDescriptorProto {
            enum class Type : uint32_t {
                TYPE_DOUBLE = 1,
                TYPE_FLOAT = 2,
                TYPE_INT64 = 3,
                TYPE_UINT64 = 4,
                TYPE_INT32 = 5,
                TYPE_FIXED64 = 6,
                TYPE_FIXED32 = 7,
                TYPE_BOOL = 8,
                TYPE_STRING = 9,
                TYPE_GROUP = 10,
                TYPE_MESSAGE = 11,
                TYPE_BYTES = 12,
                TYPE_UINT32 = 13,
                TYPE_ENUM = 14,
                TYPE_SFIXED32 = 15,
                TYPE_SFIXED64 = 16,
                TYPE_SINT32 = 17,
                TYPE_SINT64 = 18,
            };

            enum class Label : uint32_t {
                LABEL_OPTIONAL = 1,
                LABEL_REPEATED = 3,
                LABEL_REQUIRED = 2,
            };

            public:
            private:
                std::string name_;
                int32_t number_;
                Label label_;
                Type type_;
                std::string type_name_;
                std::string extendee_;
                std::string default_value_;
                int32_t oneof_index_;
                std::string json_name_;
                FieldOptions options_;
                bool proto3_optional_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                int32_t number() const;
                void set_number(int32_t value);
                Label label() const;
                void set_label(const Label& value);
                Type type() const;
                void set_type(const Type& value);
                const std::string& type_name() const;
                void set_type_name(const std::string& value);
                const std::string& extendee() const;
                void set_extendee(const std::string& value);
                const std::string& default_value() const;
                void set_default_value(const std::string& value);
                int32_t oneof_index() const;
                void set_oneof_index(int32_t value);
                const std::string& json_name() const;
                void set_json_name(const std::string& value);
                const FieldOptions& options() const;
                void set_options(const FieldOptions& value);
                bool proto3_optional() const;
                void set_proto3_optional(bool value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class OneofDescriptorProto {
            public:
            private:
                std::string name_;
                OneofOptions options_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const OneofOptions& options() const;
                void set_options(const OneofOptions& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class EnumDescriptorProto {
            class EnumReservedRange {
                public:
                private:
                    int32_t start_;
                    int32_t end_;
                public:
                    int32_t start() const;
                    void set_start(int32_t value);
                    int32_t end() const;
                    void set_end(int32_t value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::string name_;
                std::vector value_;
                EnumOptions options_;
                std::vector reserved_range_;
                std::vector reserved_name_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const std::vector& value() const;
                const EnumOptions& options() const;
                void set_options(const EnumOptions& value);
                const std::vector& reserved_range() const;
                const std::vector& reserved_name() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class EnumValueDescriptorProto {
            public:
            private:
                std::string name_;
                int32_t number_;
                EnumValueOptions options_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                int32_t number() const;
                void set_number(int32_t value);
                const EnumValueOptions& options() const;
                void set_options(const EnumValueOptions& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class ServiceDescriptorProto {
            public:
            private:
                std::string name_;
                std::vector method_;
                ServiceOptions options_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const std::vector& method() const;
                const ServiceOptions& options() const;
                void set_options(const ServiceOptions& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class MethodDescriptorProto {
            public:
            private:
                std::string name_;
                std::string input_type_;
                std::string output_type_;
                MethodOptions options_;
                bool client_streaming_;
                bool server_streaming_;
            public:
                const std::string& name() const;
                void set_name(const std::string& value);
                const std::string& input_type() const;
                void set_input_type(const std::string& value);
                const std::string& output_type() const;
                void set_output_type(const std::string& value);
                const MethodOptions& options() const;
                void set_options(const MethodOptions& value);
                bool client_streaming() const;
                void set_client_streaming(bool value);
                bool server_streaming() const;
                void set_server_streaming(bool value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FileOptions {
            enum class OptimizeMode : uint32_t {
                SPEED = 1,
                CODE_SIZE = 2,
                LITE_RUNTIME = 3,
            };

            public:
            private:
                std::string java_package_;
                std::string java_outer_classname_;
                bool java_multiple_files_;
                bool java_generate_equals_and_hash_;
                bool java_string_check_utf8_;
                OptimizeMode optimize_for_;
                std::string go_package_;
                bool cc_generic_services_;
                bool java_generic_services_;
                bool py_generic_services_;
                bool deprecated_;
                bool cc_enable_arenas_;
                std::string objc_class_prefix_;
                std::string csharp_namespace_;
                std::string swift_prefix_;
                std::string php_class_prefix_;
                std::string php_namespace_;
                std::string php_metadata_namespace_;
                std::string ruby_package_;
                FeatureSet features_;
                std::vector uninterpreted_option_;
            public:
                const std::string& java_package() const;
                void set_java_package(const std::string& value);
                const std::string& java_outer_classname() const;
                void set_java_outer_classname(const std::string& value);
                bool java_multiple_files() const;
                void set_java_multiple_files(bool value);
                bool java_generate_equals_and_hash() const;
                void set_java_generate_equals_and_hash(bool value);
                bool java_string_check_utf8() const;
                void set_java_string_check_utf8(bool value);
                OptimizeMode optimize_for() const;
                void set_optimize_for(const OptimizeMode& value);
                const std::string& go_package() const;
                void set_go_package(const std::string& value);
                bool cc_generic_services() const;
                void set_cc_generic_services(bool value);
                bool java_generic_services() const;
                void set_java_generic_services(bool value);
                bool py_generic_services() const;
                void set_py_generic_services(bool value);
                bool deprecated() const;
                void set_deprecated(bool value);
                bool cc_enable_arenas() const;
                void set_cc_enable_arenas(bool value);
                const std::string& objc_class_prefix() const;
                void set_objc_class_prefix(const std::string& value);
                const std::string& csharp_namespace() const;
                void set_csharp_namespace(const std::string& value);
                const std::string& swift_prefix() const;
                void set_swift_prefix(const std::string& value);
                const std::string& php_class_prefix() const;
                void set_php_class_prefix(const std::string& value);
                const std::string& php_namespace() const;
                void set_php_namespace(const std::string& value);
                const std::string& php_metadata_namespace() const;
                void set_php_metadata_namespace(const std::string& value);
                const std::string& ruby_package() const;
                void set_ruby_package(const std::string& value);
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class MessageOptions {
            public:
            private:
                bool message_set_wire_format_;
                bool no_standard_descriptor_accessor_;
                bool deprecated_;
                bool map_entry_;
                bool deprecated_legacy_json_field_conflicts_;
                FeatureSet features_;
                std::vector uninterpreted_option_;
            public:
                bool message_set_wire_format() const;
                void set_message_set_wire_format(bool value);
                bool no_standard_descriptor_accessor() const;
                void set_no_standard_descriptor_accessor(bool value);
                bool deprecated() const;
                void set_deprecated(bool value);
                bool map_entry() const;
                void set_map_entry(bool value);
                bool deprecated_legacy_json_field_conflicts() const;
                void set_deprecated_legacy_json_field_conflicts(bool value);
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FieldOptions {
            enum class CType : uint32_t {
                STRING = 0,
                CORD = 1,
                STRING_PIECE = 2,
            };

            enum class JSType : uint32_t {
                JS_NORMAL = 0,
                JS_STRING = 1,
                JS_NUMBER = 2,
            };

            enum class OptionRetention : uint32_t {
                RETENTION_UNKNOWN = 0,
                RETENTION_RUNTIME = 1,
                RETENTION_SOURCE = 2,
            };

            enum class OptionTargetType : uint32_t {
                TARGET_TYPE_UNKNOWN = 0,
                TARGET_TYPE_FILE = 1,
                TARGET_TYPE_EXTENSION_RANGE = 2,
                TARGET_TYPE_MESSAGE = 3,
                TARGET_TYPE_FIELD = 4,
                TARGET_TYPE_ONEOF = 5,
                TARGET_TYPE_ENUM = 6,
                TARGET_TYPE_ENUM_ENTRY = 7,
                TARGET_TYPE_SERVICE = 8,
                TARGET_TYPE_METHOD = 9,
            };

            class EditionDefault {
                public:
                private:
                    Edition edition_;
                    std::string value_;
                public:
                    Edition edition() const;
                    void set_edition(const Edition& value);
                    const std::string& value() const;
                    void set_value(const std::string& value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            class FeatureSupport {
                public:
                private:
                    Edition edition_introduced_;
                    Edition edition_deprecated_;
                    std::string deprecation_warning_;
                    Edition edition_removed_;
                public:
                    Edition edition_introduced() const;
                    void set_edition_introduced(const Edition& value);
                    Edition edition_deprecated() const;
                    void set_edition_deprecated(const Edition& value);
                    const std::string& deprecation_warning() const;
                    void set_deprecation_warning(const std::string& value);
                    Edition edition_removed() const;
                    void set_edition_removed(const Edition& value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                CType ctype_;
                bool packed_;
                JSType jstype_;
                bool lazy_;
                bool unverified_lazy_;
                bool deprecated_;
                bool weak_;
                bool debug_redact_;
                OptionRetention retention_;
                std::vector targets_;
                std::vector edition_defaults_;
                FeatureSet features_;
                FeatureSupport feature_support_;
                std::vector uninterpreted_option_;
            public:
                CType ctype() const;
                void set_ctype(const CType& value);
                bool packed() const;
                void set_packed(bool value);
                JSType jstype() const;
                void set_jstype(const JSType& value);
                bool lazy() const;
                void set_lazy(bool value);
                bool unverified_lazy() const;
                void set_unverified_lazy(bool value);
                bool deprecated() const;
                void set_deprecated(bool value);
                bool weak() const;
                void set_weak(bool value);
                bool debug_redact() const;
                void set_debug_redact(bool value);
                OptionRetention retention() const;
                void set_retention(const OptionRetention& value);
                const std::vector& targets() const;
                const std::vector& edition_defaults() const;
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const FeatureSupport& feature_support() const;
                void set_feature_support(const FeatureSupport& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class OneofOptions {
            public:
            private:
                FeatureSet features_;
                std::vector uninterpreted_option_;
            public:
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class EnumOptions {
            public:
            private:
                bool allow_alias_;
                bool deprecated_;
                bool deprecated_legacy_json_field_conflicts_;
                FeatureSet features_;
                std::vector uninterpreted_option_;
            public:
                bool allow_alias() const;
                void set_allow_alias(bool value);
                bool deprecated() const;
                void set_deprecated(bool value);
                bool deprecated_legacy_json_field_conflicts() const;
                void set_deprecated_legacy_json_field_conflicts(bool value);
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class EnumValueOptions {
            public:
            private:
                bool deprecated_;
                FeatureSet features_;
                bool debug_redact_;
                FeatureSupport feature_support_;
                std::vector uninterpreted_option_;
            public:
                bool deprecated() const;
                void set_deprecated(bool value);
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                bool debug_redact() const;
                void set_debug_redact(bool value);
                const FeatureSupport& feature_support() const;
                void set_feature_support(const FeatureSupport& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class ServiceOptions {
            public:
            private:
                FeatureSet features_;
                bool deprecated_;
                std::vector uninterpreted_option_;
            public:
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                bool deprecated() const;
                void set_deprecated(bool value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class MethodOptions {
            enum class IdempotencyLevel : uint32_t {
                IDEMPOTENCY_UNKNOWN = 0,
                NO_SIDE_EFFECTS = 1,
                IDEMPOTENT = 2,
            };

            public:
            private:
                bool deprecated_;
                IdempotencyLevel idempotency_level_;
                FeatureSet features_;
                std::vector uninterpreted_option_;
            public:
                bool deprecated() const;
                void set_deprecated(bool value);
                IdempotencyLevel idempotency_level() const;
                void set_idempotency_level(const IdempotencyLevel& value);
                const FeatureSet& features() const;
                void set_features(const FeatureSet& value);
                const std::vector& uninterpreted_option() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class UninterpretedOption {
            class NamePart {
                public:
                private:
                    std::string name_part_;
                    bool is_extension_;
                public:
                    const std::string& name_part() const;
                    void set_name_part(const std::string& value);
                    bool is_extension() const;
                    void set_is_extension(bool value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::vector name_;
                std::string identifier_value_;
                uint64_t positive_int_value_;
                int64_t negative_int_value_;
                double double_value_;
                std::string string_value_;
                std::string aggregate_value_;
            public:
                const std::vector& name() const;
                const std::string& identifier_value() const;
                void set_identifier_value(const std::string& value);
                uint64_t positive_int_value() const;
                void set_positive_int_value(uint64_t value);
                int64_t negative_int_value() const;
                void set_negative_int_value(int64_t value);
                double double_value() const;
                void set_double_value(double value);
                const std::string& string_value() const;
                void set_string_value(const std::string& value);
                const std::string& aggregate_value() const;
                void set_aggregate_value(const std::string& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FeatureSet {
            enum class FieldPresence : uint32_t {
                FIELD_PRESENCE_UNKNOWN = 0,
                EXPLICIT = 1,
                IMPLICIT = 2,
                LEGACY_REQUIRED = 3,
            };

            enum class EnumType : uint32_t {
                ENUM_TYPE_UNKNOWN = 0,
                OPEN = 1,
                CLOSED = 2,
            };

            enum class RepeatedFieldEncoding : uint32_t {
                REPEATED_FIELD_ENCODING_UNKNOWN = 0,
                PACKED = 1,
                EXPANDED = 2,
            };

            enum class Utf8Validation : uint32_t {
                UTF8_VALIDATION_UNKNOWN = 0,
                VERIFY = 2,
                NONE = 3,
            };

            enum class MessageEncoding : uint32_t {
                MESSAGE_ENCODING_UNKNOWN = 0,
                LENGTH_PREFIXED = 1,
                DELIMITED = 2,
            };

            enum class JsonFormat : uint32_t {
                JSON_FORMAT_UNKNOWN = 0,
                ALLOW = 1,
                LEGACY_BEST_EFFORT = 2,
            };

            public:
            private:
                FieldPresence field_presence_;
                EnumType enum_type_;
                RepeatedFieldEncoding repeated_field_encoding_;
                Utf8Validation utf8_validation_;
                MessageEncoding message_encoding_;
                JsonFormat json_format_;
            public:
                FieldPresence field_presence() const;
                void set_field_presence(const FieldPresence& value);
                EnumType enum_type() const;
                void set_enum_type(const EnumType& value);
                RepeatedFieldEncoding repeated_field_encoding() const;
                void set_repeated_field_encoding(const RepeatedFieldEncoding& value);
                Utf8Validation utf8_validation() const;
                void set_utf8_validation(const Utf8Validation& value);
                MessageEncoding message_encoding() const;
                void set_message_encoding(const MessageEncoding& value);
                JsonFormat json_format() const;
                void set_json_format(const JsonFormat& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class FeatureSetDefaults {
            class FeatureSetEditionDefault {
                public:
                private:
                    Edition edition_;
                    FeatureSet overridable_features_;
                    FeatureSet fixed_features_;
                public:
                    Edition edition() const;
                    void set_edition(const Edition& value);
                    const FeatureSet& overridable_features() const;
                    void set_overridable_features(const FeatureSet& value);
                    const FeatureSet& fixed_features() const;
                    void set_fixed_features(const FeatureSet& value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::vector defaults_;
                Edition minimum_edition_;
                Edition maximum_edition_;
            public:
                const std::vector& defaults() const;
                Edition minimum_edition() const;
                void set_minimum_edition(const Edition& value);
                Edition maximum_edition() const;
                void set_maximum_edition(const Edition& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class SourceCodeInfo {
            class Location {
                public:
                private:
                    std::vector path_;
                    std::vector span_;
                    std::string leading_comments_;
                    std::string trailing_comments_;
                    std::vector leading_detached_comments_;
                public:
                    const std::vector& path() const;
                    const std::vector& span() const;
                    const std::string& leading_comments() const;
                    void set_leading_comments(const std::string& value);
                    const std::string& trailing_comments() const;
                    void set_trailing_comments(const std::string& value);
                    const std::vector& leading_detached_comments() const;
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::vector location_;
            public:
                const std::vector& location() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class GeneratedCodeInfo {
            class Annotation {
                enum class Semantic : uint32_t {
                    NONE = 0,
                    SET = 1,
                    ALIAS = 2,
                };

                public:
                private:
                    std::vector path_;
                    std::string source_file_;
                    int32_t begin_;
                    int32_t end_;
                    Semantic semantic_;
                public:
                    const std::vector& path() const;
                    const std::string& source_file() const;
                    void set_source_file(const std::string& value);
                    int32_t begin() const;
                    void set_begin(int32_t value);
                    int32_t end() const;
                    void set_end(int32_t value);
                    Semantic semantic() const;
                    void set_semantic(const Semantic& value);
                    bool FromProto(const google::protobuf::Message& proto);
                    bool ToProto(google::protobuf::Message* proto) const;
            };

            public:
            private:
                std::vector annotation_;
            public:
                const std::vector& annotation() const;
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

    }  // namespace protobuf
}  // namespace google

