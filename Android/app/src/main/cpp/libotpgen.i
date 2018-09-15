// libotpgen JNI interface file

// package name
%module libotpgen

// include required and optimal interfaces
%include <std_string.i>
%include <std_vector.i>
%include <std_array.i>
%include <std_map.i>
%include <std_pair.i>
%include <std_except.i>

%include <inttypes.i>
%include <carrays.i>
%include <stl.i>

%include <std_shared_ptr.i>
%include <shared_ptr.i>
%include <cpointer.i>

%include <typemaps.i>
%include <various.i>
%include <arrays_java.i>

// custom interfaces
%include "java_heap.i"
%include "std_string_ref.i"

// UNIX timestamps
typedef long long time_t;

// forward include enum headers
%{
#include <OTPGenErrorCodes.hpp>
%}

// special pointer parameters
%apply OTPGenErrorCode *OUTPUT { OTPGenErrorCode *error };

// register raw pointers
%pointer_class(char, charP);
%pointer_class(OTPGenErrorCode, OTPGenErrorCodeP);

// string references
%apply std::string& OUTPUT { std::string &data };
%apply std::string& OUTPUT { std::string &out };

// register std types
%template(StringMap) std::map<std::string, std::string>;

// include libotpgen headers
%{
#include <OTPGen.hpp>
#include <OTPGenErrorCodes.hpp>
#include <Tokens.hpp>
#include <Tokens/OTPToken.hpp>
#include <Tokens/TOTPToken.hpp>
#include <Tokens/HOTPToken.hpp>
#include <Tokens/SteamToken.hpp>
#include <Tokens/AuthyToken.hpp>
#include <AppSupport.hpp>
#include <AppSupport/andOTP.hpp>
#include <AppSupport/Authy.hpp>
#include <AppSupport/Steam.hpp>
#include <otpauthURI.hpp>
#include <TokenDatabase.hpp>
#include <TokenStore.hpp>
#include <QRCode.hpp>
%}

// inline all libotpgen headers for the interface
%include <OTPGen.hpp>
%include <OTPGenErrorCodes.hpp>
%include <Tokens.hpp>
%include <Tokens/OTPToken.hpp>
%include <Tokens/TOTPToken.hpp>
%include <Tokens/HOTPToken.hpp>
%include <Tokens/SteamToken.hpp>
%include <Tokens/AuthyToken.hpp>
%include <AppSupport.hpp>
%include <AppSupport/andOTP.hpp>
%include <AppSupport/Authy.hpp>
%include <AppSupport/Steam.hpp>
%include <otpauthURI.hpp>
%include <TokenDatabase.hpp>
%include <TokenStore.hpp>
%include <QRCode.hpp>

// register smart pointers
%shared_ptr(OTPToken);

// register raw pointers
%pointer_class(std::shared_ptr<OTPToken>, OTPTokenSharedPtr);

// register vectors and maps
%template(OTPTokenList) std::vector<OTPToken*>;
%template(TOTPTokenList) std::vector<TOTPToken>;
%template(HOTPTokenList) std::vector<HOTPToken>;
%template(AuthyTokenList) std::vector<AuthyToken>;
%template(SteamTokenList) std::vector<SteamToken>;

// other templates
%template(OTPTokenPtr) std::shared_ptr<OTPToken>;
%template(TOTPTokenPtr) std::shared_ptr<TOTPToken>;
%template(HOTPTokenPtr) std::shared_ptr<HOTPToken>;
%template(SteamTokenPtr) std::shared_ptr<SteamToken>;
%template(AuthyTokenPtr) std::shared_ptr<AuthyToken>;
%template(OTPTokenPtrList) std::vector<std::shared_ptr<OTPToken>>;
