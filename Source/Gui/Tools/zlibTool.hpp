#ifndef ZLIBTOOL_HPP
#define ZLIBTOOL_HPP

#include <string>

class zlibTool final
{
public:
    static const std::string uncompress(const std::string &data, bool *success = nullptr);
    static const std::string uncompress(const char *data, int size, bool *success = nullptr);

private:
    zlibTool() = delete;
};

#endif // ZLIBTOOL_HPP
