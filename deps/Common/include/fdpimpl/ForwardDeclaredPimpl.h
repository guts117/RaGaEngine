#ifndef  FORWARD_DECLARED_PIMPL
#define  FORWARD_DECLARED_PIMPL

#include <utility>

//Slightly modified version of https://probablydance.com/2013/10/05/type-safe-pimpl-implementation-without-overhead/

namespace detail
{
    template<size_t ExpectedSize, size_t ActualSize, size_t ExpectedAlignment, size_t ActualAlignment>
    inline void compare_size()
    {
        static_assert(ExpectedSize == ActualSize, "The size for the ForwardDeclaredPimpl is wrong");
        static_assert(ExpectedAlignment == ActualAlignment, "The alignment for the ForwardDeclaredPimpl is wrong");
    }
    template<size_t ExpectedSize, size_t ActualSize, size_t ExpectedAlignment, size_t ActualAlignment>
    struct size_comparer
    {
        inline size_comparer()
        {
            // going through one additional layer to get good error messages
            // if I put the assert down one more template layer, gcc will show the
            // sizes in the error message
            compare_size<ExpectedSize, ActualSize, ExpectedAlignment, ActualAlignment>();
        }
    };
}

struct forwarding_constructor {};

template<typename T, size_t Size, size_t Alignment = 16>
struct ForwardDeclaredPimpl
{
    ForwardDeclaredPimpl()
    {
        new (&Get()) T();
    }
    template<typename... Args>
    ForwardDeclaredPimpl(forwarding_constructor, Args &&... args)
    {
        new (&Get()) T(std::forward<Args>(args)...);
    }
    ForwardDeclaredPimpl(const ForwardDeclaredPimpl& other)
    {
        new (&Get()) T(other.Get());
    }
    ForwardDeclaredPimpl(const T& other)
    {
        new (&Get()) T(other);
    }
    ForwardDeclaredPimpl(ForwardDeclaredPimpl&& other)
    {
        new (&Get()) T(std::move(other.Get()));
    }
    ForwardDeclaredPimpl(T&& other)
    {
        new (&Get()) T(std::move(other));
    }
    ForwardDeclaredPimpl& operator=(const ForwardDeclaredPimpl& other)
    {
        Get() = other.Get();
        return *this;
    }
    ForwardDeclaredPimpl& operator=(const T& other)
    {
        Get() = other;
        return *this;
    }
    ForwardDeclaredPimpl& operator=(ForwardDeclaredPimpl&& other)
    {
        Get() = std::move(other.Get());
        return *this;
    }
    ForwardDeclaredPimpl& operator=(T&& other)
    {
        Get() = std::move(other);
        return *this;
    }
    ~ForwardDeclaredPimpl()
    {
        detail::size_comparer<Size, sizeof(T), Alignment, alignof(T)> compare_size{};
        Get().~T();
    }
    T& Get()
    {
        return reinterpret_cast<T&>(*this);
    }
    const T& Get() const
    {
        return reinterpret_cast<const T&>(*this);
    }

private:
    alignas(Alignment) unsigned char m_pImplBuff[Size];
};

#endif // ! FORWARD_DECLARED_PIMPL