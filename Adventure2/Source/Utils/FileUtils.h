#pragma once

#include "pch.h"

template <typename T>
std::vector<T> ReadRawFile(std::string filename) {
   std::ifstream file(filename, std::ios::binary);
   if (!file.is_open()) { return std::vector<T>(); }
   file.unsetf(std::ios::skipws);

   std::vector<T> vec{std::istream_iterator<T>(file), std::istream_iterator<T>()};

   return vec;
}