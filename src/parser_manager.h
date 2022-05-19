#pragma once

#include "parser.h"

#include <cassert>
#include <string>
#include <unordered_map>
#include <functional>

namespace pbconfig {

using InitParserFunc = std::function<Parser*(void)>;

#define REGISTER_PARSER(PARSER) \
  bool resgister_parser_##PARSER##_ret = pbconfig::ParserManager::GetInstance().AddParser(&PARSER::GetInstance())

class ParserManager {
 public:
  static ParserManager& GetInstance() {
    static ParserManager instance;
    return instance;
  }

  bool AddParser(const Parser* parser) {
    const std::string& name = parser->name();
    assert(!name.empty());
    assert(parser);
    assert(!HasParser(name));
    parsers_.emplace(name, parser);
    return true;
  }

  bool HasParser(const std::string& name) const {
    return parsers_.find(name) != parsers_.end();
  }

  const Parser* GetParser(const std::string& name) const {
    auto it = parsers_.find(name);
    if (it != parsers_.end()) {
      return it->second;
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::string, const Parser*> parsers_;

  ParserManager() = default;
  ~ParserManager() = default;
  ParserManager(const ParserManager& other) = delete;
  ParserManager& operator=(const ParserManager& other) = delete;
};

}