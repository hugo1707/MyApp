#include "ResourceLoader.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string_regex.hpp>

#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio/ssl/stream.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;

Resource Resource::from_url(const std::string &url) {
  if (!url.starts_with("http://") && !url.starts_with("https://")) {
    throw std::invalid_argument("url");
  }

  std::vector<std::string> parts;

  boost::algorithm::split_regex(parts, url, boost::regex("://"));
  if (parts.size() != 2) {
    throw std::invalid_argument("url does not have a valid scheme");
  }

  auto scheme = parts[0];
  auto port = "443";

  boost::algorithm::split_regex(parts, parts[1], boost::regex("/"));

  auto host = parts[0];
  auto path = parts.size() > 1 ? ("/" + parts[1]) : "";

  return Resource(host, port, path);
}

ResourceLoader::ResourceLoader()
    : io_context(), ssl_context(ssl::context::tlsv12_client) {
  // ssl_context.set_default_verify_paths();

  // This holds the root certificate used for verification
  // load_root_certificates(ctx);
}

HTMLResponse ResourceLoader::load_html(const std::string &url) {
  // Parse url into (host, path)
  auto resource = Resource::from_url(url);

  // These objects perform our I/O
  tcp::resolver resolver(io_context);
  beast::ssl_stream<beast::tcp_stream> stream(io_context, ssl_context);

  // Set SNI Hostname (many hosts need this to handshake successfully)
  if (!SSL_set_tlsext_host_name(stream.native_handle(), resource.host.data())) {
    beast::error_code ec{static_cast<int>(::ERR_get_error()),
                         net::error::get_ssl_category()};
    throw beast::system_error{ec};
  }

  // Look up the domain name
  auto const results = resolver.resolve(resource.host, resource.port);

  // Make the connection on the IP address we get from a lookup
  beast::get_lowest_layer(stream).connect(results);

  // Perform the SSL handshake
  stream.handshake(ssl::stream_base::client);

  // Set up an HTTP GET request message
  http::request<http::string_body> req{http::verb::get, resource.path, 11};
  req.set(http::field::host, resource.host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  // Send the HTTP request to the remote host
  http::write(stream, req);

  // This buffer is used for reading and must be persisted
  beast::flat_buffer buffer;

  // Declare a container to hold the response
  http::response<http::string_body> res;

  // Receive the HTTP response
  http::read(stream, buffer, res);

  std::cout << std::quoted(res.body()) << std::endl;

  // Gracefully close the stream
  beast::error_code ec;
  stream.shutdown(ec);
  if (ec == net::error::eof) {
    // Rationale:
    // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
    ec = {};
  }
  if (ec)
    throw beast::system_error{ec};

  // If we get here then the connection is closed gracefully
  return HTMLResponse("", {}, 200);
}
