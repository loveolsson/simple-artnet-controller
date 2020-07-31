#pragma once

#include <nlohmann/json_fwd.hpp>
#include <restinio/all.hpp>

#include <map>
#include <memory>
#include <thread>

#define MIME_txt "text/plain; charset=utf-8"
#define MIME_html "text/html; charset=utf-8"
#define MIME_css "text/css; charset=utf-8"
#define MIME_jpeg "image/jpg"
#define MIME_png "image/png"
#define MIME_gif "image/gif"
#define MIME_svg "image/svg+xml; charset=utf-8"
#define MIME_ico "image/x-icon"
#define MIME_json "text/json; charset=utf-8"
#define MIME_pdf "application/pdf"
#define MIME_js "text/javascript; charset=utf-8"
#define MIME_wasm "application/wasm"
#define MIME_xml "application/xml; charset=utf-8"
#define MIME_xhtml "application/xhtml+xml; charset=utf-8"

using router_t    = restinio::router::express_router_t<>;
using traits_t    = restinio::traits_t<restinio::asio_timer_manager_t,
                                    restinio::single_threaded_ostream_logger_t, router_t>;
using my_server_t = restinio::http_server_t<traits_t>;

using HTTPHeaders     = std::vector<std::pair<std::string, std::string>>;
using HTTPJsonPostReq = nlohmann::json;
using HTTPJsonGetReq  = restinio::router::route_params_t;

using HTTPJsonRes    = std::tuple<nlohmann::json, restinio::http_status_line_t, HTTPHeaders>;
using HTTPJsonGetFn  = std::function<HTTPJsonRes(const HTTPJsonGetReq &)>;
using HTTPJsonPostFn = std::function<HTTPJsonRes(const HTTPJsonPostReq &)>;
using HTTPJsonGetAsyncFn =
    std::function<void(const HTTPJsonGetReq &, std::function<void(HTTPJsonRes)>)>;

class HTTPServer
{
private:
    void Run();

    std::unique_ptr<router_t> router;
    std::unique_ptr<my_server_t> server;
    std::unique_ptr<std::thread> thread;

public:
    HTTPServer();
    ~HTTPServer();

    void StartThread(int port);
    void Stop();

    restinio::router::express_router_t<> &GetRouter();
    void AttachJSONGet(const std::string &path, HTTPJsonGetFn fn);
    void AttachJSONGetAsync(const std::string &path, HTTPJsonGetAsyncFn fn);
    void AttachJSONPost(const std::string &path, HTTPJsonPostFn fn);
};

template <typename RESP>
RESP
init_resp(RESP resp)
{
    resp.append_header(restinio::http_field::server, "RESTinio sample server /v.0.2");
    resp.append_header_date_field();

    return resp;
}