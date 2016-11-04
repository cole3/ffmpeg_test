#ifndef _TDLL_
#define _TDLL_

#include <dlfcn.h>
#include <iostream>

class Tlib
{
private:
    void *handle;
public:
    bool ok;

    Tlib(const char *soName1)
    {
        handle = dlopen(soName1, RTLD_LAZY);
        ok = !!handle;
    };

    ~Tlib()
    {
        if (handle) {
            dlclose(handle);
        }
    }

    void *loadFunction(const char *name)
    {
        void *func = dlsym(handle, name);
        ok &= !!func;
        return func;
    };
};

#endif
