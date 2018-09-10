#ifndef QRCODE_HPP
#define QRCODE_HPP

#include <string>

class QRCode
{
    QRCode() = delete;

public:
    // input from file, output to memory buffer
    static bool decode(const std::string &filename, std::string &data);

    // input from memory buffer, output to memory buffer
    static bool encode(const std::string &input, std::string &out);
};

#endif // QRCODE_HPP
