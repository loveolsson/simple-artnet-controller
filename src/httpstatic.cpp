#include "httpstatic.hpp"

#include "httpstaticfiles.hpp"

#include <restinio/all.hpp>

#include <map>
#include <string>
#include <utility>

using namespace restinio;

struct StaticFile {
    const char *path;
    const std::string &str;
    const char *type;
};

void
InitStaticFiles(HTTPServer *server)
{
    const std::vector<StaticFile> files = {
        {"/", fileIndexHtml, MIME_html},
        {"/site.js", fileSiteJs, MIME_js},
        {"/bootstrap.min.css", fileBootstrapMinCss, MIME_css},
    };

    for (const auto &file : files) {
        server->GetRouter().http_get(file.path, [file](auto req, auto) {
            init_resp(req->create_response())
                .append_header("Content-Type", file.type)
                .set_body(file.str)
                .done();

            return restinio::request_accepted();
        });
    }
}