#include <webby.hpp>

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
  router.add("/",
             webby::method::GET | webby::method::HEAD,
             webby::file_handler("../include"));

  // Create the server.
  webby::server server(config, router);

  // Run the server.
  server.run();

  return 0;
}
