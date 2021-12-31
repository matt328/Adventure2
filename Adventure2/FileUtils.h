#pragma once

#include "pch.h"
#include "Log.h"

template <typename T>
std::vector<T> ReadRawFile(std::string filename) {
   std::ifstream file(filename, std::ios::binary);
   if (!file.is_open()) { return std::vector<T>(); }
   file.unsetf(std::ios::skipws);

   std::vector<T> vec{std::istream_iterator<T>(file), std::istream_iterator<T>()};

   LOG_TRACE("Loaded terrain, size: {}", vec.size());
   return vec;
}

void BoxFilter(std::vector<char>& heightData) {
   int width = (int)std::sqrtf((float)heightData.size());
   auto bounds = heightData.size();
   for (int z = 0; z < width; ++z) {
      for (int x = 0; x < width; ++x) {
         char value = 0;
         float cellAverage = 1.0f;
         // Top Row
         if (((x - 1) + (z - 1) * width) >= 0 && ((x - 1) + (z - 1) * width) < bounds) {
            value += heightData[(x - 1) + (z - 1) * width];
            ++cellAverage;
         }
         if (((x - 0) + (z - 1) * width) >= 0 && ((x - 0) + (z - 1) * width) < bounds) {
            value += heightData[(x - 0) + (z - 1) * width];
            ++cellAverage;
         }
         if (((x + 1) + (z - 1) * width) >= 0 && ((x + 1) + (z - 1) * width) < bounds) {
            value += heightData[(x + 1) + (z - 1) * width];
            ++cellAverage;
         }
         // Middle Row
         if (((x - 1) + (z - 0) * width) >= 0 && ((x - 1) + (z - 0) * width) < bounds) {
            value += heightData[(x - 1) + (z - 0) * width];
            ++cellAverage;
         }
         // Center Point will always be in bounds
         value += heightData[x + z + width];

         if (((x + 1) + (z - 0) * width) >= 0 && ((x + 1) + (z - 0) * width) < bounds) {
            value += heightData[(x + 1) + (z - 0) * width];
            ++cellAverage;
         }

         if (((x - 1) + (z + 1) * width) >= 0 && ((x - 1) + (z + 1) * width) < bounds) {
            value += heightData[(x - 1) + (z + 1) * width];
            ++cellAverage;
         }
         if (((x - 0) + (z + 1) * width) >= 0 && ((x - 0) + (z + 1) * width) < bounds) {
            value += heightData[(x - 0) + (z + 1) * width];
            ++cellAverage;
         }
         if (((x + 1) + (z + 1) * width) >= 0 && ((x + 1) + (z + 1) * width) < bounds) {
            value += heightData[(x + 1) + (z + 1) * width];
            ++cellAverage;
         }

         heightData[x + z * width] = (char)(value / cellAverage);
      }
   }
}