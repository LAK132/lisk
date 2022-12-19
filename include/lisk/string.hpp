#ifndef LISK_STRING_HPP
#define LISK_STRING_HPP

#include <string>

namespace lisk
{
	struct expression;

	struct symbol : public std::string
	{
		symbol()               = default;
		symbol(const symbol &) = default;
		symbol(symbol &&)      = default;

		symbol(const std::string &value) : std::string(value) {}
		symbol(std::string &&value) : std::string(std::move(value)) {}
		symbol(const char *value) : std::string(value) {}

		symbol &operator=(const symbol &) = default;
		symbol &operator=(symbol &&) = default;

		symbol &operator=(const std::string &value)
		{
			std::string::operator=(value);
			return *this;
		}
		symbol &operator=(std::string &&value)
		{
			std::string::operator=(std::move(value));
			return *this;
		}
		symbol &operator=(const char *value)
		{
			std::string::operator=(value);
			return *this;
		}

		using std::string::operator[];
	};

	struct string : public std::string
	{
		string()               = default;
		string(const string &) = default;
		string(string &&)      = default;

		string(const std::string &value) : std::string(value) {}
		string(std::string &&value) : std::string(std::move(value)) {}
		string(const char *value) : std::string(value) {}

		string &operator=(const string &) = default;
		string &operator=(string &&) = default;

		string &operator=(const std::string &value)
		{
			std::string::operator=(value);
			return *this;
		}
		string &operator=(std::string &&value)
		{
			std::string::operator=(std::move(value));
			return *this;
		}
		string &operator=(const char *value)
		{
			std::string::operator=(value);
			return *this;
		}

		using std::string::operator[];
	};

	string to_string(const symbol &sym);
	const string &type_name(const symbol &);

	string to_string(const string &str);
	const string &type_name(const string &);
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out);
bool operator>>(const lisk::expression &arg, lisk::string &out);

template<>
struct std::hash<lisk::symbol> : public std::hash<std::string>
{
};
template<>
struct std::hash<lisk::string> : public std::hash<std::string>
{
};

#endif