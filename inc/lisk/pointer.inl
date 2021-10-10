namespace lisk
{
  template<typename T>
  pointer::pointer(T *ptr) : _type(typeid(T))
  {
    _value.emplace<void *>(ptr);
  }

  template<typename T>
  pointer::pointer(const T *ptr) : _type(typeid(T))
  {
    _value.emplace<const void *>(ptr);
  }

  template<typename T>
  pointer::pointer(std::shared_ptr<T> ptr) : _type(typeid(T))
  {
    _value.emplace<std::shared_ptr<void>>(ptr);
  }

  template<typename T>
  pointer &pointer::operator=(T *ptr)
  {
    _value.emplace<void *>(ptr);
    _type = std::type_index(typeid(T));
    return *this;
  }

  template<typename T>
  pointer &pointer::operator=(const T *ptr)
  {
    _value.emplace<const void *>(ptr);
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
    return _type == std::type_index(typeid(T)) &&
           std::holds_alternative<void *>(_value);
  }

  template<typename T>
  bool pointer::is_raw_const_ptr() const
  {
    return _type == std::type_index(typeid(T)) &&
           std::holds_alternative<const void *>(_value);
  }

  template<typename T>
  bool pointer::is_shared_ptr() const
  {
    return _type == std::type_index(typeid(T)) &&
           std::holds_alternative<std::shared_ptr<void>>(_value);
  }

  template<typename T>
  T *pointer::as_raw_ptr() const
  {
    return static_cast<T *>(std::get<void *>(_value));
  }

  template<typename T>
  const T *pointer::as_raw_const_ptr() const
  {
    return static_cast<const T *>(std::get<const void *>(_value));
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
