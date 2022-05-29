#include "json_parser.h"

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include "parser_manager.h"
#include "io_util.h"

namespace pbconfig {

REGISTER_PARSER(JsonParser);

}
