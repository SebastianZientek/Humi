#pragma once

#include <FS.h>

#include <string>

class FileAdp
{
public:
    FileAdp(fs::File file)
        : m_file{file}
    {
    }

    std::string readAsString()
    {
        return m_file.readString().c_str();
    }

    void print(const std::string &str)
    {
        m_file.print(str.c_str());
    }

    void close()
    {
        m_file.close();
    }

    bool operator !()
    {
        return !m_file;
    }

private:
    fs::File m_file;
};
