#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "config.h"

namespace pbconfig {

class Parser {
 public:
  virtual bool Parse(const std::string& filename, PbConfig& config) const { return false; }

  const std::string &name() const { return name_; }
  void set_name(const std::string& name) { name_ = name; }
  void set_name(std::string&& name) { name_ = std::move(name); }

 protected:
  Parser(const std::string& name) : name_(name) {}
  virtual ~Parser() {}

 private:
  std::string name_;

  Parser(const Parser& other) = delete;
  Parser& operator=(const Parser& other) = delete;
};

}