#include "managedvar.h"
#include "database.h"

namespace llpp::core::data
{
	template <typename T>
	ManagedVar<T>::ManagedVar(const std::string& t_key) : key(t_key)
	{
		load();
	}

	template <typename T>
	ManagedVar<T>::ManagedVar(
		const std::string& t_key, T t_default_if_not_found)
		: key(t_key), default_if_not_found(t_default_if_not_found)
	{
		load();
	};

	template <typename T> ManagedVar<T>::~ManagedVar() { save(); }

	template <typename T> T ManagedVar<T>::get()
	{
		load();
		return value;
	}

	template <typename T> void ManagedVar<T>::set(const T& value)
	{
		this->value = value;
		save();
	}

	template <typename T> void ManagedVar<T>::load()
	{
		was_found_last_query = false;
		auto select = std::format(
			"SELECT value FROM config WHERE key = '{}';", key);
		int rc = sqlite3_exec(db, select.c_str(), load_callback, this, nullptr);

		if (!was_found_last_query) {
			std::cout << "[!] Inserting default for: " << key << std::endl;
			set(default_if_not_found);
		}
	}

	template <typename T> void ManagedVar<T>::save()
	{
		std::ostringstream update;
		update << std::format("INSERT OR REPLACE INTO config (key, "
							  "value) VALUES ('{}', '{}');",
			key, value);

		int rc = sqlite3_exec(
			db, update.str().c_str(), nullptr, nullptr, nullptr);

		if (rc != SQLITE_OK) {
			std::cerr << "Failed to execute SQL statement. Error: "
					  << sqlite3_errmsg(db) << std::endl;
		}
	}

	void ManagedVar<bool>::save()
	{
		std::ostringstream update;
		update << std::format("INSERT OR REPLACE INTO config (key, "
							  "value) VALUES ('{}', '{}');",
			key, int(value));

		int rc = sqlite3_exec(
			db, update.str().c_str(), nullptr, nullptr, nullptr);

		if (rc != SQLITE_OK) {
			std::cerr << "Failed to execute SQL statement. Error: "
					  << sqlite3_errmsg(db) << std::endl;
		}
	}

	template <typename T>
	int ManagedVar<T>::load_callback(void* data, int argc, char** argv, char**)
	{
		ManagedVar* var = static_cast<ManagedVar*>(data);
		if (argc > 0) {
			std::istringstream iss(argv[0]);
			iss >> var->value;
			var->was_found_last_query = true;
		}
		return 0;
	}

	int ManagedVar<std::string>::load_callback(
		void* data, int argc, char** argv, char**)
	{
		ManagedVar<std::string>* var = static_cast<ManagedVar<std::string>*>(
			data);
		if (argc > 0) {
			const char* textValue = reinterpret_cast<const char*>(argv[0]);
			if (textValue) {
				var->value = textValue;
				var->was_found_last_query = true;
			}
		}
		return 0;
	}

	int ManagedVar<bool>::load_callback(
		void* data, int argc, char** argv, char**)
	{
		ManagedVar<bool>* var = static_cast<ManagedVar<bool>*>(data);
		if (argc > 0) {
			int intValue;
			std::istringstream(argv[0]) >> intValue;
			var->value = (intValue != 0);
			var->was_found_last_query = true;
		}
		return 0;
	}




	template struct ManagedVar<bool>;
	template struct ManagedVar<int>;
	template struct ManagedVar<float>;
	template struct ManagedVar<std::string>;


}