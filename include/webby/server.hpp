/**
 * @file
 */
#pragma once

#include <webby/config.hpp>
#include <webby/error.hpp>

#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <sstream>

namespace webby {
  class server {
    public:
      /**
       * @brief Default constructor
       */
      server() : _config(std::move(webby::config())), _socket(0) {
        _config.error_log() << qlog::debug << "server::server()" << std::endl;
        try {
          init();
        }
        catch(const webby::server_error& e) {
          _config.error_log() << qlog::error << e.what() << std::endl;
          throw;
        }
      }

      /**
       * @brief Constructor that accepts a server configuration.
       * @param[in] c Server configuration.
       */
      server(const webby::config& c) : _config(c), _socket(0) {
        _config.error_log() << qlog::debug
                            << "server::server(const webby::config&)" << std::endl;
        try {
          init();
        }
        catch(const webby::server_error& e) {
          _config.error_log() << qlog::error << e.what() << std::endl;
          throw;
        }
      }

      /**
       * @brief Destructor
       */
      ~server() {
        _config.error_log() << qlog::debug << "server::~server" << std::endl;
        if(_socket > 0) {
          close(_socket);
        }
      }

    private:
      /**
       * @brief Initializes the server.
       */
      void init() {
        _config.error_log() << qlog::debug << "server::init()" << std::endl;
        try {
          // Used to store return status from various functions.
          int err = 0;

          // The addrinfo structure found in netdb.h is used to define the socket type. Here the
          // socket is configured as a streaming TCP/IP server.
          struct addrinfo hints;
          memset(&hints, 0, sizeof(struct addrinfo));
          hints.ai_flags    = AI_PASSIVE;   // Socket will be passed to bind().
          hints.ai_family   = PF_INET;      // Internet Protocol (IP) socket.
          hints.ai_socktype = SOCK_STREAM;  // Streaming socket.
          hints.ai_protocol = IPPROTO_TCP;  // TCP protocol

          // Retrieves the socket address for the host. Here "localhost" is used so that the server
          // is available only on the current system. A real server would be accessible to the
          // outside world so the hostname would be either one that can be resolved to an external
          // IP, or simply NULL to be accessible on all attached networks. The second parameter is
          // the service (port number) that the server will listen on.
          struct addrinfo* res = NULL;
          if((err = getaddrinfo(_config.address().c_str(), std::to_string(_config.port()).c_str(),
                  &hints, &res))) {
            std::ostringstream msg;
            msg << "Could not resolve address: (" << err << ") " << gai_strerror(err);
            throw webby::server_error(msg.str());
          }

          // Now that all of the information for the socket is available, create it.
          _socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
          if(_socket < 0) {
            std::ostringstream msg;
            msg << "Could not create socket: " << errno;
            throw webby::server_error(msg.str());
          }

          // Allows the server to re-bind to the socket if the server is terminated and restarted
          // quickly (within the TIME_WAIT interval) as it takes time for the OS to notice that
          // this has happened. If "address in use" errors are seen, not using SO_REUSEADDR is
          // usually the cause.
          int reuse = 1;
          if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            std::ostringstream msg;
            msg << "Could not configure socket: " << errno;
            throw webby::server_error(msg.str());
          }

          // Connects (binds) this process to the server socket created above.
          if(bind(_socket, res->ai_addr, res->ai_addrlen) < 0) {
            std::ostringstream msg;
            msg << "Could not bind to socket: " << errno;
            throw webby::server_error(msg.str());
          }

          // Listens for incoming connections. The second paremeter (backlog) is set to 10,000.
          // This is the number of connections that the operating system can queue up while a
          // request is being serviced. If the queue is full, then clients will receive a
          // "connection refused" error. If requests are not handled quickly enough, then queued
          // requests may time out.
          if(listen(_socket, 10000) < 0) {
            std::ostringstream msg;
            msg << "Could not listen for incoming connections: " << errno;
            throw webby::server_error(msg.str());
          }
        }
        catch(const server_error& err) {
          _config.error_log() << qlog::error << err.what() << std::endl;
          throw;
        }
      }

      /** Server configuration. */
      const webby::config& _config;

      /** Server port */
      int _socket;

      // The can be only one server class. Disabling the copy and move constructors as well as the
      // assignment operator help enforce this.
      server(const server&) = delete;
      server(const server&&) = delete;
      void operator=(const server&) = delete;
  };
}
