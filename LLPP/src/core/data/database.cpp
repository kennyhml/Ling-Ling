#include "database.h"

namespace llpp::core::data
{
	bool init(const std::filesystem::path& db_path)
	{
		bool connected = sqlite3_open(db_path.string().c_str(), &db);

		if (connected) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		}
		else {
			fprintf(stderr, "Opened database successfully\n");
		}

		// Create the configurations table if it doesn't exist
		const char* create_table_sql = "CREATE TABLE IF NOT EXISTS config ("
									   "    key TEXT PRIMARY KEY NOT NULL,"
									   "    value TEXT"
									   ");";

		int rc = sqlite3_exec(db, create_table_sql, nullptr, nullptr, nullptr);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "Can't create table: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return false;
		}

		fprintf(stderr, "Table created successfully\n");

		return true;
	}

}
