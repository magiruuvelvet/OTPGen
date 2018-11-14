# OTPGen

Multi-purpose OTP token generator written in C++ and Qt with encryption and System Tray Icon.
Supports TOTP, HOTP, Authy and Steam and custom digit lengths and periods.

<br>

> This application is still in development, but the only missing mandatory bit is HOTP
> in the GUI. The counter isn't increased at all right now. If you don't need HOTP than
> this application is stable enough already for production use.

<br>

### Important Notice

> The `master` branch currently doesn't have a working GUI anymore. It was removed due
> to the new storage format based on SQLite3. The GUI needs a rewrite anyway and since the
> format and the API changed too much, it is now broken. It is recommended to use the
> last stable release `v0.9.3` for now for working production builds.

<br>

## Features

 - Can generate tokens for TOTP, HOTP, Authy and Steam
 - Supports custom digit lengths and periods <br>
   Non-standard OTPs may not use either 6 or 8 digits. Example: Authy (7 digits and 10 seconds)
 - System Tray Icon
 - [Qt Keychain](https://github.com/frankosterfeld/qtkeychain) Integration
   - KWallet, GNOME Keyring, OS X Keychain, Windows Credential Store
 - Clean interface
 - Import token secrets from other applications
   - andOTP (supports both: plaintext and encrypted backups)
   - Authy (supports both: xml and json input)
   - SteamGuard
 - Import tokens from QR Code images
   - Supported formats are: PNG, JPG and SVG (experimental)
 - Export your tokens to other applications
   - andOTP (supports both: plaintext and encrypted backups)
 - Search your tokens with regular expressions in the search bar and never lose
   time because of a huge token database
 - Copy tokens to clipboard without revealing them in the UI
 - Copy tokens straight from the system tray menu without even opening the UI at all
 - Custom icons to better recognize your tokens


## Planned

 - Export
   - `otpauth:` uri
   - QR Code

 - **Refactor/Rewrite GUI !!!**
 - ~~Use Model/View instead of a tedious hard-to-manage Widget~~
   - ~~Most problematic by now is getting the correct/current row~~
 - GUI is now broken due to the new storage format and a rewrite
   must be done now. Use `v0.9.3` for a stable production release.

 - User Settings

<br>

## Requirements

 - [crypto++](https://cryptopp.com/) <br>
   **Hint:** this library is also bundled and can be statically linked.
   see CMake build options for more details.

<br>

**Optimal Requirements based on features**

 - Graphical User Interface
   - Qt 5 (Core, Gui, Widgets, Xml, Network)
   - zlib (semi-optimal, *see CMake build options*)

 - Qt Keychain Integration
   - *Graphical User Interface*
   - Qt 5 D-Bus (only when using Qt Keychain with KWallet)
   - [libsecret](https://wiki.gnome.org/Projects/Libsecret) (for the bundled Qt Keychain library)


## Building the application

#### Requirements

 - CMake 3.8+
 - Qt build tools (qmake, moc, rcc) - *only for the GUI module* (see CMake build options)

#### Build Steps

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Make sure to build a **Release** build!! The Debug build uses a hardcoded test password
to avoid entering a password all the time during development. Never use real token secrets
during development.

This application uses shared libraries. Make sure to add `${CMAKE_BINARY_DIR}/lib` to the
library path. Most IDEs have a CMake run configurations to do that on a per project basis.

#### Build options

 - `-DDISABLE_GUI=ON` (default *OFF*): disables building of the graphical user interface.
   Note that the CLI isn't functional yet.

 - `-DDISABLE_CLI=ON` (default *OFF*): disables building of the console application.

 - `-DDISABLE_QTKEYCHAIN=ON` (default *OFF*): disables the Qt Keychain integration.

 - `-DUNIT_TESTING=ON` (default *OFF*): enables building of the unit tests. (*recommended*)

 - `-DBUILD_MIGRATION_TOOL=ON` (default *OFF*): builds the migration tool (see below)

 - `-DWITH_QR_CODES=ON` (default *ON*): enables support for decoding and encoding QR Code images.
   Note that webcam scanning isn't supported and not planned.

 - `-DBUNDLED_ZLIB=ON` (default *OFF*): use the bundled zlib library instead of the system-installed
   one. recommended for portable builds.

 - `-DBUNDLED_CRYPTOPP=ON` (default *OFF*): use the bundled crypto++ library instead of the system-installed
   one. recommended for portable builds.

<br>

**Hint**

Disable both, the GUI and CLI to build only the library.

<br>

### Building the unit tests

This application uses the [**bandit**](https://github.com/banditcpp/bandit) unit testing framework.
It is recommended to run the unit tests first if you experience issues before reporting a bug.

```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TESTING=ON ..
make
```

After compilation copy the `otpgen-tests` binary to the `Tests` directory of this repository
and run it. Alternatively if you are using an IDE, just set the working directory of the
test binary to the `Tests` directory. This is required for external files to be found.

To get more detailed output run the unit test binary with the `--reporter=info` option.

*Unit tests are disabled by default.*


## Tips

Looking for high quality SVG icons? Check out this amazing repository with over 1000 icons:

 - [gilbarbara/logos](https://github.com/gilbarbara/logos)


<br>

### Migrating your old database

Make sure you enable the build option to compile the migration tool. Once this is done
you can convert your old token data (`v0.9.3` and below) to the new format (after `v0.9.3`).

> `$ otpgen-migratedb /path/to/your/database tokens.db`

Place the converted database into the same location as the old database and make sure
the filename is `tokens.db` for the application to recognize it.

You should be able to find your old database in the following directories:

 - [XDG](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html) (Linux, BSD): `$XDG_CONFIG_HOME/マギルゥーベルベット/OTPGen`
 - Mac: `$HOME/Library/Application Support/マギルゥーベルベット/OTPGen`
 - Windows: `%USERPROFILE%\AppData\Roaming\マギルゥーベルベット\OTPGen`

<br>

## Screenshots

> Screenshots may be out of date.

![Main Window](./.screenshots/MainWindow.png "Main Window")

![Add Tokens](./.screenshots/AddTokens.png "Add Tokens")
