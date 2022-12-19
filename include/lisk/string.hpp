#ifndef LISK_STRING_HPP
#define LISK_STRING_HPP

#include <lak/string.hpp>
#include <lak/utility.hpp>

namespace lisk
{
	struct symbol : public lak::astring
	{
		symbol()               = default;
		symbol(const symbol &) = default;
		symbol(symbol &&)      = default;

		symbol(const lak::astring &value) : lak::astring(value) {}
		symbol(lak::astring &&value) : lak::astring(lak::move(value)) {}
		symbol(const char *value) : lak::astring(value) {}

		symbol &operator=(const symbol &) = default;
		symbol &operator=(symbol &&) = default;

		symbol &operator=(const lak::astring &value)
		{
			lak::astring::operator=(value);
			return *this;
		}
		symbol &operator=(lak::astring &&value)
		{
			lak::astring::operator=(lak::move(value));
			return *this;
		}
		symbol &operator=(const char *value)
		{
			lak::astring::operator=(value);
			return *this;
		}

		using lak::astring::operator[];
	};

	struct string : public lak::astring
	{
		string()               = default;
		string(const string &) = default;
		string(string &&)      = default;

		string(const lak::astring &value) : lak::astring(value) {}
		string(lak::astring &&value) : lak::astring(lak::move(value)) {}
		string(const char *value) : lak::astring(value) {}

		string &operator=(const string &) = default;
		string &operator=(string &&) = default;

		string &operator=(const lak::astring &value)
		{
			lak::astring::operator=(value);
			return *this;
		}
		string &operator=(lak::astring &&value)
		{
			lak::astring::operator=(lak::move(value));
			return *this;
		}
		string &operator=(const char *value)
		{
			lak::astring::operator=(value);
			return *this;
		}

		using lak::astring::operator[];
	};

	lisk::string to_string(const lisk::symbol &sym);
	const lisk::string &type_name(const lisk::symbol &);

	lisk::string to_string(const lisk::string &str);
	const lisk::string &type_name(const lisk::string &);

	struct expression;
}

bool operator>>(const lisk::expression &arg, lisk::symbol &out);
bool operator>>(const lisk::expression &arg, lisk::string &out);

template<>
struct std::hash<lisk::symbol> : public std::hash<lak::astring>
{
};
template<>
struct std::hash<lisk::string> : public std::hash<lak::astring>
{
};

#endif
