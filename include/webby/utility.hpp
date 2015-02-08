/**
 * @file utility.hpp
 */

#pragma once
#include <algorithm>
#include <string.h>
#include <string>

/**
 * @namespace webby
 */
namespace webby {
  /**
   * @brief Comparison function for the header map.
   */
  struct no_case_compare {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
      return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
  };

  /**
   * @brief Converts a string to all lowercase characters.
   */
  std::string lowercase(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
  }
}
