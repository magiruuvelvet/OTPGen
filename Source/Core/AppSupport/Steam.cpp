#include "Steam.hpp"

#include <iostream>

#include <TokenDatabase.hpp>

#include <cereal/external/rapidjson/document.h>
#include <cereal/external/rapidjson/memorystream.h>

#include <Tools/otpauthURI.hpp>

// SteamGuard JSON schema
//
// {
//     "steamid": "",
//     "shared_secret": "",     <-- base-64 encoded secret
//     "serial_number": "",
//     "revocation_code": "",
//     "uri": "",               <-- otpauth uri, contains base-32 encoded secret of "shared_secret"
//     "server_time": "",
//     "account_name": "",
//     "token_gid": "",
//     "identity_secret": "",
//     "secret_1": "",
//     "status": 0,
//     "steamguard_scheme": ""
// }

namespace AppSupport {

bool Steam::importFromSteamGuard(const std::string &file, SteamToken &target)
{
    // read file contents into memory
    std::string out;
    auto status = TokenDatabase::readFile(file, out);
    if (status != TokenDatabase::Success)
    {
        out.clear();
        return false;
    }

    if (out.empty())
    {
        return false;
    }

    // parse json
    try {
        rapidjson::StringStream s(out.c_str());
        rapidjson::Document json;
        json.ParseStream(s);

        // root element must be an object
        if (!json.IsObject())
        {
            return false;
        }

        auto object = json.GetObject();

        // check if object has all required members for import
        if (!(object.HasMember("steamid") &&    // use this member to check if file is really a SteamGuard json
            object.HasMember("shared_secret")))
        {
            return false;
        }

        // try to import base-64 secret
        if (!target.importBase64Secret(object["shared_secret"].GetString()))
        {
            // if that fails parse the URI and use the base-32 secret directly
            if (object.HasMember("uri"))
            {
                otpauthURI uri(object["uri"].GetString());
                if (!uri.valid())
                {
                    return false;
                }

                target.setSecret(uri.secret());
            }
            else
            {
                return false;
            }
        }
    } catch (...) {
        // catch all rapidjson exceptions
        return false;
    }

    return true;
}

}
