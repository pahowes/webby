#pragma once

/**
 * @namespace webby
 */
namespace webby {
  /**
   * @brief Base class used to implement a standard RESTful resource handler.
   */
  template<typename T> class rest_handler {
    public:
      /**
       * @brief Invoked by the router to handle a request.
       * @param[in] req Request that triggered the use of this handler.
       * @param[out] res Response sent to the connected host.
       */
      void operator()(const webby::request& req, webby::response& res) {
        // Returns a 501 "Not Implemented" response by default unless the subclass overrides it.
        res.set_status_code(501);
        switch(req.method()) {
          case method::DELETE:
            static_cast<T*>(this)->destroy(req, res);
            break;
          case method::GET:
            if(req.path() == req.route()) {
              static_cast<T*>(this)->index(req, res);
            }
            else {
              static_cast<T*>(this)->show(req, res);
            }
            break;
          case method::POST:
            static_cast<T*>(this)->create(req, res);
            break;
          case method::PUT:
            static_cast<T*>(this)->update(req, res);
            break;
        }
      }

      /**
       * @brief Responds with a collection of resources
       * @param[in] req Request for a collection of resources.
       * @param[out] res Response that contains the collection of resources.
       *
       * This method is invoked by a request in the form, "GET /path"
       */
      void index(const webby::request&, webby::response&) { }

      /**
       * @brief Responds with a single resource.
       * @param[in] req Request for a single resource.
       * @param[out] res Response that contains the resource.
       *
       * This method is invoked by a request in the form, "GET /path/{id}"
       */
      void show(const webby::request&, webby::response&) { }

      /**
       * @brief Responds with the resource that was created.
       * @param[in] req Request that contains the information about the resource to create.
       * @param[out] res Response that contains the resource that was created.
       *
       * This method is invoked by a request in the form, "POST /path"
       */
      void create(const webby::request&, webby::response&) { }

      /**
       * @brief Responds with the updated resource
       * @param[in] req Request that contains the information in the resource to update.
       * @param[out] res Response that contains the updated resource.
       *
       * This method is invoked by a request in the form, "PUT /path/{id}"
       */
      void update(const webby::request&, webby::response&) { }

      /**
       * @brief Responds with the resource that was deleted.
       * @param[in] req Request with the ID of the resource to delete.
       * @param[out] res Response that contains the resource that was deleted.
       *
       * This method is invoked by a request in the form, "DELETE /path/{id}"
       */
      void destroy(const webby::request&, webby::response&) { }
  };
}

