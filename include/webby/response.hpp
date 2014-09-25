/**
 * @file response.hpp
 */
#pragma once

#include <time.h>
#include <map>
#include <webby/utility.hpp>

/**
 * @namespace webby
 */
namespace webby {
  // Forward reference.
  class server;

  /**
   * @brief Encapsulates the response sent back to the connected host.
   */
  class response {
    public:
      /**
       * @brief Exception object used for response errors.
       */
      class error : public std::runtime_error {
        public:
          /**
           * @brief Constructs the `response::error` object.
           * @param[in] what_arg Explanatory string.
           */
          explicit error(const std::string& what_arg) : runtime_error(what_arg) { }

          /**
           * @brief Constructs the `response::error` object.
           * @param[in] what_arg Explanatory string.
           */
          explicit error(const char* what_arg) : runtime_error(what_arg) { }
      };

      /**
       * @brief Sets a header value.
       * @param[in] name Name of the header.
       * @param[in] value Value of the header.
       * @returns Reference to this webby::response object for chaining.
       */
      response& set_header(const std::string& name, const std::string& value) {
        _config.error_log() << qlog::debug << "response::set_header" << std::endl;
        _header[name] = value;
        return *this;
      }

      /**
       * @brief Sets the reason for the response.
       * @param[in] reason Reason string sent with the response.
       * @returns Reference to this webby::response object for chaining.
       */
      response& set_reason(const std::string& reason) {
        _config.error_log() << qlog::debug << "response::set_reason" << std::endl;
        _reason = reason;
        return *this;
      }

      /**
       * @brief Sets the status code for the response.
       * @param[in] status_code Status code for the response.
       * @returns Reference to this webby::response object for chaining.
       */
      response& set_status_code(unsigned short status_code) {
        _config.error_log() << qlog::debug << "response::set_status_code" << std::endl;
        _status_code = status_code;
        return *this;
      }

      /**
       * @brief Sets the HTTP version recognized by this server.
       * @param[in] version HTTP version of this server.
       * @returns Reference to this webby::response object for chaining.
       *
       * In general this function should not be needed. The default HTTP version is 1.1.
       */
      response& set_version(const std::string& version) {
        _config.error_log() << qlog::debug << "response::set_version" << std::endl;
        _version = version;
        return *this;
      }

      /**
       * @brief Sends a body chunk.
       * @param[in] data Data buffer to transmit.
       * @param[in] length Length of the data buffer.
       *
       * When response::write_block() is invoked for the first time all of the headers are
       * transmitted to the connected host. If the `Content-Length` header was not set, then a
       * webby::response::error exception is thrown. The `Content-Length` header is not mandatory,
       * but has become a de-facto standard, and is enforced.
       */
      void write_block(const unsigned char* data, const unsigned long length) {
        _config.error_log() << qlog::debug << "response::write_block" << std::endl;

        // Sends the headers if necessary.
        if(!_sent_headers) {
          if(_header.count("Content-Length") == 0) {
            throw response::error("The Content-Length header was not provided.");
          }
          send_headers();
        }

        // Send the data to the connected host.
        _worker.write_block(data, length);
      }

    protected:
      /**
       * @brief Constructs a new webby::response object from a @p worker socket.
       * @param[in] worker Worker socket used to communicate with the connected host.
       */
      response(const webby::config& config, const net::worker& worker) :
          _config(config), _sent_headers(false), _worker(worker), _version("1.1") {
        _config.error_log() << qlog::debug << "response::response()" << std::endl;
      }

      /**
       * @brief Destroys the webby::response object.
       *
       * If the response has not yet been sent it is sent at this time.
       */
      ~response() {
        _config.error_log() << qlog::debug << "response::~response()" << std::endl;

        // Sends the headers if necessary.
        if(!_sent_headers) {
          if(_header.count("Content-Length") == 0) {
            _header["Content-Length"] = "0";
          }
          send_headers();
        }
      }

      /**
       * @brief Sends the status line and headers to the connected host.
       */
      void send_headers() {
        _config.error_log() << qlog::debug << "response::send_headers()" << std::endl;
        std::ostringstream res;

        // Generates the status line.
        res << "HTTP/" << _version << " " << _status_code << " " << _reason << "\r\n";

        // Adds all of the headers.
        for(auto header = _header.cbegin(); header != _header.cend(); ++header) {
          res << header->first << ": " << header->second << "\r\n";
        }

        // Adds the RFC 1123 Date header.
        struct tm* timeinfo;
        char buffer [80];
        memset(buffer, 0, 80);
        time_t tt = time(nullptr);
        timeinfo = gmtime(&tt);
        strftime (buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        res << "Date: " << buffer << "\r\n";

        // Blank line.
        res << "\r\n";

        const std::string& str = res.str();
        _worker.write_block(str.c_str(), str.length());

        // Flag that the headers have been sent.
        _sent_headers = true;
      }

    private:
      /**
       * @brief Server configuration.
       */
      const webby::config& _config;

      /**
       * @brief Headers sent with the response.
       */
      std::map<std::string, std::string, no_case_compare> _header;

      /**
       * @brief Reason string for the response.
       */
      std::string _reason;

      /**
       * @brief `true` if the headers have already been sent; otherwise `false`.
       */
      bool _sent_headers;

      /**
       * @brief Status code of the response.
       */
      unsigned short _status_code;

      /**
       * @brief Worker socket used to communicate with the connected host.
       */
      const net::worker& _worker;

      /**
       * @brief HTTP version sent to the connected host.
       */
      std::string _version;

      /**
       * @brief Necessary so that webby::server can call the send function.
       */
      friend class webby::server;
  };
}
