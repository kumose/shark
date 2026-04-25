#include "shark_options.runtime.h"

namespace shark {
    namespace idl {

        const std::string& SharkFileOptions::runtime_namespace() const {
            return runtime_namespace_;
        }

        void SharkFileOptions::set_runtime_namespace(const std::string& value) {
            runtime_namespace_ = value;
        }

        const std::vector& SharkFileOptions::cpp_includes() const {
            return cpp_includes_;
        }

        const std::string& SharkFileOptions::sk_map_type() const {
            return sk_map_type_;
        }

        void SharkFileOptions::set_sk_map_type(const std::string& value) {
            sk_map_type_ = value;
        }

        const std::string& SharkFileOptions::sk_vector() const {
            return sk_vector_;
        }

        void SharkFileOptions::set_sk_vector(const std::string& value) {
            sk_vector_ = value;
        }

        CppStand SharkFileOptions::sk_stand() const {
            return sk_stand_;
        }

        void SharkFileOptions::set_sk_stand(const CppStand& value) {
            sk_stand_ = value;
        }

        SharkEdition SharkFileOptions::edition() const {
            return edition_;
        }

        void SharkFileOptions::set_edition(const SharkEdition& value) {
            edition_ = value;
        }

        bool SharkFileOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SharkFileOptions&>(proto);
            runtime_namespace_ = msg.runtime_namespace();
            // TODO: repeated field cpp_includes
            sk_map_type_ = msg.sk_map_type();
            sk_vector_ = msg.sk_vector();
            sk_stand_ = static_cast<CppStand>(msg.sk_stand());
            edition_ = static_cast<SharkEdition>(msg.edition());
            return true;
        }

        bool SharkFileOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SharkFileOptions*>(proto);
            msg->set_runtime_namespace(runtime_namespace_);
            // TODO: repeated field cpp_includes
            msg->set_sk_map_type(sk_map_type_);
            msg->set_sk_vector(sk_vector_);
            msg->set_sk_stand(static_cast<int32_t>(sk_stand_));
            msg->set_edition(static_cast<int32_t>(edition_));
            return true;
        }

        EnumUnderlyingType SharkEnumOptions::underlying_type() const {
            return underlying_type_;
        }

        void SharkEnumOptions::set_underlying_type(const EnumUnderlyingType& value) {
            underlying_type_ = value;
        }

        bool SharkEnumOptions::bit_flags() const {
            return bit_flags_;
        }

        void SharkEnumOptions::set_bit_flags(bool value) {
            bit_flags_ = value;
        }

        bool SharkEnumOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SharkEnumOptions&>(proto);
            underlying_type_ = static_cast<EnumUnderlyingType>(msg.underlying_type());
            bit_flags_ = msg.bit_flags();
            return true;
        }

        bool SharkEnumOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SharkEnumOptions*>(proto);
            msg->set_underlying_type(static_cast<int32_t>(underlying_type_));
            msg->set_bit_flags(bit_flags_);
            return true;
        }

        bool SharkMessageOptions::virtual_dtor() const {
            return virtual_dtor_;
        }

        void SharkMessageOptions::set_virtual_dtor(bool value) {
            virtual_dtor_ = value;
        }

        bool SharkMessageOptions::private_ctor() const {
            return private_ctor_;
        }

        void SharkMessageOptions::set_private_ctor(bool value) {
            private_ctor_ = value;
        }

        bool SharkMessageOptions::is_instance() const {
            return is_instance_;
        }

        void SharkMessageOptions::set_is_instance(bool value) {
            is_instance_ = value;
        }

        bool SharkMessageOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SharkMessageOptions&>(proto);
            virtual_dtor_ = msg.virtual_dtor();
            private_ctor_ = msg.private_ctor();
            is_instance_ = msg.is_instance();
            return true;
        }

        bool SharkMessageOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SharkMessageOptions*>(proto);
            msg->set_virtual_dtor(virtual_dtor_);
            msg->set_private_ctor(private_ctor_);
            msg->set_is_instance(is_instance_);
            return true;
        }

        const std::string& SharkFieldOptions::ctype() const {
            return ctype_;
        }

        void SharkFieldOptions::set_ctype(const std::string& value) {
            ctype_ = value;
        }

        bool SharkFieldOptions::is_atomic() const {
            return is_atomic_;
        }

        void SharkFieldOptions::set_is_atomic(bool value) {
            is_atomic_ = value;
        }

        bool SharkFieldOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SharkFieldOptions&>(proto);
            ctype_ = msg.ctype();
            is_atomic_ = msg.is_atomic();
            return true;
        }

        bool SharkFieldOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SharkFieldOptions*>(proto);
            msg->set_ctype(ctype_);
            msg->set_is_atomic(is_atomic_);
            return true;
        }

        const std::string& SharkEnumValueOptions::sk_enum_name() const {
            return sk_enum_name_;
        }

        void SharkEnumValueOptions::set_sk_enum_name(const std::string& value) {
            sk_enum_name_ = value;
        }

        bool SharkEnumValueOptions::FromProto(const google::protobuf::Message& proto) {
            const auto& msg = static_cast<const SharkEnumValueOptions&>(proto);
            sk_enum_name_ = msg.sk_enum_name();
            return true;
        }

        bool SharkEnumValueOptions::ToProto(google::protobuf::Message* proto) const {
            auto* msg = static_cast<SharkEnumValueOptions*>(proto);
            msg->set_sk_enum_name(sk_enum_name_);
            return true;
        }

    }  // namespace idl
}  // namespace shark

