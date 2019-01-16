#ifndef _SINGLETON_H_
#define _SINGLETON_H_

// A Generic singleton pattern in C++11
// Based on https://www.theimpossiblecode.com/blog/c11-generic-singleton-pattern/

template <typename T, typename CONTEXT>
class Singleton
{
public:
    // Very fast inlined operator accessors
    T*          operator->() { return mpInstance; }
    const T*    operator->() const { return mpInstance; }
    T&          operator*() { return *mpInstance; }
    const T&    operator*() const { return *mpInstance; }

protected:
    Singleton()
    {
        static bool static_init = []()->bool {
            mpInstance = new T;
            return true;
        }();
    }

    // Call the Singleton constructor with an int for it to invoke your static init() method.
    Singleton(int)
    {
        static bool static_init = []()->bool {
            mpInstance = CONTEXT::init();
            return true;
        }();
    }

private:
    static T *mpInstance;
};

template <typename T, typename CONTEXT>
T *Singleton<T, CONTEXT>::mpInstance;

#endif