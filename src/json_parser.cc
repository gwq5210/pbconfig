#include "json_parser.h"

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include "io_util.h"
#include "parser_manager.h"

namespace pbconfig {

REGISTER_PARSER(JsonParser);

}
