#pragma once

#include <cstdio>
#include <string>
#include <iterator>
#include <fstream>

namespace pbconfig {

std::string ReadFile(const std::string& filename) {
  std::ifstream in(filename);
  return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

}