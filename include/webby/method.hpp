#pragma once

/**
 * @namespace webby
 */
namespace webby {
  /**
   * @brief Bit flags for the HTTP 1.1 methods defined by RFC 2616.
   */
  enum class method {
    CONNECT = 0x01, ///< CONNECT
    DELETE  = 0x02, ///< DELETE
    GET     = 0x04, ///< GET
    HEAD    = 0x08, ///< HEAD
    OPTIONS = 0x10, ///< OPTIONS
    POST    = 0x20, ///< POST
    PUT     = 0x40, ///< PUT
    TRACE   = 0x80, ///< TRACE

    REST    = 0x66, ///< Combination of DELETE, GET, POST, and PUT for RESTful web services.
    ALL     = 0xFF  ///< Match any method
  };

  method operator|(method lhs, method rhs) {
    return static_cast<method>(static_cast<int>(lhs) | static_cast<int>(rhs));
  }

  method operator&(method lhs, method rhs) {
    return static_cast<method>(static_cast<int>(lhs) & static_cast<int>(rhs));
  }

  std::string to_string(method m) {
    std::string str;
    std::vector<std::string> methods;
    if(method::CONNECT == (method::CONNECT & m)) methods.push_back("CONNECT");
    if(method::DELETE  == (method::DELETE  & m)) methods.push_back("DELETE");
    if(method::GET     == (method::GET     & m)) methods.push_back("GET");
    if(method::HEAD    == (method::HEAD    & m)) methods.push_back("HEAD");
    if(method::OPTIONS == (method::OPTIONS & m)) methods.push_back("OPTIONS");
    if(method::POST    == (method::POST    & m)) methods.push_back("POST");
    if(method::PUT     == (method::PUT     & m)) methods.push_back("PUT");
    if(method::TRACE   == (method::TRACE   & m)) methods.push_back("TRACE");
    for(auto itr = methods.begin(); itr != methods.end(); ++itr) {
      if(itr != methods.begin()) {
        str.append(", ");
      }
      str.append(*itr);
    }
    return str;
  }
}
