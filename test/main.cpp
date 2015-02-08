#include <webby.hpp>

#include <map>
#include <sstream>
#include <string>

// Example implementation of a restful web service.
class item : public webby::rest_handler<item> {
  public:
    // Responds with all of the items in a JSON array.
    void index(const webby::request& req, webby::response& res) {
      std::string s = get_all();
      res.set_status_code(200)
         .set_header("Content-Length", std::to_string(s.length()))
         .write_block(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
    }

    // Responds with a single item in JSON format.
    void show(const webby::request& req, webby::response& res) {
      // Parse the ID of the resource to respond with from the request path.
      int id = std::stoi(req.path().substr(req.route().length() + 1));
      std::string s = get_by_id(id);

      if(s.length()) {
        res.set_status_code(200)
           .set_header("Content-Length", std::to_string(s.length()))
           .write_block(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
      }
      else {
        res.set_status_code(404);
      }
    }

  private:
    // Gets all of the items in JSON format. This should really be handled by a JSON generator,
    // but for the purpose of this example it's good enough.
    std::string get_all() {
      std::stringstream ss;
      ss << "[";
      bool first = true;
      for(auto i : _item) {
        if(!first) {
          ss << ",";
          first = false;
        }
        ss << "{id:" << i.first << ", value:\"" << i.second << "\"}";
      }
      ss << "]";
      return ss.str();
    }

    // Gets a single item by its ID in JSON format. Again, this should be performed by a JSON
    // generator.
    std::string get_by_id(int id) {
      if(_item.count(id)) {
        std::stringstream ss;
        ss << "{id:" << id << ", value:\"" << _item[id] << "\"}";
        return ss.str();
      }
      return "";
    }

    static std::map<int, std::string> _item;
};

// Sample data.
std::map<int, std::string> item::_item = {
  {1, "First item"},
  {2, "Second item"}
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
  router.add("/item", webby::method::REST, item())
        .add("/", webby::method::GET | webby::method::HEAD, webby::file_handler("../include"));

  // Create the server.
  webby::server server(config, router);

  // Run the server.
  server.run();

  return 0;
}
