#include "Resources.hpp"

// #define INCBIN_OUTPUT_SECTION ".irom.text"
#include <Arduino.h>
#include <incbin.h>

INCTXT(IndexHtml, "src/html/index.html");
INCTXT(MainJs, "src/html/main.js");
INCTXT(PicoCss, "src/html/pico.min.css");
INCBIN(Favicon, "src/html/fav.png");

const char *Resources::getIndexHtml()
{
    return reinterpret_cast<const char *>(gIndexHtmlData);  // NOLINT
}

const char *Resources::getMainJs()
{
    return reinterpret_cast<const char *>(gMainJsData);  // NOLINT
}

const char *Resources::getPicoCss()
{
    return reinterpret_cast<const char *>(gPicoCssData);  // NOLINT
}

const unsigned char *Resources::getFavicon()
{
    return gFaviconData;
}

unsigned int Resources::getFaviconSize()
{
    return gFaviconSize;
}
