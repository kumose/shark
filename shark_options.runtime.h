#pragma once
#include <cstdint>
#include <string>

namespace shark {
    namespace idl {

        enum class EnumUnderlyingType : uint32_t {
            ENUM_UNDERLYING_UNSPECIFIED = 0,
            ENUM_UNDERLYING_U8 = 1,
            ENUM_UNDERLYING_U16 = 2,
            ENUM_UNDERLYING_U32 = 3,
            ENUM_UNDERLYING_U64 = 4,
        };

        enum class EnumStringSource : uint32_t {
            ENUM_STRING_SOURCE_UNSPECIFIED = 0,
            ENUM_STRING_SOURCE_PROTO_REFLECTION = 1,
            ENUM_STRING_SOURCE_CUSTOM_TABLE = 2,
        };

        enum class CppStand : uint32_t {
            CXX_UNSPECIFIED = 0,
            CXX_17 = 1,
            CXX_20 = 2,
            CXX_23 = 3,
        };

        enum class SharkEdition : uint32_t {
            EDITION_UNSPECIFIED = 0,
            SK_EDITION = 2026,
        };

        class SharkFileOptions {
            public:
            private:
                std::string runtime_namespace_;
                std::vector cpp_includes_;
                std::string sk_map_type_;
                std::string sk_vector_;
                CppStand sk_stand_;
                SharkEdition edition_;
            public:
                const std::string& runtime_namespace() const;
                void set_runtime_namespace(const std::string& value);
                const std::vector& cpp_includes() const;
                const std::string& sk_map_type() const;
                void set_sk_map_type(const std::string& value);
                const std::string& sk_vector() const;
                void set_sk_vector(const std::string& value);
                CppStand sk_stand() const;
                void set_sk_stand(const CppStand& value);
                SharkEdition edition() const;
                void set_edition(const SharkEdition& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class SharkEnumOptions {
            public:
            private:
                EnumUnderlyingType underlying_type_;
                bool bit_flags_;
            public:
                EnumUnderlyingType underlying_type() const;
                void set_underlying_type(const EnumUnderlyingType& value);
                bool bit_flags() const;
                void set_bit_flags(bool value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class SharkMessageOptions {
            public:
            private:
                bool virtual_dtor_;
                bool private_ctor_;
                bool is_instance_;
            public:
                bool virtual_dtor() const;
                void set_virtual_dtor(bool value);
                bool private_ctor() const;
                void set_private_ctor(bool value);
                bool is_instance() const;
                void set_is_instance(bool value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class SharkFieldOptions {
            public:
            private:
                std::string ctype_;
                bool is_atomic_;
            public:
                const std::string& ctype() const;
                void set_ctype(const std::string& value);
                bool is_atomic() const;
                void set_is_atomic(bool value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

        class SharkEnumValueOptions {
            public:
            private:
                std::string sk_enum_name_;
            public:
                const std::string& sk_enum_name() const;
                void set_sk_enum_name(const std::string& value);
                bool FromProto(const google::protobuf::Message& proto);
                bool ToProto(google::protobuf::Message* proto) const;
        };

    }  // namespace idl
}  // namespace shark

