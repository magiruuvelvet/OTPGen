# OTPGen for Android

Multi-purpose OTP token generator written in C++ and Kotlin with encryption.
Supports TOTP, HOTP, Authy and Steam and custom digit lengths and periods.

<br>

> The Android version is in active development and does absolutely nothing
> at the moment other than loading the C++ native library.

## Features

*The app will have the same features as the Qt version which are applicable.*

**This includes:**

 - OTP Token formats
 - App Support (import/export)
 - Search functionality
 - Custom icon support
 - Load QR code images from files

**Additionally the Android version will have some features the Qt version doesn't:**

 - Scan QR codes with the device camera

<br>

#### About the encryption

Since this is my first Android app, the encryption will be limited to a
password/pin input on app start for the beginning. I'm aware of the
Android KeyStore and looking forward to use it in the future.

As a note: only the decryption password will be stored in the KeyStore.
The token database is encrypted using the native libotpgen token database
functionality and therefore is 100% compatible with the desktop version and
can be simply copy-pasted.

<br>

## Requirements

All dependencies are bundled and statically linked.

`libotpgen` works with:

 - `arm64-v8a` (*confirmed working*, my development device)
 - `armeabi-v7a`
 - `x86`
 - `x86_64`

<br>

## Building the application

#### Requirements

 - CMake 3.8+
 - Gradle 4.4+
 - [Swig](https://github.com/swig/swig) 3.0+ (autogenerate JNI binding library)
 - Kotlin Compiler (at least version 1.2.70)
 - Android Studio
 - Android SDK 21 or higher
 - Android NDK (modern version which uses Clang by default and has C++17 support) <br>
   *Library may not compile or work with GNU GCC!!*

#### Swig

Swig is a tool to automatically generate language bindings for several programming languages.
Since `libotpgen` is a larger C++ library with a lot of custom classes, writing bindings by
hand is a major pita. For this case I decided to make use of this tool.

Note that Android Studio will not autocomplete the generated package and mark everything red.
The compilation works though and a prober APK is built. Unit tests are working too. In case
you experience problems with Swig please tell me by opening a bug report.

#### Build Steps

 - Open Android Studio and import an existing Gradle project
 - Select this folder: `Android`
 - Let Android Studio do all the magic for you

#### Build options

Unlike the desktop version, the Android version doesn't have any build options.
All `libotpgen` build options are enforced to be highly portable to work on
many devices.

## Unit testing

The app has unit tests which are exclusively for the device (called instrumentation tests).
The test set currently covers the token generation. The results must be identical with the
desktop version obviously.
