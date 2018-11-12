#include "TokenDatabase.hpp"

#include <fstream>
#include <ostream>
#include <sstream>
#include <memory>

#include <sqlite/sqlite3.h>
#include <sqlite_modern_cpp.h>

#include <cryptopp/cryptlib.h>
#include <cryptopp/algparam.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace {
    // database version, used for possible migrations
    static const std::uint32_t DATABASE_VERSION = 0x0f000005;

    // SQLite3 connection handle
    static std::shared_ptr<sqlite::database> db;
    static bool db_status;
    static std::string db_data;
}

std::string TokenDatabase::databasePassword;
std::string TokenDatabase::databasePath;

std::vector<OTPToken::sqliteSortOrder> TokenDatabase::order;

const std::string TokenDatabase::getErrorMessage(const Error &error)
{
    switch (error)
    {
        case Success: return {};

        case FileReadFailure:     return "Unable to read file.";
        case FileWriteFailure:    return "Unable to write file.";
        case FileEmpty:           return "File is empty.";
        case InvalidTokenFile:    return "Not a valid token database.";

        case EncryptionFailure:   return "Failed to encrypt data.";
        case DecryptionFailure:   return "Failed to decrypt data.";
        case InvalidCiphertext:   return "Failed to decrypt data. Either the password is incorrect or the file is corrupt.";
        case PasswordEmpty:       return "Password is empty.";
        case PasswordHashFailure: return "Failed to hash password.";

        case SqlDatabaseNotOpen:           return "Database is not connected.";
        case SqlMemoryAllocationError:     return "Failed to allocate memory for database.";
        case SqlSerializationError:        return "Failed to serialize the database for encryption.";
        case SqlDeserializationError:      return "Failed to deserialize the database after decrypting it.";
        case SqlSystemTableCreationError:  return "Failed to create system tables.";
        case SqlStaticRecordCreationError: return "Failed to create system records.";
        case SqlStatementPrepareFailed:    return "Failed to prepare SQL statement.";
        case SqlBindBinaryDataFailed:      return "Faled to bind binary data to SQL statement.";
        case SqlExecutionFailed:           return "Failed to execute SQL statement.";
        case SqlEmptyResults:              return "SQL statement returned nothing.";
        case SqlSchemaValidationFailed:    return "Database schema is invalid / was user-modified.";

        case UnknownFailure: return "An unknown error occurred!";
    }

    return {};
}

bool TokenDatabase::databaseConnected()
{
    return db_status;
}

TokenDatabase::Error TokenDatabase::initDatabase()
{
    if (db_status)
    {
        closeDatabase();
    }

    // create in-memory database
    try {
        db = std::make_shared<sqlite::database>(":memory:");
        db_status = true;
    } catch (sqlite::sqlite_exception &) {
        db_status = false;
        return SqlMemoryAllocationError;
    }

    return Success;
}

void TokenDatabase::closeDatabase()
{
    // force close database
    (void) sqlite3_close_v2(db->connection().get());
    db = nullptr;
    db_status = false;
    db_data.clear();
}

bool TokenDatabase::setPassword(const std::string &password)
{
    if (password.empty())
        return false;

    // remove old password
    TokenDatabase::databasePassword.clear();

    // don't use smart pointers here, already managed/deleted by crypto++ itself
    CryptoPP::SHA256 hash;
    CryptoPP::StringSource src(password, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(TokenDatabase::databasePassword))));

    return true;
}

bool TokenDatabase::setTokenDatabase(const std::string &file)
{
    if (file.empty())
    {
        return false;
    }

    databasePath = file;
    return true;
}

TokenDatabase::Error TokenDatabase::changePassword(const std::string &newPassword)
{
    Error status = Success;

    if (newPassword.empty())
    {
        return PasswordEmpty;
    }

    auto pwdStatus = setPassword(newPassword);
    if (!pwdStatus)
    {
        return PasswordHashFailure;
    }

    status = saveTokens();
    return status;
}

const OTPToken TokenDatabase::selectToken(const OTPToken::sqliteTokenID &id)
{
    auto statement = sqlite3_mprintf("select * from %Q where id = %u limit 1;", "tokens", id);

    OTPToken token;

    // BLOB == std::vector<T> in this C++ SQL library
    std::vector<OTPToken::DigitType> digits;
    std::vector<OTPToken::PeriodType> period;
    std::vector<OTPToken::CounterType> counter;

    try {
        (*db) << statement
              >> [&](const OTPToken::sqliteLongID &, // "id" is not needed here
                     const OTPToken::TokenType &type,
                     const OTPToken::Label &label,
                     const OTPToken::Icon &icon,
                     const OTPToken::TokenSecret &secret,
                     const std::vector<OTPToken::DigitType> &digits,
                     const std::vector<OTPToken::PeriodType> &period,
                     const std::vector<OTPToken::CounterType> &counter,
                     const OTPToken::ShaAlgorithm &algorithm)
        {
            token.setType(type);
            token.setLabel(label);
            token.setIcon(icon);
            token.setSecret(unmangleTokenSecret(secret));
            token.setDigitLength(digits.empty() ? 0U : digits.at(0));
            token.setPeriod(period.empty() ? 0U : period.at(0));
            token.setCounter(counter.empty() ? 0U : counter.at(0));
            token.setAlgorithm(algorithm);
        };
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return {};
    }

    sqlite3_free(statement);
    return token;
}

/** FIXME: make use of this to have this straight inside the SQL statement
 *         and avoid sorting the list manually using the config.order list
 *

ORDER BY
  CASE ID
    WHEN 4 THEN 0
    WHEN 3 THEN 1
    WHEN 1 THEN 2
    WHEN 5 THEN 3
    WHEN 6 THEN 4
  END

*/

const TokenDatabase::OTPTokenList TokenDatabase::selectTokens(const OTPToken::sqliteTypesID &type)
{
    char *statement = nullptr;

    if (type == OTPToken::None)
    {
        statement = sqlite3_mprintf("select id from %Q order by id asc;", "tokens");
    }
    else
    {
        statement = sqlite3_mprintf("select id from %Q where type = %u order by id asc;", "tokens", type);
    }

    std::vector<OTPToken::sqliteLongID> ids;

    // fetch all ids
    try {
        (*db) << statement
              >> [&](const OTPToken::sqliteLongID &id)
        {
            ids.emplace_back(id);
        };
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return {};
    }

    sqlite3_free(statement);

    // fetch all tokens
    OTPTokenList tokens;

    for (auto&& id : ids)
    {
        tokens.emplace_back(selectToken(id));
    }

    return tokens;
}

const TokenDatabase::OTPTokenList TokenDatabase::selectTokens(const OTPToken::Label &label_like)
{
    auto statement = sqlite3_mprintf("select * from %Q where label like %Q order by id asc;", "tokens", label_like.c_str());

    OTPTokenList tokens;

    // BLOB == std::vector<T> in this C++ SQL library
    std::vector<OTPToken::DigitType> digits;
    std::vector<OTPToken::PeriodType> period;
    std::vector<OTPToken::CounterType> counter;

    try {
        (*db) << statement
              >> [&](const OTPToken::sqliteLongID &, // "id" is not needed here
                     const OTPToken::TokenType &type,
                     const OTPToken::Label &label,
                     const OTPToken::Icon &icon,
                     const OTPToken::TokenSecret &secret,
                     const std::vector<OTPToken::DigitType> &digits,
                     const std::vector<OTPToken::PeriodType> &period,
                     const std::vector<OTPToken::CounterType> &counter,
                     const OTPToken::ShaAlgorithm &algorithm)
        {
            OTPToken token;
            token.setType(type);
            token.setLabel(label);
            token.setIcon(icon);
            token.setSecret(unmangleTokenSecret(secret));
            token.setDigitLength(digits.empty() ? 0U : digits.at(0));
            token.setPeriod(period.empty() ? 0U : period.at(0));
            token.setCounter(counter.empty() ? 0U : counter.at(0));
            token.setAlgorithm(algorithm);
            tokens.emplace_back(token);
        };
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return {};
    }

    sqlite3_free(statement);
    return tokens;
}

TokenDatabase::Error TokenDatabase::insertToken(const OTPToken &token)
{
    auto statement = sqlite3_mprintf("insert into %Q (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q) values (?, ?, ?, ?, ?, ?, ?, ?);",
                                     "tokens",
                                     "type", "label", "icon", "secret", "digits", "period", "counter", "algorithm");

    // BLOB == std::vector<T> in this C++ SQL library
    try {
        (*db) << statement
              << token.type()
              << token.label()
              << token.icon() // already a std::vector<>
              << mangleTokenSecret(token.secret())
              << std::vector<OTPToken::DigitType>{token.digitLength()}
              << std::vector<OTPToken::PeriodType>{token.period()}
              << std::vector<OTPToken::CounterType>{token.counter()}
              << token.algorithm();
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return SqlExecutionFailed;
    }

    sqlite3_free(statement);
    return Success;
}

OTPToken::sqliteTokenID TokenDatabase::tokenCount(const OTPToken::sqliteTypesID &type)
{
    char *statement = nullptr;

    if (type == OTPToken::None)
    {
        statement = sqlite3_mprintf("select count(*) from %Q;", "tokens");
    }
    else
    {
        statement = sqlite3_mprintf("select count(*) from %Q where type = %u;", "tokens", type);
    }

    OTPToken::sqliteTokenID count = 0;

    try {
        (*db) << statement >> count;
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return -1;
    }

    sqlite3_free(statement);
    return count;
}

const std::string TokenDatabase::selectTokenTypeName(const OTPToken::sqliteTypesID &id)
{
    return selectStaticValue("types", id);
}

const std::string TokenDatabase::selectAlgorithmName(const OTPToken::sqliteAlgorithmsID &id)
{
    return selectStaticValue("algorithms", id);
}

TokenDatabase::Error TokenDatabase::bootstrapDatabase()
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // create tables to store types and algorithms
    for (auto&& table : {"types", "algorithms"})
    {
        auto res = createTable(table, {
            {"id",   "int(1) PRIMARY KEY NOT NULL"},
            {"name", "text UNIQUE"},
        });
        if (res != Success)
        {
            return res;
        }
    }

    // insert static types
    auto res = insertStaticValues("types", {
        {OTPToken::TOTP,  "TOTP"},
        {OTPToken::HOTP,  "HOTP"},
        {OTPToken::Steam, "Steam"},
    });
    if (res != Success)
    {
        return res;
    }

    // insert static algorithms
    res = insertStaticValues("algorithms", {
        {OTPToken::SHA1,   "SHA1"},
        {OTPToken::SHA256, "SHA256"},
        {OTPToken::SHA512, "SHA512"},
    });
    if (res != Success)
    {
        return res;
    }

    // create config table
    res = createTable("config", {
        {"id",   "text PRIMARY KEY NOT NULL"},
        {"data", "blob"},
    });
    if (res != Success)
    {
        return res;
    }

    // store database version into config
    res = storeDatabaseVersion();
    if (res != Success)
    {
        return res;
    }

    // create record to store the sort order (display order)
    res = storeDisplayOrder({});
    if (res != Success)
    {
        return res;
    }

    // create table to store the tokens
    res = createTable("tokens", {
        {"id",        "INTEGER PRIMARY KEY NOT NULL"},
        {"type",      "int(1) NOT NULL"},
        {"label",     "text NOT NULL UNIQUE COLLATE NOCASE"},
        {"icon",      "blob"},
        {"secret",    "text NOT NULL"},
        {"digits",    "blob"},
        {"period",    "blob"},
        {"counter",   "blob"},
        {"algorithm", "int(1) NOT NULL"},
    },
        "FOREIGN KEY(type) REFERENCES types(id), "
        "FOREIGN KEY(algorithm) REFERENCES algorithms(id)");
    if (res != Success)
    {
        return res;
    }

    return Success;
}

TokenDatabase::Error TokenDatabase::createTable(const std::string &table_name, const std::vector<SchemaField> &schema, const std::string &additional)
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // sanitize table name
    auto table = sqlite3_mprintf("create table %Q ", table_name.c_str());
    std::string query = std::string(table);
    sqlite3_free(table);

    // prepare schema
    query += "(";
    for (auto&& s : schema)
    {
        auto field = sqlite3_mprintf("%Q %s, ", s.name.c_str(), s.datatype.c_str());
        query += std::string(field);
        sqlite3_free(field);
    }
    query.erase(query.find_last_of(','));

    // add additional content to query
    if (!additional.empty())
    {
        query += ", " + additional;
    }

    // finalize query
    query += ");";

    // execute query
    try {
        (*db) << query;
    } catch (sqlite::sqlite_exception &) {
        return SqlSystemTableCreationError;
    }

    return Success;
}

TokenDatabase::Error TokenDatabase::insertStaticValues(const std::string &table_name, const std::vector<StaticValueSet> &values)
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // sanitize table name
    auto table = sqlite3_mprintf("insert into %Q ", table_name.c_str());
    std::string query = std::string(table);
    sqlite3_free(table);

    // prepare values
    query += "values ";
    for (auto&& v : values)
    {
        auto field = sqlite3_mprintf("(%u, %Q), ", v.id, v.value.c_str());
        query += std::string(field);
        sqlite3_free(field);
    }
    query.erase(query.find_last_of(','));

    // finalize query
    query += ";";

    // execute query
    try {
        (*db) << query;
    } catch (sqlite::sqlite_exception &) {
        return SqlStaticRecordCreationError;
    }

    return Success;
}

const std::string TokenDatabase::selectStaticValue(const std::string &table, const OTPToken::sqliteShortID &id)
{
    if (!db_status)
    {
        return {};
    }

    auto statement = sqlite3_mprintf("select name from %Q where id = %i limit 1;", table.c_str(), id);

    std::string name;

    try {
        (*db) << statement >> name;
    } catch (sqlite::sqlite_exception &) {
    }

    sqlite3_free(statement);
    return name;
}

TokenDatabase::Error TokenDatabase::storeDatabaseVersion()
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // prepare statement
    auto statement = sqlite3_mprintf("insert into %Q values (?, ?);",
                                     "config");

    try {
        (*db) << statement << "database" << std::vector<std::uint32_t>{DATABASE_VERSION};
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return SqlExecutionFailed;
    }

    sqlite3_free(statement);
    return Success;
}

TokenDatabase::Error TokenDatabase::getDatabaseVersion(std::uint32_t &version)
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // prepare statement
    auto statement = sqlite3_mprintf("select %s from %Q where %s = %Q limit 1;",
                                     "data", "config", "id", "database");

    std::vector<std::uint32_t> data;

    try {
        (*db) << statement >> data;
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return SqlExecutionFailed;
    }

    sqlite3_free(statement);

    if (data.empty())
    {
        // assume user manipulation when this field is empty or
        // doesn't contain a serialized std::vector<>
        return SqlSchemaValidationFailed;
    }

    // the database version is the first element
    version = data.at(0);

    return Success;
}

TokenDatabase::Error TokenDatabase::storeDisplayOrder(const std::vector<OTPToken::sqliteSortOrder> &order)
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // prepare statement
    auto statement = sqlite3_mprintf("insert into %Q values (?, ?);",
                                     "config");

    try {
        (*db) << statement << "order" << order;
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return SqlExecutionFailed;
    }

    sqlite3_free(statement);
    return Success;
}

TokenDatabase::Error TokenDatabase::getDisplayOrder(std::vector<OTPToken::sqliteSortOrder> &order)
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // prepare statement
    auto statement = sqlite3_mprintf("select %s from %Q where %s = %Q limit 1;",
                                     "data", "config", "id", "order");

    try {
        (*db) << statement >> order;
    } catch (sqlite::sqlite_exception &) {
        sqlite3_free(statement);
        return SqlExecutionFailed;
    }

    sqlite3_free(statement);
    return Success;
}

bool TokenDatabase::serializeDatabase(std::string &out)
{
    // database must be open
    sqlite3_int64 size = 0;
    auto data = sqlite3_serialize(db->connection().get(), "main", &size, 0);

    if (!data)
    {
        out.clear();
        return false;
    }

    out = std::string(reinterpret_cast<const char*>(data), size);
    sqlite3_free(data);
    return true;
}

bool TokenDatabase::deserializeDatabase(const std::string &data)
{
    if (data.empty())
    {
        return false;
    }

    // copy stream, sqlite uses this
    // clearing it or changing its content will cause failure later
    db_data = data;

    // empty database must be open
    auto rc = sqlite3_deserialize(db->connection().get(), "main",
                                  const_cast<unsigned char*>(
                                      reinterpret_cast<const unsigned char*>(db_data.data())
                                  ), db_data.size(), db_data.size(), 0);
    if (rc)
    {
        return false;
    }

    return true;
}

TokenDatabase::Error TokenDatabase::validateSchema()
{
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    const auto pragma = "pragma table_info(%Q)";

#define SQLITE_PRAGMA_ARGLIST \
    const sqlite3_int64 &/*cid*/, \
    const std::string &name, \
    const std::string &type, \
    const bool &notnull, \
    const std::string &dflt_value, \
    const bool &pk

    const auto verifyStatics = [&](const std::string &table) {
        auto statement = sqlite3_mprintf(pragma, table.c_str());

        bool validId = false, validName = false;

        try {
            (*db) << statement >> [&](SQLITE_PRAGMA_ARGLIST)
            {
                if (name == "id")
                {
                    validId = (type == "int(1)" && notnull && dflt_value.empty() && pk);
                }
                else if (name == "name")
                {
                    validName = (type == "text" && !notnull && dflt_value.empty() && !pk);
                }
            };
        } catch (sqlite::sqlite_exception &) {
            sqlite3_free(statement);
            return false;
        }

        sqlite3_free(statement);
        return validId && validName;
    };

    const auto verifyConfig = [&] {
        auto statement = sqlite3_mprintf(pragma, "config");

        bool validId = false, validData = false;

        try {
            (*db) << statement >> [&](SQLITE_PRAGMA_ARGLIST)
            {
                if (name == "id")
                {
                    validId = (type == "text" && notnull && dflt_value.empty() && pk);
                }
                else if (name == "data")
                {
                    validData = (type == "blob" && !notnull && dflt_value.empty() && !pk);
                }
            };
        } catch (sqlite::sqlite_exception &) {
            sqlite3_free(statement);
            return false;
        }

        sqlite3_free(statement);
        return validId && validData;
    };

    const auto verifyTokens = [&] {
        auto statement = sqlite3_mprintf(pragma, "tokens");

        bool validId = false,
             validType = false,
             validLabel = false,
             validIcon = false,
             validSecret = false,
             validDigits = false,
             validPeriod = false,
             validCounter = false,
             validAlgorithm = false;

        try {
            (*db) << statement >> [&](SQLITE_PRAGMA_ARGLIST)
            {
                if (name == "id")
                {
                    validId = (type == "INTEGER" && notnull && dflt_value.empty() && pk);
                }
                else if (name == "type")
                {
                    validType = (type == "int(1)" && notnull && dflt_value.empty() && !pk);
                }
                else if (name == "label")
                {
                    validLabel = (type == "text" && notnull && dflt_value.empty() && !pk);
                }
                else if (name == "icon")
                {
                    validIcon = (type == "blob" && !notnull && dflt_value.empty() && !pk);
                }
                else if (name == "secret")
                {
                    validSecret = (type == "text" && notnull && dflt_value.empty() && !pk);
                }
                else if (name == "digits")
                {
                    validDigits = (type == "blob" && !notnull && dflt_value.empty() && !pk);
                }
                else if (name == "period")
                {
                    validPeriod = (type == "blob" && !notnull && dflt_value.empty() && !pk);
                }
                else if (name == "counter")
                {
                    validCounter = (type == "blob" && !notnull && dflt_value.empty() && !pk);
                }
                else if (name == "algorithm")
                {
                    validAlgorithm = (type == "int(1)" && notnull && dflt_value.empty() && !pk);
                }
            };
        } catch (sqlite::sqlite_exception &) {
            sqlite3_free(statement);
            return false;
        }

        sqlite3_free(statement);
        return validId &&
               validType &&
               validLabel &&
               validIcon &&
               validSecret &&
               validDigits &&
               validPeriod &&
               validCounter &&
               validAlgorithm;
    };

    auto ret = verifyStatics("types");
    if (!ret) return SqlSchemaValidationFailed;

    ret = verifyStatics("algorithms");
    if (!ret) return SqlSchemaValidationFailed;

    ret = verifyConfig();
    if (!ret) return SqlSchemaValidationFailed;

    ret = verifyTokens();
    if (!ret) return SqlSchemaValidationFailed;

    return Success;
}

TokenDatabase::Error TokenDatabase::initializeTokens()
{
    // allocate a new sqlite database in-memory
    auto status = initDatabase();
    if (status != Success)
    {
        return status;
    }

    // bootstrap a new database from the built-in schema
    status = bootstrapDatabase();
    if (status != Success)
    {
        return status;
    }

    // write to disk
    status = saveTokens();
    if (status != Success)
    {
        return status;
    }

    return Success;
}

TokenDatabase::Error TokenDatabase::saveTokens()
{
    // check if the database is open
    if (!db_status)
    {
        return SqlDatabaseNotOpen;
    }

    // serialize the sqlite database
    std::string sqlitedb;
    auto ret = serializeDatabase(sqlitedb);
    if (!ret)
    {
        return SqlSerializationError;
    }

    // encrypt the stream
    std::string encrypted;
    auto status = encrypt(databasePassword, sqlitedb, encrypted);
    sqlitedb.clear();
    if (status != Success)
    {
        return status;
    }

    // write the encrypted stream to a file
    status = writeFile(databasePath, encrypted);
    encrypted.clear();
    return status;
}

TokenDatabase::Error TokenDatabase::loadTokens()
{
    // read the encrypted file
    std::string in;
    auto status = readFile(databasePath, in);
    if (status != Success)
    {
        return status;
    }

    // decrypt the stream
    std::string decrypted;
    status = decrypt(databasePassword, in, decrypted);
    in.clear();
    if (status != Success)
    {
        return status;
    }

    // allocate memory for a database, if not yet initialized
    if (!db_status)
    {
        status = initDatabase();
        if (status != Success)
        {
            return status;
        }
    }

    // deserialize the sqlite database
    auto ret = deserializeDatabase(decrypted);
    decrypted.clear();
    if (!ret)
    {
        return SqlDeserializationError;
    }

    // perform a query in this function to avoid failure later
    // FIXME: still couldn't figure out why this happens, but
    // it works after the first execution in the same function
    std::uint32_t version = 0;
    (void) getDatabaseVersion(version);

    // validate the schema of the database
    status = validateSchema();
    if (status != Success)
    {
        return status;
    }

    return Success;
}

const OTPToken::TokenSecret TokenDatabase::mangleTokenSecret(const OTPToken::TokenSecret &secret)
{
    auto mangled = secret;
    std::reverse(mangled.begin(), mangled.end());
    return mangled;
}

const OTPToken::TokenSecret TokenDatabase::unmangleTokenSecret(const OTPToken::TokenSecret &secret)
{
    // currently the token secret is only reversed
    return mangleTokenSecret(secret);
}

TokenDatabase::Error TokenDatabase::encrypt(const std::string &password,
                                            const std::string &input_buffer, std::string &out, const int64_t &size)
{
    out.clear();

    try {
        CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH + CryptoPP::AES::BLOCKSIZE);
        CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
        hkdf.DeriveKey(key, key.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(), nullptr, 0);

        std::string ciphertext;

        CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
        auto input_buffer_size = (size == -1 ? input_buffer.size() : static_cast<std::size_t>(size));
        stfEncryptor.Put(reinterpret_cast<const unsigned char*>(input_buffer.data()), input_buffer_size);
        stfEncryptor.MessageEnd();

        for (auto&& c : ciphertext)
        {
            out.push_back(c);
        }

        return Success;
    } catch (CryptoPP::InvalidCiphertext e) {
        // e.what();
        return InvalidCiphertext;
    } catch (...) {
        return EncryptionFailure;
    }
}

TokenDatabase::Error TokenDatabase::encryptFromFile(const std::string &password,
                                                    const std::string &file, std::string &out)
{
    out.clear();

    auto status = readFile(file, out);
    if (status != Success)
    {
        out.clear();
        return status;
    }

    auto in = out;

    return encrypt(password, in, out);
}

TokenDatabase::Error TokenDatabase::decrypt(const std::string &password,
                                            const std::string &input_buffer, std::string &out, const int64_t &size)
{
    out.clear();

    try {
        CryptoPP::SecByteBlock key(CryptoPP::AES::MAX_KEYLENGTH + CryptoPP::AES::BLOCKSIZE);
        CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
        hkdf.DeriveKey(key, key.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                       reinterpret_cast<const unsigned char*>(password.data()), password.size(), nullptr, 0);

        std::string decryptedtext;

        CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
        auto input_buffer_size = (size == -1 ? input_buffer.size() : static_cast<std::size_t>(size));
        stfDecryptor.Put(reinterpret_cast<const unsigned char*>(input_buffer.data()), input_buffer_size);
        stfDecryptor.MessageEnd();

        for (auto&& c : decryptedtext)
        {
            out.push_back(c);
        }

        return Success;
    } catch (CryptoPP::InvalidCiphertext e) {
        // e.what();
        return InvalidCiphertext;
    } catch (...) {
        return DecryptionFailure;
    }
}

TokenDatabase::Error TokenDatabase::decryptFromFile(const std::string &password,
                                                    const std::string &file, std::string &out)
{
    out.clear();

    std::string buffer;
    auto status = readFile(file, buffer);
    if (status != Success)
    {
        return status;
    }

    return decrypt(password, buffer, out);
}

TokenDatabase::Error TokenDatabase::readFile(const std::string &file, std::string &out)
{
    std::string buffer;

    try {
        std::ifstream stream(file, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
        std::streamsize stream_size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        // reserve memory for file
        buffer.resize(stream_size);

        // read file into buffer
        if (!stream.read(buffer.data(), stream_size))
        {
            stream.close();
            buffer.clear();
            return FileReadFailure;
        }

        stream.close();
    } catch (...) {
        return FileReadFailure;
    }

    if (buffer.empty())
    {
        return FileEmpty;
    }

    out = buffer;

    return Success;
}

TokenDatabase::Error TokenDatabase::writeFile(const std::string &location, const std::string &buffer)
{
    try {
        std::ofstream stream(location, std::ios_base::out | std::ios_base::binary);
        if (!stream.write(buffer.data(), buffer.size()))
        {
            stream.close();
            return FileWriteFailure;
        }

        stream.close();
    } catch (...) {
        return FileWriteFailure;
    }

    return Success;
}
