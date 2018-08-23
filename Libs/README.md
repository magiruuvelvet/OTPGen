# Dependency Overview

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
 - QR Code Decoder/Encoder
