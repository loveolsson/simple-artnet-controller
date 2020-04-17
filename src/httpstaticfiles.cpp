#include "httpstaticfiles.hpp"

#include "httpstatic_hack.hpp"

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#include <incbin.h>

INCBIN(IndexHtml, "index.html");
INCBIN(SiteJs, "site.js");
INCBIN(BootstrapMinCss, "bootstrap.min.css");

const std::string fileIndexHtml((char *)gIndexHtml_data, gIndexHtml_size);
const std::string fileSiteJs((char *)gSiteJs_data, gSiteJs_size);
const std::string fileBootstrapMinCss((char *)gBootstrapMinCss_data, gBootstrapMinCss_size);