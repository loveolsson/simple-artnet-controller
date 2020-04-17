#include "httpserver.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <thread>

using namespace httplib;
using namespace nlohmann;

void
HTTPServer::Run()
{
    this->server->listen_after_bind();
}

HTTPServer::HTTPServer()
    : server(std::make_unique<Server>())
{
}

HTTPServer::~HTTPServer()
{
}

void
HTTPServer::StartThread(int port)
{
    this->Stop();

    if (!this->server->bind_to_port("0.0.0.0", port)) {
        port = this->server->bind_to_any_port("0.0.0.0");
    }

    this->thread = std::make_unique<std::thread>(&HTTPServer::Run, this);
}

void
HTTPServer::Stop()
{
    this->server->stop();

    if (this->thread) {
        std::cout << "Stopping HTTP server...";

        if (this->thread->joinable()) {
            this->thread->join();
        }

        std::cout << "done." << std::endl;

        this->thread.reset();
    }
}

Server &
HTTPServer::GetServer()
{
    return *this->server;
}

void
HTTPServer::AttachJSONGet(const std::string &path, HTTPJsonFn fn)
{
    this->server->Get(path.c_str(), [fn](const Request &req, Response &res) {
        auto body = json::object();

        for (const auto &param : req.params) {
            body[param.first] = param.second;
        }

        auto ret = fn(body);
        res.set_content(std::get<0>(ret).dump(), MIME_json);
        res.status = std::get<1>(ret);
        for (auto &h : std::get<2>(ret)) {
            res.set_header(h.first.c_str(), h.second.c_str());
        }
    });
}

void
HTTPServer::AttachJSONPost(const std::string &path, HTTPJsonFn fn)
{
    this->server->Post(path.c_str(), [fn](const Request &req, Response &res) {
        try {
            auto body = json::parse(req.body);
            auto ret  = fn(body);
            res.set_content(std::get<0>(ret).dump(), MIME_json);
            res.status = std::get<1>(ret);
            for (auto &h : std::get<2>(ret)) {
                res.set_header(h.first.c_str(), h.second.c_str());
            }
        } catch (json::exception &ex) {
            res.set_content(ex.what(), MIME_txt);
            res.status = 500;
        }
    });
}
