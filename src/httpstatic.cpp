#include "httpstatic.hpp"

#include "httpstaticfiles.hpp"

#include <httplib.h>

#include <map>
#include <string>
#include <utility>

using namespace httplib;

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
        server->GetServer().Get(file.path, [file](const Request &req, Response &res) {
            res.set_content(file.str, file.type);
        });
    }
}