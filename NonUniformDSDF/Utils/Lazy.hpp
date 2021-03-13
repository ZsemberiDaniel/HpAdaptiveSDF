#ifndef LAZY_H
#define LAZY_H

#include <optional>
#include <functional>
#include <stdexcept>

// Exception thrown on attempt to access an uninitialized Lazy
struct uninitialized_lazy_exception : public std::runtime_error
{
    uninitialized_lazy_exception()
        :std::runtime_error("uninitialized lazy value")
    {}
};

template<typename T>
class Lazy
{
public:
    // Default constructor
    Lazy()
        :m_initializer(DefaultInitializer)
    {
    }

    // Construct with initializer
    Lazy(std::function<T(void)> initializer)
        :m_initializer(initializer)
    {
    }

    // Copy constructor
    Lazy(const Lazy& o)
        :m_initializer(o.m_initializer),
        m_value(o.m_value)
    {
    }

    // Assign from Lazy<T>
    Lazy& operator=(const Lazy<T>& o)
    {
        m_initializer = o.m_initializer;
        m_value = o.m_value;
        return *this;
    }

    // Assign from T
    T& operator=(const T& value)
    {
        m_value = value;
        return m_value.value();
    }

    // Answer true if initialized, either implicitly via function or explicitly via assignment
    bool isInitialized() const
    {
        return m_value.has_value();
    }

    // Force initialization, if not already done, and answer with the value
    // Throws exception if not implicitly or explicitly initialized
    T& force()
    {
        if (!isInitialized())
        {
            construct();
        }
        return m_value.value();
    }

    // Implicitly force initialization and answer with value
    operator T& ()
    {
        return force();
    }

private:
    std::optional<T> m_value;
    std::function<T(void)>  m_initializer;

    // Call copy constructor for T.  Deinitialize self first, if necessary.
    void construct()
    {
        m_value = m_initializer();
    }

    // Inititializer if none specified; throw exception on attempt to access uninitialized lazy
    static T DefaultInitializer()
    {
        throw uninitialized_lazy_exception();
    }

    // Deinitialize if none specified; does nothing
    static void DefaultDeinitializer(T&)
    {
    }
};

#endif
