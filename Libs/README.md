# Dependency Overview

## bandit

 - URL: [https://github.com/banditcpp/bandit](https://github.com/banditcpp/bandit)
 - Status: clean
 - Portable header-only Unit Testing Framework
 - Required to run the unit tests of this application

## cereal

 - URL: [https://github.com/USCiLab/cereal](https://github.com/USCiLab/cereal)
 - Status: clean
 - Binary serialization
 - Used to save/load the token database

## PlatformFolders

 - URL: [https://github.com/sago007/PlatformFolders](https://github.com/sago007/PlatformFolders)
 - Status: clean
 - Only used in the **CLI** version
 - Alternative to `QStandardPaths` to determine OS standard paths

## boost

 - URL: https://www.boost.org/
 - Status: clean, only the `filesystem` library and required header files
 - Only used in the **CLI** version
 - Create the config directory on first start

## replxx

 - URL: https://github.com/AmokHuginnsson/replxx
 - Status: clean
 - *Planned to use for the CLI version*

## qtkeychain

 - URL: [https://github.com/frankosterfeld/qtkeychain](https://github.com/frankosterfeld/qtkeychain)
 - Status: includes custom patches
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
