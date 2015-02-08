/**
 * @file
 */
#pragma once

#include <asf.hpp>
#include <net.hpp>

#include <webby/config.hpp>
#include <webby/request.hpp>
#include <webby/response.hpp>
#include <webby/router.hpp>

namespace webby {
  /**
   * @brief Server object that the client interacts with.
   */
  class server {
    public:
      /**
       * @brief Exception object used for fatal server errors.
       *
       * The `webby::server::error` class represents fatal errors encountered by `webby::server`
       * that cannot be recovered from.
       */
      class error : public std::runtime_error {
        public:
          /**
           * @brief Constructs the `webby::server::error` object.
           * @param[in] what_arg Explanatory string.
           */
          explicit error(const std::string& what_arg) : runtime_error(what_arg) { }

          /**
           * @brief Constructs the `webby::server::error` object.
           * @param[in] what_arg Explanatory string.
           */
          explicit error(const char* what_arg) : runtime_error(what_arg) { }
      };

      /**
       * @brief Constructor that accepts a server configuration and router.
       * @param[in] config Server configuration.
       * @param[in] router Request router.
       *
       * The configuration structure defines what hostname and port the server will listen on, and
       * provides loggers for errors and client access. The router is used to determine how each
       * request is handled, or if an error is sent back to the client.
       */
      server(const webby::config& config, const webby::router& router)
            : _config(config), _router(router) {
        _config.error_log() << qlog::debug
                            << "server::server(const webby::config&)" << std::endl;
        try {
          init();
        }
        catch(const webby::server::error& e) {
          _config.error_log() << qlog::error << e.what() << std::endl;
          throw;
        }
      }

      /**
       * @brief Destructor
       */
      ~server() {
        _config.error_log() << qlog::debug << "server::~server" << std::endl;
      }

      /**
       * @brief Runs the server.
       */
      void run() {
        _config.error_log() << qlog::debug << "server::run()" << std::endl;

        // The base implementation of the server is the simplest possible: An infinite loop that
        // blocks on the server::accept() call until a client connects.
        while(1) {
          // Accept the incoming connection and create a worker socket for it.
          net::worker worker = _server.accept();

          // Some connection logging.
          _config.error_log() << qlog::debug << "Accepted connection" << std::endl;
          _config.error_log() << qlog::debug << "  Client Hostname: " << worker.client_hostname()
              << std::endl;
          _config.error_log() << qlog::debug << "  Client IP: " << worker.client_ip() << std::endl;

          // Decompose the HTTP request from the client.
          request req(_config, worker);

          // Create the default response for the handler to populate.
          response res(_config, worker);

          // Populates some default headers.
          if(req.has_header("Host")) {
            std::ostringstream location;
            location << "http://" << req.header("Host") << req.path();
            res.set_header("Location", location.str());
          }

          // Routes the request to a handler.
          _router.dispatch(req, res);
        }
      }

    private:
      /** Server configuration. */
      const webby::config& _config;

      /**
       * @brief Request router.
       */
      const webby::router& _router;

      /**
       * @brief Initializes the server.
       */
      void init() {
        _config.error_log() << qlog::debug << "server::init()" << std::endl;
        _server.connect(_config.address(), _config.port());
        _config.error_log() << qlog::info << "Server listening at " << _config.address() << ":"
          << _config.port() << std::endl;
      }

      /**
       * @brief Server socket.
       */
      net::server _server;
  };
}
