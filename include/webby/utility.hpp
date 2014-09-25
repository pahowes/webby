/**
 * @file utility.hpp
 */

#pragma once
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
}
