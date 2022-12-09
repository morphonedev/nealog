template <typename Base, typename T> inline bool isInstanceOf(const T* ptr)
{
    return dynamic_cast<const Base*>(ptr) != nullptr;
}
