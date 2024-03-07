#pragma once

class Resources
{
public:
    [[nodiscard]] static const char *getIndexHtml();
    [[nodiscard]] static const char *getPicoCss();
    [[nodiscard]] static const unsigned char *getFavicon();
    [[nodiscard]] static unsigned int getFaviconSize();
};
