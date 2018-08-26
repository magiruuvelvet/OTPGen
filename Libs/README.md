# Dependency Overview

## bandit

 - URL: [https://github.com/banditcpp/bandit](https://github.com/banditcpp/bandit)
 - Portable header-only Unit Testing Framework
 - Required to run the unit tests of this application

## cereal

 - URL: [https://github.com/USCiLab/cereal](https://github.com/USCiLab/cereal)
 - Status: clean
 - Binary serialization
 - Used to save/load the token database

## libbaseencode

 - URL: [https://github.com/paolostivanin/libbaseencode](https://github.com/paolostivanin/libbaseencode)
 - Status: clean
 - Dependency of **libcotp**
 - Base-32/Base-64 encoding and decoding

## libcotp

 - URL: [https://github.com/magiruuvelvet/libcotp](https://github.com/magiruuvelvet/libcotp)
 - Status: includes custom patches
 - Generates OTP codes

## qtkeychain

 - URL: [https://github.com/frankosterfeld/qtkeychain](https://github.com/frankosterfeld/qtkeychain)
 - Status: clean, except cmake files
 - Native OS Keychain Integration (store/receive token database password from keychain)

## zxing-cpp

 - URL: [https://github.com/glassechidna/zxing-cpp](https://github.com/glassechidna/zxing-cpp)
 - Status: clean, except cmake files
 - QR Code Decoder (only decodes raw pixel data)
 - Doesn't support encoding

## QRCodeGenerator

 - URL: [https://github.com/nayuki/QR-Code-generator](https://github.com/nayuki/QR-Code-generator)
 - Status: clean, C++ version
 - QR Code Encoder (only encodes text/binary data)
 - Doesn't support decoding

## qtsingleapplication / qtlockedfile

 - Code taken from Qt Creator 4.7.0 source code
 - Status: modified, removed namespace
 - Single instance lock with message sending
