#include <fc/rpc/multipart_parser.hpp>

#include <fc/io/json.hpp>
#include <fc/reflect/variant.hpp>

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

typedef struct part_file {
    std::string name;
    std::string path;
    explicit part_file(const std::string& name, const std::string& path)
        : name(name), path(path) {}
} part_file;

static multipartparser_callbacks g_callbacks;

static bool             g_body_begin_called;
static std::string      g_header_name;
static std::string      g_header_value;
static std::list<part>  g_parts;
static bool             g_body_end_called;
static std::list<part_file>  g_files;

static void init_globals()
{
    g_body_begin_called = false;
    g_header_name.clear();
    g_header_value.clear();
    g_parts.clear();
    g_files.clear();
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
    bool json_found = false;
    assert(multipartparser_execute(&parser, &g_callbacks, message.c_str(), message.size()) == message.size());
    for (auto& gp : g_parts) {
        if (gp.headers.count("Content-Disposition")) {
            auto& content_disp_header = gp.headers["Content-Disposition"];

            if (!json_found && content_disp_header.find("name=\"json_data\"") != std::string::npos) {
                json_found = true;
                json_str = gp.body;
            } else {
                auto file_name_start = content_disp_header.find("filename=\"");
                if (file_name_start != std::string::npos) {
                    file_name_start += 10;
                    auto filename_end = content_disp_header.find_first_of("\"", file_name_start );
                    if (filename_end != std::string::npos) {
                        auto file_name = content_disp_header.substr(file_name_start, filename_end - file_name_start);
                        g_files.emplace_back(file_name, gp.body);
                    }
                }
            }
        }
    }

    variant var = fc::json::from_string( json_str, fc::json::legacy_parser, max_depth );

    if( var.is_object() && !g_files.empty()) {
        variants files;
        for (auto& f : g_files) {
            mutable_variant_object file_obj;
            file_obj("name", f.name);
            file_obj("path", f.path);
            files.push_back(file_obj);
        }

        variant_object var_obj = var.get_object();
        mutable_variant_object mut_var_obj(var_obj);

        if (var_obj.contains("params")) {
            if (var_obj["params"].is_array()) {
                variants params_obj = var_obj["params"].get_array();
                params_obj.push_back(files);
                mut_var_obj["params"] = params_obj;
            }
        } else {
            variants params_obj;
            params_obj.push_back(files);
            mut_var_obj["params"] = params_obj;
        }

        auto json_string = fc::json::to_string(mut_var_obj);
        return mut_var_obj;
    }

    return var;
}

}} // namespace fc::rpc