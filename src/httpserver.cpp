#include "httpserver.hpp"

#include <nlohmann/json.hpp>
#include <restinio/all.hpp>

#include <iostream>
#include <thread>

using namespace nlohmann;

void
HTTPServer::Run()
{
    restinio::run(
        restinio::on_thread_pool(4, restinio::skip_break_signal_handling(), *this->server));
}

HTTPServer::HTTPServer()
    : router(std::make_unique<router_t>())
{
}

HTTPServer::~HTTPServer()
{
}

void
HTTPServer::StartThread(int port)
{
    this->server =
        std::make_unique<my_server_t>(restinio::own_io_context(), [this, port](auto &settings) {
            settings.port(port);
            settings.address("0.0.0.0");
            settings.request_handler(std::move(this->router));
        });

    this->thread = std::make_unique<std::thread>(&HTTPServer::Run, this);
}

void
HTTPServer::Stop()
{
    if (this->server) {
        restinio::initiate_shutdown(*this->server);
        if (this->thread->joinable()) {
            this->thread->join();
        }
    }
}

router_t &
HTTPServer::GetRouter()
{
    return *this->router;
}

void
HTTPServer::AttachJSONGet(const std::string &path, HTTPJsonGetFn fn)
{
    this->router->http_get(path, [fn](std::shared_ptr<restinio::request_t> req,
                                      restinio::router::route_params_t params) {
        auto ret = fn(params);
        auto res = init_resp(req->create_response(std::get<1>(ret)))
                       .append_header("Content-Type", MIME_json)
                       .set_body(std::get<0>(ret).dump());

        for (auto &h : std::get<2>(ret)) {
            res.append_header(h.first.c_str(), h.second.c_str());
        }

        res.done();

        return restinio::request_accepted();
    });
}

void
HTTPServer::AttachJSONGetAsync(const std::string &path, HTTPJsonGetAsyncFn fn)
{
    this->router->http_get(path.c_str(), [fn](std::shared_ptr<restinio::request_t> req,
                                              restinio::router::route_params_t params) {
        fn(params, [req](const HTTPJsonRes &ret) {
            auto res = init_resp(req->create_response(std::get<1>(ret)))
                           .append_header("Content-Type", MIME_json);
            res.set_body(std::get<0>(ret).dump());

            for (auto &h : std::get<2>(ret)) {
                res.append_header(h.first.c_str(), h.second.c_str());
            }

            res.done();
        });

        return restinio::request_accepted();
    });
}

void
HTTPServer::AttachJSONPost(const std::string &path, HTTPJsonPostFn fn)
{
    this->router->http_post(path.c_str(), [fn](std::shared_ptr<restinio::request_t> req,
                                               restinio::router::route_params_t params) {
        try {
            auto body = json::parse(req->body());
            auto ret  = fn(body);
            auto res  = init_resp(req->create_response(std::get<1>(ret)))
                           .append_header("Content-Type", MIME_json)
                           .set_body(std::get<0>(ret).dump());
            for (auto &h : std::get<2>(ret)) {
                res.append_header(h.first.c_str(), h.second.c_str());
            }
            res.done();
            return restinio::request_accepted();

        } catch (json::exception &ex) {
        }

        return restinio::request_rejected();
    });
}
