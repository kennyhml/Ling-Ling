#pragma once
#include <iostream>
#include <string>

namespace llpp::core::data
{
	template <typename T> struct ManagedVar;

	template <typename T> struct ManagedVar
	{
	public:
		ManagedVar(const std::string& key);
		ManagedVar(const std::string& key, T default_if_not_exist);

		~ManagedVar();

		T get();
		void set(const T& value);

	private:
		const std::string key;
		T value;
		T default_if_not_found;
		bool was_found_last_query = false;

		void load();
		void save();
		static int load_callback(void* data, int argc, char** argv, char**);
	};
}