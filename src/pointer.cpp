#include "lisk/pointer.hpp"
#include "lisk/expression.hpp"

namespace lisk
{
  string to_string(const pointer &)
  {
    // :TODO: actually return the value.
    return "<POINTER>";
  }

  const string &type_name(const pointer &)
  {
    const static string name = "pointer";
    return name;
  }
}

bool operator>>(const lisk::expression &arg, lisk::pointer &out)
{
  if (!arg.is_atom() || !arg.as_atom().is_pointer()) return false;

  out = arg.as_atom().as_pointer();
  return true;
}
