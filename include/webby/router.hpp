/**
 * @file router.hpp
 */
#pragma once
#include <vector>

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

      /**
       * @brief Default constructor.
       */
      router() {
        _error_handler = router::default_error_handler;
      }

      /**
       * @brief Adds a new route to the table.
       */
      router& add(const std::string& path, enum webby::method mask, handler_t handler) {
        _route.push_back(route{path, mask, handler});
        return *this;
      }

      /**
       * @brief Routes a request to the appropriate handler.
       */
      void dispatch(request& req, response& res) const {
        for(auto itr = _route.cbegin(); itr != _route.cend(); ++itr) {
          if(req.path().compare(0, itr->path.length(), itr->path) == 0) {
            if(req.method() == (req.method() & itr->mask)) {
              req.set_route(itr->path);
              itr->handler(req, res);
            }
            else {
              res.set_status_code(405)
                 .set_header("Allow", to_string(itr->mask));
            }
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
        res.set_status_code(404);
      }

    private:
      /**
       * @brief The structure of a route.
       */
      struct route {
        /**
         * @brief base path to match.
         */
        std::string path;

        /**
         * @brief Mask of the HTTP methods that the route will accept.
         */
        enum webby::method mask;

        /**
         * @brief Function that handles processing for the route.
         */
        handler_t handler;
      };

      /**
       * @brief Stores routes and handlers.
       */
      std::vector<route> _route;

      /**
       * @brief Stores the error handler.
       */
      handler_t _error_handler;
  };
}
