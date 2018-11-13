#ifndef TOKENDATABASE_HPP
#define TOKENDATABASE_HPP

#include "AppSupport.hpp"
#include "OTPToken.hpp"

#include <cstdio>
#include <string>
#include <vector>

class TokenDatabase final
{
    TokenDatabase() = delete;

    // for readFile()
    friend class AppSupport::andOTP;
    friend class AppSupport::Authy;
    friend class AppSupport::Steam;

    static std::string databasePassword;
    static std::string databasePath;

public:
    enum Error {
        Success = 0,

        FileReadFailure,     // unable to read file or file not found or directory given
        FileWriteFailure,    // unable to write file
        FileEmpty,           // file is empty
        InvalidTokenFile,    // token file is invalid

        EncryptionFailure,   // unknown encryption failure
        DecryptionFailure,   // unknown decryption failure
        InvalidCiphertext,   // wrong password or cipher, or corrupt input buffer
        PasswordEmpty,       // password is empty
        PasswordHashFailure, // failed to hash password

        SqlDatabaseNotOpen,           // database not connected during load/save
        SqlMemoryAllocationError,     // :memory: can't be allocated
        SqlSerializationError,        // unable to serialize the database
        SqlDeserializationError,      // unable to deserialize the database
        SqlSystemTableCreationError,  // can't create system table
        SqlStaticRecordCreationError, // can't insert static record
        SqlStatementPrepareFailed,    // failed to prepare sql statement
        SqlBindBinaryDataFailed,      // failed to bind binary BLOB data
        SqlExecutionFailed,           // failed to execute sql statement
        SqlDisplayOrderStoreFailed,   // failed to store the display order
        SqlDisplayOrderUpdateFailed,  // failed to update the display order
        SqlDispalyOrderGetFailed,     // failed to receive the display order
        SqlEmptyResults,              // got no values back from SELECT statement
        SqlSchemaValidationFailed,    // tables are missing or don't have the correct schema,
                                      // edge-case when the user replaces the file manually

        UnknownFailure,      // unknown or unhandled error
    };

    using OTPTokenList = std::vector<OTPToken>;
    using DisplayOrder = std::vector<OTPToken::sqliteSortOrder>;

    // translate error enum to a human readable message describing the error
    static const std::string getErrorMessage(const Error &error);

    // get database connection status
    static bool databaseConnected();

    // initialize an empty database; close opened database
    static Error initDatabase();
    static void closeDatabase();

    // initialize/save/load the token database
    static Error initializeTokens();
    static Error saveTokens();
    static Error loadTokens();

    // display order
    static const DisplayOrder displayOrder();

    // database configuration
    static bool setPassword(const std::string &password);
    static bool setTokenDatabase(const std::string &file);

    // change database password
    static Error changePassword(const std::string &newPassword);

    // sqlite SQL statement wrappers
    static const OTPToken selectToken(const OTPToken::sqliteTokenID &id);
    static const OTPTokenList selectTokens(const OTPToken::sqliteTypesID &type = OTPToken::None);
    static const OTPTokenList selectTokens(const OTPToken::Label &label_like);
    static Error insertToken(const OTPToken &token);
    static Error updateToken(const OTPToken::sqliteTokenID &id, const OTPToken &token);
    static Error deleteToken(const OTPToken::sqliteTokenID &id);
    static OTPToken::sqliteTokenID tokenCount(const OTPToken::sqliteTypesID &type = OTPToken::None);

    static const std::string selectTokenTypeName(const OTPToken::sqliteTypesID &id);
    static const std::string selectAlgorithmName(const OTPToken::sqliteAlgorithmsID &id);

private:
    struct SchemaField {
        const std::string name;
        const std::string datatype;
    };
    struct StaticValueSet {
        const OTPToken::sqliteShortID id;
        const std::string value;
    };

    // creates an empty database with all the tables required for operation
    // types, algorithms and config are also created there
    static Error bootstrapDatabase();
    static Error createTable(const std::string &table_name, const std::vector<SchemaField> &schema, const std::string &additional = {});
    static Error insertStaticValues(const std::string &table_name, const std::vector<StaticValueSet> &values);
    static const std::string selectStaticValue(const std::string &table, const OTPToken::sqliteShortID &id);

    static Error executeGenericTokenStatement(const std::string &statement, const OTPToken &token);

    static const std::string genUpdateQuery(const std::string &table, const std::vector<std::string> &fields, const std::string &condition = {});
    static const std::string genInsertQuery(const std::string &table, const std::vector<std::string> &fields);

    // database config functions
    static Error storeDatabaseVersion();
    static Error getDatabaseVersion(std::uint32_t &version);
    static Error storeDisplayOrder(const DisplayOrder &order);
    static Error updateDisplayOrder(const DisplayOrder &order);
    static Error getDisplayOrder(DisplayOrder &order);

    // serialization functions
    static bool serializeDatabase(std::string &out);
    static bool deserializeDatabase(const std::string &data);

    // validate the schema of user-loaded (encrypted file on disk) databases
    static Error validateSchema();

    // additional token obfuscation
    static const OTPToken::TokenSecret mangleTokenSecret(const OTPToken::TokenSecret &secret);
    static const OTPToken::TokenSecret unmangleTokenSecret(const OTPToken::TokenSecret &secret);

    // encryption APIs
    static Error encrypt(const std::string &password,
                         const std::string &input_buffer, std::string &out, const int64_t &size = -1);
    static Error encryptFromFile(const std::string &password,
                                 const std::string &file, std::string &out);

    // decryption APIs
    static Error decrypt(const std::string &password,
                         const std::string &input_buffer, std::string &out, const int64_t &size = -1);
    static Error decryptFromFile(const std::string &password,
                                 const std::string &file, std::string &out);

    // write I/O APIs
    static Error readFile(const std::string &file, std::string &out);
    static Error writeFile(const std::string &location, const std::string &buffer);
};

#endif // TOKENDATABASE_HPP
