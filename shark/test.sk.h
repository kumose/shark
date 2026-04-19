#include <cstdint>

#include <string>

#include <vector>

#include <unordered_map>

#include <google/protobuf/message.h>

enum class Color
:unsigned int{
    RED=0,
    GREEN=1,
    BLUE=2
    };

class Person{
    std::string name_;
    
    int age_;
    
    std::vector<std::string> emails_;
    
    std::vector<test::Person::ScoresEntry> scores_;
    
    test.Color favorite_color_;
    
    test::Person::Address address_;
    
    std::string const& name()const;
    
    int age()const;
    
    std::vector<std::string> const& emails()const;
    
    std::vector<test::Person::ScoresEntry> const& scores()const;
    
    test.Color favorite_color()const;
    
    test::Person::Address const& address()const;
    
    void set_name(std::string& const value);
    
    void set_age(int value);
    
    void set_favorite_color(test.Color value);
    
    void set_address(test::Person::Address& const value);
    
    bool FromProto(google::protobuf::Message const& proto);
    
    bool ToProto(google::protobuf::Message* proto)const;
    
    class Address{
        std::string street_;
        
        int number_;
        
        std::string const& street()const;
        
        int number()const;
        
        void set_street(std::string& const value);
        
        void set_number(int value);
        
        bool FromProto(google::protobuf::Message const& proto);
        
        bool ToProto(google::protobuf::Message* proto)const;
        };
    };
