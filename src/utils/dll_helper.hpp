#pragma once

#include <string>
#include <dlfcn.h>

typedef void*   DLLHandler;
typedef void*   ProcessHandler;

namespace naiveTrader
{

class DLLHelper{
public:
    static DLLHandler loadLibrary(const char *filename) {
        try{
            DLLHandler ret = dlopen(filename, RTLD_NOW);
            if (ret == NULL) {
                printf("%s\n", dlerror());
            }
            return ret;
        } catch(...) {
            return NULL;
        }
    }

    static void freeLibrary(DLLHandler handler) {
        if (handler == NULL) {
            return;
        }
        dlclose(handler);
    }

    static ProcessHandler getSymbol(DLLHandler handler, const char *name) {
        if (handler == NULL) {
            return;
        }
        return dlsym(handler, name);
    }

    static std::string getDllName(const char *name, const char *unixPreix = "lib") {
        size_t idx = 0;
        while(std::isalpha(name[idx])) {
            idx++;
        }
        std::string dllName(name, idx);
        dllName.append(unixPreix);
        dllName.append(name + idx);
        dllName.append(".so");
        return std::move(dllName);
    }

};


} // namespace naiveTrader
