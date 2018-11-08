// enable serialization and deserialization of databases
#define SQLITE_ENABLE_DESERIALIZE

// case sensitive matching
// disabled by default, just for testing
// #define SQLITE_CASE_SENSITIVE_LIKE

// enable the ICU extension for prober Unicode support
// NOTE: for now this is only handled on the GUI side using Qt
//       no Unicode specific SQL operations are done
// #define SQLITE_ENABLE_ICU
// #define SQLITE_ENABLE_ICU_COLLATIONS
