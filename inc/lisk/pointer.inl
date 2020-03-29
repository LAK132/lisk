/*
MIT License

Copyright (c) 2020 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

namespace lisk
{
  template<typename T>
  pointer::pointer(T *ptr)
  : _type(typeid(T))
  {
    _value.emplace<void*>(ptr);
  }

  template<typename T>
  pointer::pointer(const T *ptr)
  : _type(typeid(T))
  {
    _value.emplace<const void*>(ptr);
  }

  template<typename T>
  pointer::pointer(std::shared_ptr<T> ptr)
  : _type(typeid(T))
  {
    _value.emplace<std::shared_ptr<void>>(ptr);
  }

  template<typename T>
  pointer &pointer::operator=(T *ptr)
  {
    _value.emplace<void*>(ptr);
    _type = std::type_index(typeid(T));
    return *this;
  }

  template<typename T>
  pointer &pointer::operator=(const T *ptr)
  {
    _value.emplace<const void*>(ptr);
    _type = std::type_index(typeid(T));
    return *this;
  }

  template<typename T>
  pointer &pointer::operator=(std::shared_ptr<T> ptr)
  {
    _value.emplace<std::shared_ptr<void>>(ptr);
    _type = std::type_index(typeid(T));
    return *this;
  }

  template<typename T>
  bool pointer::is_raw_ptr() const
  {
    return
      _type == std::type_index(typeid(T)) &&
      std::holds_alternative<void*>(_value);
  }

  template<typename T>
  bool pointer::is_raw_const_ptr() const
  {
    return
      _type == std::type_index(typeid(T)) &&
      std::holds_alternative<const void*>(_value);
  }

  template<typename T>
  bool pointer::is_shared_ptr() const
  {
    return
      _type == std::type_index(typeid(T)) &&
      std::holds_alternative<std::shared_ptr<void>>(_value);
  }

  template<typename T>
  T *pointer::as_raw_ptr() const
  {
    return static_cast<T*>(std::get<void*>(_value));
  }

  template<typename T>
  const T *pointer::as_raw_const_ptr() const
  {
    return static_cast<const T*>(std::get<const void*>(_value));
  }

  template<typename T>
  std::shared_ptr<T> pointer::as_shared_ptr() const
  {
    return std::static_pointer_cast<T>(
      std::get<std::shared_ptr<void>>(_value));
  }

  template<typename T>
  string to_string(T *)
  {
    // :TODO:
    return "<POINTER>";
  }

  template<typename T>
  const string &type_name(T *)
  {
    const static string name = "<" + type_name(T{}) + "*>";
    return name;
  }

  template<typename T>
  string to_string(const T *)
  {
    // :TODO:
    return "<POINTER>";
  }

  template<typename T>
  const string &type_name(const T *)
  {
    const static string name = "<const " + type_name(T{}) + "*>";
    return name;
  }

  template<typename T>
  string to_string(const std::shared_ptr<T> &)
  {
    // :TODO:
    return "<POINTER>";
  }

  template<typename T>
  const string &type_name(const std::shared_ptr<T> &)
  {
    const static string name = "<managed " + type_name(T{}) + "*>";
    return name;
  }
}

