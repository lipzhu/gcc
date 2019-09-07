#ifndef __MissingUEH_H
#define __MissingUEH_H

namespace coro = std::experimental::coroutines_n4830;

/* Diagose missing unhandled_exception() in the promise type.  */
struct MissingUEH {
  coro::coroutine_handle<> handle;
  MissingUEH () : handle (nullptr) {}
  MissingUEH (coro::coroutine_handle<> handle) : handle (handle) {}
  struct missing_ueh {
    coro::suspend_never initial_suspend() { return {}; }
    coro::suspend_never final_suspend() { return {}; }
    MissingUEH get_return_object() {
      return MissingUEH (coro::coroutine_handle<missing_ueh>::from_promise (*this));
    }
    void return_void () {}
  };
};

template<> struct coro::coroutine_traits<MissingUEH> {
    using promise_type = MissingUEH::missing_ueh;
};

#endif
