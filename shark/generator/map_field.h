#pragma once


#include <map>
#include <string>

#include <shark/generator/field.h>

namespace shark {
    class MapFieldGenerator : public FieldGenerator {
    public:
        explicit MapFieldGenerator(const google::protobuf::FieldDescriptor *descriptor, std::string message);

        ~MapFieldGenerator() override = default;

        // implements FieldGenerator ---------------------------------------
        void generate_members(google::protobuf::io::Printer *printer) const override;

        void generate_members_declares(google::protobuf::io::Printer *printer) const override;


        void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const override;

        void generate_move_ctor_define(google::protobuf::io::Printer *printer) const override;
        void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const override;

        void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const override ;
        void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const override ;
        // Generate a static initializer for this field.
        virtual void GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const override {

        }

        // Generate members to initialize this field from a static initializer
        virtual void GenerateStaticInit(google::protobuf::io::Printer *printer) const override {

        }

        virtual std::string get_default_value() const {
            return "";
        }
    };
} // namespace shark
