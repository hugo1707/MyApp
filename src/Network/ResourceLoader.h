#pragma once

#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

namespace net = boost::asio;
namespace ssl = net::ssl;

struct Resource {
  std::string host;
  std::string port;
  std::string path;

  Resource(std::string host, std::string port, std::string path)
      : host(std::move(host)), port(std::move(port)), path(std::move(path)) {}

  [[nodiscard]] std::string to_string() const {
    return fmt::format("Resource(host: {}, path: {})", host, path);
  }

  bool is_https() const { return port == "443"; }

  static Resource from_url(const std::string &url);
};

using headers_map = std::unordered_map<std::string, std::string>;

struct HTMLResponse {
  std::string body;
  headers_map headers;
  int status;

  HTMLResponse(std::string body, headers_map headers, int status)
      : body(std::move(body)), headers(std::move(headers)), status(status) {}
};

class ResourceLoader {
public:
  ResourceLoader();

  HTMLResponse load_html(const std::string &url);

private:
  net::io_context io_context;
  ssl::context ssl_context;
};
