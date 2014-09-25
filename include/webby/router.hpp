/**
 * @file router.hpp
 */
#pragma once

/**
 * @namespace webby
 */
namespace webby {
  /**
   * @brief Routes request to the correct handler.
   */
  class router {
    public:
      /**
       * @brief Signature of route handler functions.
       */
      typedef std::function<void(const request& ,response&)> handler_t;

      router() {
        _error_handler = router::default_error_handler;
      }

      /**
       * @brief Adds a new route to the table.
       */
      router& add(const std::string& path, handler_t handler) {
        _route[path] = handler;
        return *this;
      }

      /**
       * @brief Routes a request to the appropriate handler.
       */
      void route(const webby::request& req, webby::response& res) const {
        for(auto itr = _route.cbegin(); itr != _route.cend(); ++itr) {
          if(req.path().compare(0, itr->first.length(), itr->first) == 0) {
            itr->second(req, res);
            return;
          }
        }

        _error_handler(req, res);
      }

      /**
       * @brief Sets the handler invoked when a route cannot be found for the request.
       * @param[in] handler The handler to invoke.
       * @returns Reference to this webby::router object for chaining.
       *
       * The handler installed to generate the default 404 error responses can be replaced by
       * calling this function.
       */
      router& set_error_handler(handler_t handler) {
        _error_handler = handler;
        return *this;
      }

    protected:
      /**
       * @brief Default error handler.
       * @param[in] req Request that caused the error.
       * @param[in] res Response sent back to the client with the error.
       *
       * The error handler sends a "404 Not Found" error through the response to inform the
       * connected host that none of the routes matched the request.
       */
      static void default_error_handler(const request& req, response& res) {
        (void)(req);
        res.set_status_code(404)
           .set_reason("Not Found");
      }

    private:
      /**
       * @brief Stores routes and handlers.
       */
      std::map<std::string, handler_t> _route;

      /**
       * @brief Stores the error handler.
       */
      handler_t _error_handler;
  };
}
