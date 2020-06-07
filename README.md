# gen-interface-v2

A tool to generate non-intrusive interface for C++.

class can implement interface without inherit interface.

## Grammar

```
import "${filename}";

#include <something>

namespace <name>;

interface <name> extends? <base1>, <base2>... {
  <return type> <method name>(<arg1 type> <arg1 name>, <arg2 type> <arg2 name>...) <specifier>;
}
```

## Usage

Require C++17.

`mkdir build && cd build && cmake .. && make`

`./gen_interface_v2 <interface file>.i <output header file>.hpp`

if `<output header file>` is empty, result will be write to stdout.

## Example

```java
interface Reader {
    ssize_t read(void* buf, size_t size);
}

interface Writer {
    ssize_t write(const void* buf, size_t size);
}

interface ReadWriter extends Reader, Writer {}
```


```C++
#include "ReadWriter.cpp"

class File {
  public:
    int fd = -1;
    ssize_t read(void *buf, size_t size) {
        return ::read(fd, buf, size);
    }
    ssize_t write(const void* buf, size_t size) {
        return ::write(fd, buf, size);
    }
};

class FixedSizeBuffer {
    char buffer[4096];
    size_t rdoff;
    size_t wroff;
  public:
    ssize_t read(void *buf, size_t size) {
      auto readable = std::min(wroff - rdoff, size);
      memcpy(buf, buffer, readable);
      rdoff += readable;
      if (rdoff == wroff) {rdoff = wroff = 0;}
      return readable;
    }
    ssize_t write(const void* buf, size_t size) {
      auto writable = std::min(4096 - wroff, size);
      memcpy(buffer, buf, writable);
      wroff += writable;
      return writable;
    }
}

void copy(Writer w, Reader r) {
  char buf[1024];
  auto c = r.read(buf, 1024);
  w.write(buf, c);
}

void readSomething(Reader r) {
  char buf[3];
  r.read(buf, 3);
}

void writeSomething(ReadWriter rw) {
  rw.write("hello, world", 12);
  readSomething(rw);
}

int main() {
  File f{fileno(stdout)};
  FixedSizeBuffer buffer;
  writeSomething(buffer);
  copy(f, buffer);
  std::vector<Writer> wl(f, buffer);
  for (auto& w : wl) {
    w.write("hello, world", 12);
  }
}
```

## Implementation

C++ Memory Model:

```C++
class A {
    void* __virtualTable; // compiler implicit added
    //... member
    void func();
};
```

We can emulate the virtual table:

```C++
struct Interface {
  VirtualTable* __virtualTable;
  A* object;
}
```

So we can call by member pointer access operator:

```C++
(object->*(__virtualTable->func))(args...);
```

Virtual table construct:

```C++
template<typename T>
struct VirtualTable {
  void(T::* func)();
  inline static VirtualTable virtualTable{&T::func};
}
```

To generalize Interface, we use a `Dummy` struct:

```C++
struct Interface {
  struct Dummy{};
  VirtualTable<Dummy>* __virtualTable;
  Dummy* object;

  template<typename T>
  Interface(T* t) : virtualTable(reinterpret_cast<VirtualTable<Dummy>*>(&VirtualTable<T>::virtualTable)),
   object(reinterpret_cast<Dummy*>(t)){}
}
```
