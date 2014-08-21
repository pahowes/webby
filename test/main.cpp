#include <webby.hpp>

int main() {
  // Set up the logs.
  qlog::logger access_log(std::cout, qlog::severity::DEBUG);
  qlog::logger error_log(std::cerr, qlog::severity::DEBUG);

  // Configure the server.
  webby::config config;
  config.set_address("localhost")
        .set_port(8080)
        .set_access_log(access_log)
        .set_error_log(error_log);

  webby::server server(config);

  return 0;
}
