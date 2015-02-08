#pragma once

#include <mapped_file.hpp>
#include <webby/method.hpp>

namespace webby {
  class file_handler {
    public:
      /**
       * @brief Constructs a new file_handler object.
       * @param[in] root Root path of the directory to serve files from.
       */
      file_handler(const std::string& root) : _root(root) { }

      /**
       * @brief Invoked by the router.
       * @param[in] req Request that triggered the use of this handler.
       * @param[out] res Response sent to the connected host.
       */
      void operator()(const webby::request& req, webby::response& res) {
        // Appends the requested path to the root path and adds "/index.html" if the request was for
        // a directory.
        std::string path = fix_path(_root + req.path());

        try {
          mapped::file mf(path);
          mapped::buffer_t b = mf.map();
          res.set_status_code(200)
             .set_header("Content-Length", std::to_string(b.second))
             .write_block(reinterpret_cast<unsigned char const*>(b.first), b.second);
        }
        catch(std::system_error const& ex) {
          if(ex.code().value() == 2) {
            res.set_status_code(404);
          }
          else {
            res.set_status_code(500);
          }
        }
      }

    private:
      /**
       * @brief Appends "/index.html" to the path if it specified a directory.
       * @param[in] path Path to fix.
       * @return Fixed path.
       */
      std::string fix_path(std::string path) {
        struct stat st;
        ::stat(path.c_str(), &st);
        if(S_ISDIR(st.st_mode)) {
          if(path[path.length() - 1] != '/') {
            path += "/";
          }
          path += "index.html";
        }
        return path;
      }

      /**
       * @brief Root path of the served directory.
       */
      const std::string _root;
  };
}
