#include <fc/rpc/multipart_parser.hpp>

#include <fc/io/json.hpp>

#include "multipartparser.h"

#include <cassert>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <list>
#include <map>
#include <string>

namespace fc { namespace rpc {

typedef struct part {
    std::map<std::string,std::string> headers;
    std::string body;
} part;

static multipartparser_callbacks g_callbacks;

static bool             g_body_begin_called;
static std::string      g_header_name;
static std::string      g_header_value;
static std::list<part>  g_parts;
static bool             g_body_end_called;

static void init_globals()
{
    g_body_begin_called = false;
    g_header_name.clear();
    g_header_value.clear();
    g_parts.clear();
    g_body_end_called = false;
}

static int on_body_begin(multipartparser* /*parser*/)
{
    g_body_begin_called = true;
    return 0;
}

static int on_part_begin(multipartparser* /*parser*/)
{
    g_parts.push_back(part());
    return 0;
}

static void on_header_done()
{
    g_parts.back().headers[g_header_name] = g_header_value;
    g_header_name.clear();
    g_header_value.clear();
}

static int on_header_field(multipartparser* /*parser*/, const char* data, size_t size)
{
    if (g_header_value.size() > 0)
        on_header_done();
    g_header_name.append(data, size);
    return 0;
}

static int on_header_value(multipartparser* /*parser*/, const char* data, size_t size)
{
    g_header_value.append(data, size);
    return 0;
}

static int on_headers_complete(multipartparser* /*parser*/)
{
    if (g_header_value.size() > 0)
        on_header_done();
    return 0;
}

static int on_data(multipartparser* /*parser*/, const char* data, size_t size)
{
    g_parts.back().body.append(data, size);
    return 0;
}

static int on_part_end(multipartparser* /*parser*/)
{
    return 0;
}

static int on_body_end(multipartparser* /*parser*/)
{
    g_body_end_called = true;
    return 0;
}

void test_simple()
{
    multipartparser parser;

#define BOUNDARY "simple boundary"
#define PART0_BODY                                      \
    "This is implicitly typed plain ASCII text.\r\n"    \
    "It does NOT end with a linebreak."
#define PART1_BODY                                      \
    "This is explicitly typed plain ASCII text.\r\n"    \
    "It DOES end with a linebreak.\r\n"
#define BODY                                            \
    "--" BOUNDARY "\r\n"                                \
    "\r\n"                                              \
    PART0_BODY                                          \
    "\r\n--" BOUNDARY "\r\n"                            \
    "Content-type: text/plain; charset=us-ascii\r\n"    \
    "\r\n"                                              \
    PART1_BODY                                          \
    "\r\n--" BOUNDARY "--\r\n"                          \

    init_globals();
    multipartparser_init(&parser, BOUNDARY);

    assert(multipartparser_execute(&parser, &g_callbacks, BODY, strlen(BODY)) == strlen(BODY));
    assert(g_body_begin_called);
    assert(g_parts.size() == 2);

    part p0 = g_parts.front(); g_parts.pop_front();
    assert(p0.headers.empty());
    assert(p0.body.compare(PART0_BODY) == 0);

    part p1 = g_parts.front(); g_parts.pop_front();
    assert(p1.headers.size() == 1);
    assert(p1.body == PART1_BODY);

    assert(g_body_end_called);

#undef BOUNDARY
#undef BODY
}

multipart_parser::multipart_parser() {
}

multipart_parser::~multipart_parser() {
}

variant multipart_parser::parse_string( const std::string& message,  uint32_t max_depth ) {
    multipartparser_callbacks_init(&g_callbacks);
    g_callbacks.on_body_begin = &on_body_begin;
    g_callbacks.on_part_begin = &on_part_begin;
    g_callbacks.on_header_field = &on_header_field;
    g_callbacks.on_header_value = &on_header_value;
    g_callbacks.on_headers_complete = &on_headers_complete;
    g_callbacks.on_data = &on_data;
    g_callbacks.on_part_end = &on_part_end;
    g_callbacks.on_body_end = &on_body_end;

    std::stringstream ss(message.c_str());
    std::string boundary;
    std::getline(ss,boundary);
    boundary = boundary.substr(2, boundary.size() - 3);

    multipartparser parser;
    init_globals();
    multipartparser_init(&parser, boundary.c_str());

    std::string json_str = message;
    assert(multipartparser_execute(&parser, &g_callbacks, message.c_str(), message.size()) == message.size());
    for (auto& gp : g_parts) {
        if (gp.headers.count("Content-Disposition") 
         && gp.headers["Content-Disposition"].find("name=\"json_data\"") != std::string::npos) {
            json_str = gp.body;
         }
    }

    variant var = fc::json::from_string( json_str, fc::json::legacy_parser, max_depth );
    return var;
}

}} // namespace fc::rpc