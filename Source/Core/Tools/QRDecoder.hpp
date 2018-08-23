#ifdef OTPGEN_WITH_QR_CODES
#ifndef QRDECODER_HPP
#define QRDECODER_HPP

#include <string>

class QRDecoder
{
    QRDecoder() = delete;

public:
    static bool decode(const std::string &filename, std::string &data);
};

#endif // QRDECODER_HPP
#endif
