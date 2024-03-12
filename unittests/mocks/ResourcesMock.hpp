#include <CppUTestExt/MockSupport.h>

class ResourcesMock
{
public:
    [[nodiscard]] static const char *getIndexHtml()
    {
        return mock("ResourcesMock").actualCall("getIndexHtml").returnStringValueOrDefault("");
    }

    [[nodiscard]] static const char *getMainJs()
    {
        return mock("ResourcesMock").actualCall("getMainJs").returnStringValueOrDefault("");
    }

    [[nodiscard]] static const char *getPicoCss()
    {
        return mock("ResourcesMock").actualCall("getPicoCss").returnStringValueOrDefault("");
    }

    [[nodiscard]] static const unsigned char *getFavicon()
    {
        auto *value
            = mock("ResourcesMock").actualCall("getFavicon").returnPointerValueOrDefault(nullptr);

        return static_cast<unsigned char *>(value);
    }

    [[nodiscard]] static unsigned int getFaviconSize()
    {
        return mock("ResourcesMock")
            .actualCall("getFaviconSize")
            .returnUnsignedIntValueOrDefault(0);
    }
};
