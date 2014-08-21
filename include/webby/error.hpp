/**
 * @file
 */
#pragma once

#include <stdexcept>

/** @namespace */
namespace webby {
  /**
   * @brief Exception object used for fatal server errors.
   *
   * The `webby::server_error` class represents fatal errors encountered by `webby::server` that
   * cannot be recovered from.
   */
  class server_error : public std::runtime_error {
    public:
      /**
       * @brief Constructs the `webby::server_error` object.
       * @param[in] what_arg Explanatory string.
       */
      explicit server_error(const std::string& what_arg) : runtime_error(what_arg) { }

      /**
       * @brief Constructs the `webby::server_error` object.
       * @param[in] what_arg Explanatory string.
       */
      explicit server_error(const char* what_arg) : runtime_error(what_arg) { }
  };
}
