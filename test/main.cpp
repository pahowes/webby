#include <mapped_file.hpp>
#include <webby.hpp>

// For file_handler.
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

class file_handler {
  public:
    /**
     * @brief Constructs a new file_handler object.
     * @param[in] root Root path of the directory to serve files from.
     */
    file_handler(const std::string& root) : _root(root) {
    }

    /**
     * @brief Invoked by the router.
     * @param[in] req Request that triggered the use of this handler.
     * @param[out] res Response sent to the connected host.
     */
    void operator()(const webby::request& req, webby::response& res) {
      // File handler allows only the GET method.
      if(req.method() != "GET") {
        res.set_status_code(405)
           .set_reason("Method not allowed")
           .set_header("Allow", "GET");
        return;
      }

      // Appends the requested path to the root path and adds "/index.html" if the request was for
      // a directory.
      std::string path = fix_path(_root + req.path());

      try {
        mapped::file mf(path);
        mapped::buffer_t b = mf.map();
        res.set_status_code(200)
           .set_reason("OK")
           .set_header("Content-Length", std::to_string(b.second))
           .write_block(reinterpret_cast<unsigned char const*>(b.first), b.second);
      }
      catch(std::system_error const& ex) {
        if(ex.code().value() == 2) {
          res.set_status_code(404)
             .set_reason("Not found");
        }
        else {
          res.set_status_code(500)
             .set_reason("Internal server error");
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

int main() {
  // Set up the logs. As there can be only one owner for the log, std::unique_ptr is used
  // to manage its owership and lifetime.
  std::unique_ptr<qlog::logger> access_log(new qlog::logger(std::cout, qlog::severity::DEBUG));
  std::unique_ptr<qlog::logger> error_log(new qlog::logger(std::cerr, qlog::severity::DEBUG));

  // Creates the server configuration.
  webby::config config;
  config.set_address("localhost")
        .set_port(8080)
        .set_access_log(access_log)
        .set_error_log(error_log);

  // Sets up the routing table.
  webby::router router;
  router.add("/", file_handler("../include"));

  // Create the server.
  webby::server server(config, router);

  // Run the server.
  server.run();

  return 0;
}
