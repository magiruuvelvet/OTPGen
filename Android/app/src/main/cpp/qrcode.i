%include <std_string.i>

%{
#include <QRCode.hpp>
%}

// only generate getters, this variables are not supposed to be changed
%immutable;

%inline %{
  struct QRCodeDecodeResult {
    bool status;
    std::string data;
  };

  QRCodeDecodeResult QRCode_decode(const std::string &filename) {
    QRCodeDecodeResult ret;
    ret.status = QRCode::decode(filename, ret.data);
    return ret;
  }
%}

%ignore QRCode;
%ignore QRCode::decode;
%ignore QRCode::encode;

%include <QRCode.hpp>

%mutable;
