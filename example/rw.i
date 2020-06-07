// test include
#include <tuple>

// test namespace
namespace example;
// reader class
interface Reader {
  std::tuple<ssize_t, int> read(void * buf, size_t size);
}
/*
 * test comment
 */
interface Writer {
  std::tuple<ssize_t, int> write(const void* buf, size_t size);
}

