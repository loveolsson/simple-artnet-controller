#pragma once

#include <nlohmann/json_fwd.hpp>

#include <map>
#include <memory>
#include <thread>

#define MIME_txt "text/plain"
#define MIME_html "text/html"
#define MIME_css "text/css"
#define MIME_jpeg "image/jpg"
#define MIME_png "image/png"
#define MIME_gif "image/gif"
#define MIME_svg "image/svg+xml"
#define MIME_ico "image/x-icon"
#define MIME_json "application/json"
#define MIME_pdf "application/pdf"
#define MIME_js "application/javascript"
#define MIME_wasm "application/wasm"
#define MIME_xml "application/xml"
#define MIME_xhtml "application/xhtml+xml"

namespace httplib {
class Server;
}

using HTTPHeaders     = std::vector<std::pair<std::string, std::string>>;
using HTTPJsonReq     = nlohmann::json;
using HTTPJsonRes     = const std::tuple<nlohmann::json, int, HTTPHeaders>;
using HTTPJsonFn      = std::function<HTTPJsonRes(const HTTPJsonReq &)>;
using HTTPJsonAsyncFn = std::function<void(const HTTPJsonReq &, std::function<void(HTTPJsonRes)>)>;

class HTTPServer
{
private:
    void Run();

    std::unique_ptr<httplib::Server> server;
    std::unique_ptr<std::thread> thread;

public:
    HTTPServer();
    ~HTTPServer();

    void StartThread(int port);
    void Stop();

    httplib::Server &GetServer();
    void AttachJSONGet(const std::string &path, HTTPJsonFn fn);
    void AttachJSONGetAsync(const std::string &path, HTTPJsonAsyncFn fn);
    void AttachJSONPost(const std::string &path, HTTPJsonFn fn);
};
