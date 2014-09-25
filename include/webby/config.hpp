/**
 * @file config.hpp
 */
#pragma once

#include <qlog.hpp>
#include <memory>
#include <string>

/**
 * @namespace
 */
namespace webby {
  /**
   * Defines all of the configuration options for the embedded server.
   */
  class config {
    public:
      /**
       * @brief Default constructor.
       */
      config() : _address("localhost"),
                 _port(8000),
                 _access_log(new qlog::logger(std::cout, qlog::severity::DEBUG)),
                 _error_log(new qlog::logger(std::cerr, qlog::severity::DEBUG)) {
      }

      /**
       * @brief Destructor
       */
      ~config() {
      }

      /**
       * @brief Gets the server address.
       * @returns current server address.
       */
      const std::string& address() const {
        return this->_address;
      }

      /**
       * @brief Sets the server address.
       * @param[in] address The new server address.
       * @returns a reference to the `webby::config` instance to allow for chaining.
       */
      config& set_address(const std::string& address) {
        this->_address = address;
        return *this;
      }

      /**
       * @brief Sets the server address.
       * @param[in] address The new server address.
       * @returns a reference to this `webby::config` instance to allow for chaining.
       */
      config& set_address(const char* address) {
        this->_address = std::string(address);
        return *this;
      }

      /**
       * @brief Gets the server port.
       * @returns the current server port.
       */
      unsigned short port() const {
        return this->_port;
      }

      /**
       * @brief sets the server port.
       * @param[in] port the new server port.
       * @returns a references to this `webby::config` instance to allow for chaining.
       */
      config& set_port(const unsigned short port) {
        this->_port = port;
        return *this;
      }

      /**
       * @brief Gets the access log.
       * @returns a reference to the access log.
       */
      qlog::logger& access_log() const {
        return *_access_log;
      }

      /**
       * @brief Sets the access log stream.
       * @param[in] log Access log stream.
       * @returns a references to this `webby::config` instance to allow for chaining.
       */
      config& set_access_log(std::unique_ptr<qlog::logger>& log) {
        _access_log = std::move(log);
        return *this;
      }

      /**
       * @brief Gets the error log.
       * @returns a reference to the error log.
       */
      qlog::logger& error_log() const {
        return *_error_log;
      }

      /**
       * @brief Sets the error log stream.
       * @param[in] log Error log stream.
       * @returns a references to this `webby::config` instance to allow for chaining.
       */
      config& set_error_log(std::unique_ptr<qlog::logger>& log) {
        _error_log = std::move(log);
        return *this;
      }

    private:
      /// Hostname or IPv4 address the server listens on. Defaults to `localhost`.
      std::string _address;

      /// Port the server listens on. Defaults to `80`.
      unsigned short _port;

      /// Access log location.
      std::unique_ptr<qlog::logger> _access_log;

      /// Error log location.
      std::unique_ptr<qlog::logger> _error_log;
  };
}
