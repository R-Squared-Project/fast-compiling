#pragma once
#include <fc/variant.hpp>

namespace fc { namespace rpc {

class multipart_parser
{
public:
    multipart_parser();
    ~multipart_parser();
    variant parse_string( const std::string& utf8_str, uint32_t max_depth );
};

}} // namespace fc::rpc