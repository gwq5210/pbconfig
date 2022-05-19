#include "config.h"

#include "parser_manager.h"

namespace pbconfig {

bool PbConfig::Parse(const std::string& filename, FileType file_type/* = FileType::kAutoDetect*/) {
  const Parser* parser = ParserManager::GetInstance().GetParser("kJson");
  assert(parser);
  return parser->Parse(filename, *this);
}

}