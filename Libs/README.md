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

## sqlite3

 - URL: [https://sqlite.org](https://sqlite.org)
 - Status: clean, with some features enabled not existing in binary releases
 - New token database storage \[`:memory:`\] (**work in progress**) with
   transparent backwards-compatibility to the current crypto++ implementation.

## sqlite_modern_cpp

 - URL: [https://github.com/SqliteModernCpp/sqlite_modern_cpp](https://github.com/SqliteModernCpp/sqlite_modern_cpp)
 - Status: clean, except patched `sqlite3.h` path to use the bundled one
 - Used for modern C++14 and C++17 bindings for SQLite3

## PlatformFolders

 - URL: [https://github.com/sago007/PlatformFolders](https://github.com/sago007/PlatformFolders)
 - Status: clean
 - Only used in the **CLI** version
 - Alternative to `QStandardPaths` to determine OS standard paths

## boost

 - URL: [https://www.boost.org/](https://www.boost.org/)
 - Status: clean, only the `filesystem` library and required header files
 - Only used in the **CLI** version
 - Create the config directory on first start

## zlib

 - URL: [https://github.com/madler/zlib](https://github.com/madler/zlib)
 - Status: clean, except CMake; removed unneeded files
 - Optimal bundled zlib using CMake switch

## crypto++

 - URL: [https://github.com/weidai11/cryptopp](https://github.com/weidai11/cryptopp)
 - Status: organized files, removed unneeded files, build system replaced with CMake
 - Status of Crypto++ CMake: includes custom patches
 - Optimal bundled crypto++ using CMake switch

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
