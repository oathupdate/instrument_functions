#ifndef _INCULDE_CYG_PROFILE_HPP
#define _INCULDE_CYG_PROFILE_HPP

#include <dlfcn.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cxxabi.h>
#include <unordered_map>
#include <string>
#include <chrono>
#include <ctime>

#ifndef NO_INSTRUMENT
#define NO_INSTRUMENT __attribute__((no_instrument_function))
#endif



class Time {
 public:
    static int64_t NowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

class CygProfile {
 public:
    static CygProfile &instance() {
        static CygProfile p = CygProfile();
        return p;
    }
    std::string &get(void *addr) {
        static std::string kEmpty = "";
        auto iter = addr2funcname_.find(addr);
        if (iter != addr2funcname_.end()) {
            return iter->second;
        }
        return kEmpty;
    }

    CygProfile() {
        const char* env = std::getenv("CYG_PROFILE_FUNC_FILTER");
        if (env) {
            func_filter_ = std::string(env);
        }
        env = std::getenv("CYG_PROFILE_ENABLE");
        if (env) {
            enable_ = std::atoi(env);
        }
    }
    std::unordered_map<std::string, uint64_t> func_dur_;
    std::unordered_map<void*, std::string> addr2funcname_;
    std::string func_filter_;
    bool enable_ = false;
};

NO_INSTRUMENT static std::string &GetTraceFunctionName(void *callee, void *caller) {
    std::string &name = CygProfile::instance().get(callee);
    if (!name.empty()) {
        return name;
    }
    Dl_info info;
    if (!dladdr(callee, &info)) {
        CygProfile::instance().addr2funcname_[callee] = "-unknown";
        return CygProfile::instance().get(callee);
    }
    char *demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, NULL);
    if (demangled) {
        CygProfile::instance().addr2funcname_[callee] = demangled;
        free(demangled);
    } else {
        CygProfile::instance().addr2funcname_[callee] = "-unknown";
    }
    return CygProfile::instance().get(callee);
}

NO_INSTRUMENT static void __cyg_profile_func_enter(void *this_fn, void *call_site) {
    if (!CygProfile::instance().enable_) {
        return;
    }
    const std::string &name = GetTraceFunctionName(this_fn, call_site);
    if (name.find(CygProfile::instance().func_filter_) == std::string::npos) {
        return;
    }
    pid_t tid = syscall(SYS_gettid);
    CygProfile::instance().func_dur_[std::to_string(tid) + name] = Time::NowMs();
}

NO_INSTRUMENT static void __cyg_profile_func_exit(void *this_fn, void *call_site) {
    if (!CygProfile::instance().enable_) {
        return;
    }

    const std::string &name = GetTraceFunctionName(this_fn, call_site);
    if (name.find(CygProfile::instance().func_filter_) == std::string::npos) {
        return;
    }
    pid_t tid = syscall(SYS_gettid);
    auto time = CygProfile::instance().func_dur_[std::to_string(tid) + name];
    printf("%s %d %lums\n", name.c_str(), tid, Time::NowMs() - time);
}

#endif
