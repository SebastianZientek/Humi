#include "Resources.hpp"

// #define INCBIN_OUTPUT_SECTION ".irom.text"
#include <Arduino.h>
#include <incbin.h>

INCTXT(IndexHtml, "src/html/index.html");
INCTXT(PicoCss, "src/html/pico.min.css");
INCBIN(Favicon, "src/html/fav.png");

const char *Resources::getIndexHtml()
{
    return (char*)gIndexHtmlData;
}

const char *Resources::getPicoCss()
{
    return (char*)gPicoCssData;
}

const unsigned char *Resources::getFavicon()
{
    return gFaviconData;
}

unsigned int Resources::getFaviconSize()
{
    return gFaviconSize;
}
