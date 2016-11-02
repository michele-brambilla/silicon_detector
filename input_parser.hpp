#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>

/////////
// JSON input parser
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>


rapidjson::Document parse(const int, char**);
std::string read_config_file(const char*);
