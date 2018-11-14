#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#if !defined(OS_WINDOWS)
#include <csignal>
#include <map>
#endif

namespace {

#if !defined(OS_WINDOWS)

// determine type of signal handler functions
#if defined(OS_BSD) || defined(OS_MACOS)
using signal_func = sig_t;
#else
using signal_func = sighandler_t;
#endif

// signal map
static std::map<int, signal_func> signals;

#endif // !OS_WINDOWS

} // namespace

#endif // SIGNALS_HPP
