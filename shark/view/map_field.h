#pragma once


#include <map>
#include <string>

#include <shark/generator/field_map.h>

namespace shark {
    class MapFieldViewGenerator : public FieldNoMetaGenerator {
    public:
        explicit MapFieldViewGenerator(const google::protobuf::FieldDescriptor *descriptor);

        ~MapFieldViewGenerator() override = default;

        // implements FieldGenerator ---------------------------------------
        void generate_members(google::protobuf::io::Printer *printer) const override;

        void generate_members_declares(google::protobuf::io::Printer *printer) const override;


        void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const override;

        void generate_move_ctor_define(google::protobuf::io::Printer *printer) const override;
        void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const override;

        void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const override ;
        void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const override ;


        virtual std::string get_default_value() const {
            return "";
        }
    };
} // namespace shark
