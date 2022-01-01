#pragma once

#include "pch.h"
#include "Log.h"

float Normalize(unsigned short input, int maxHeight) {
   double ranged = (double)input / USHORT_MAX;
   return (float)ranged * maxHeight;
}

std::vector<float> ReadRawFile(std::string filename, int maxHeight = 256) {
   try {
      auto file = std::ifstream(filename, std::ios::binary);
      if (!file.is_open()) { throw std::exception("Error opening file"); }
      auto vec = std::vector<float>{};

      unsigned short value = 0;
      int i = 0;
      char buf[sizeof(unsigned short)]{};

      while (file.read(buf, sizeof(buf))) {
         memcpy(&value, buf, sizeof(value));
         vec.push_back(Normalize(value, maxHeight));
         ++i;
      }

      return vec;

   } catch (std::exception& e) { LOG_ERROR("error: {}", e.what()); }

   return std::vector<float>();
}
