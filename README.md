# OTPGen

Multi-purpose OTP token generator written in C++ and Qt with encryption and System Tray Icon.
Supports TOTP, HOTP, Authy and Steam and custom digit lengths and periods.

<br>

> This application is still in development, but already somewhat usable.

<br>

## Features

 - Can generate tokens for TOTP, HOTP, Authy and Steam
 - Supports custom digit lengths and periods <br>
   Non-standard OTPs may not use either 6 or 8 digits. Example: Authy (7 digits and 10 seconds)
 - System Tray Icon
 - Clean interface
 - Import token secrets from other applications
   - andOTP
   - Authy
   - SteamGuard
 - Search your tokens with regular expressions in the search bar and never lose
   time because of a huge token database

> Note that the import feature is not implemented yet. The UI will only report false errors.

## Planned

 - Export
 - Copy to clipboard without using the "Show" checkbox

<br>

## Requirements

 - Qt 5
 - libgcrypt (for the bundled OTP generation library written in C)
 - crypto++ (for the database encryption)



<br>

## Screenshots

![Main Window](./.screenshots/MainWindow.png "Main Window")

![Add Tokens](./.screenshots/AddTokens.png "Add Tokens")
