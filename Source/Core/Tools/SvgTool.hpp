#ifdef OTPGEN_GUI
#ifndef SVGTOOL_HPP
#define SVGTOOL_HPP

#include <string>

class SvgTool final
{
    SvgTool() = delete;

public:
    static void changeFillColor(std::string &svg, const std::string &color);
};

#endif // SVGTOOL_HPP
#endif
