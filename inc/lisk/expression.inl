template<typename T>
bool operator>>(const lisk::expression &arg, T *&out)
{
	if (!arg.is_atom() || !arg.as_atom().is_pointer() ||
	    !arg.as_atom().as_pointer().is_raw_ptr<T>())
		return false;

	out = arg.as_atom().as_pointer().as_raw_ptr<T>();
	return true;
}

template<typename T>
bool operator>>(const lisk::expression &arg, const T *&out)
{
	if (!arg.is_atom() || !arg.as_atom().is_pointer()) return false;

	// Technically we can trivially cast from a non-const pointer to a const
	// pointer, so we may as well check both cases.
	if (arg.as_atom().as_pointer().is_raw_const_ptr<T>())
	{
		out = arg.as_atom().as_pointer().as_raw_const_ptr<T>();
		return true;
	}
	else if (arg.as_atom().as_pointer().is_raw_ptr<T>())
	{
		out = arg.as_atom().as_pointer().as_raw_ptr<T>();
		return true;
	}
	else
		return false;
}

template<typename T>
bool operator>>(const lisk::expression &arg, std::shared_ptr<T> &out)
{
	if (!arg.is_atom() || !arg.as_atom().is_pointer() ||
	    !arg.as_atom().as_pointer().is_shared_ptr<T>())
		return false;

	out = arg.as_atom().as_pointer().as_shared_ptr<T>();
	return true;
}
