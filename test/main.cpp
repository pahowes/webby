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

      // Combine the root path with the request path 
      std::string path = _root + req.path();

      // If the path specifies a directory, append "index.html" to it.
      struct stat st;
      ::lstat(path.c_str(), &st);
      if(S_ISDIR(st.st_mode)) {
        if(path[path.length() - 1] != '/') {
          path += "/";
        }
        path += "index.html";
      }
      
      unsigned long size = file_size(path);
      if(size > 0) {
        int fd = ::open(path.c_str(), O_RDONLY);
        if(fd > 0) {
          void* data = ::mmap(nullptr, size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
          if(data) {
            res.set_status_code(200)
               .set_reason("OK")
               .set_header("Content-Length", std::to_string(size))
               .write_block(reinterpret_cast<unsigned char*>(data), size);
            ::munmap(data, size);
          }
          else {
            std::cerr << "* Error (" << errno << ") when mapping file: " << path << std::endl;
            res.set_status_code(500)
               .set_reason("Internal server error");
          }
          ::close(fd);
        }
        else {
          std::cerr << "* Error (" << errno << ") when opening file: " << path << std::endl;
          if(errno == 2) {
            res.set_status_code(404)
               .set_reason("Not found");
          }
          else {
            res.set_status_code(500)
               .set_reason("Internal server error");
          }
        }
      }
      else {
        res.set_status_code(404)
           .set_reason("Not Found");
      }
    }

  private:
    /**
     * @brief Returns the size of the file at @p path.
     * @param[in] path Path of a file.
     * @returns Size of the file found at @p path.
     */
    unsigned long file_size(const std::string& path) {
      std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
      return unsigned(long(file.tellg()));
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
