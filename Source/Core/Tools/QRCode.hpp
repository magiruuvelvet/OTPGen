#ifdef OTPGEN_WITH_QR_CODES
#ifndef QRCODE_HPP
#define QRCODE_HPP

#include <string>

class QRCode
{
    QRCode() = delete;

public:
    static bool decode(const std::string &filename, std::string &data);
    static bool encode(const std::string &input, std::string &out);
};

#endif // QRCODE_HPP
#endif
