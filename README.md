# UnicodeConvAtl
Convenient easy-to-use and efficient helper functions to convert between Unicode UTF-16 and UTF-8 using ATL CString

---

This project contains a header-only C++ file that implements a couple of functions
to simply, efficiently, and conveniently convert Unicode text between UTF-16 and UTF-8, using ATL `CString` classes and direct Win32 API calls.

`CStringW` is used to store **UTF-16**-encoded text.
`CStringA` is used to store **UTF-8**-encoded text.

The exported functions are:

```cpp
    // Convert from UTF-16 to UTF-8
    CStringA ToUtf8(CStringW const& utf16)
    
    // Convert from UTF-8 to UTF-16
    CStringW ToUtf16(CStringA const& utf8)
```

These functions live under the `UnicodeConvAtl` namespace.

This code compiles cleanly at warning level 4 (`/W4`)
on both 32-bit and 64-bit builds.

Just `#include "UnicodeConvAtl.h"` in your projects, and enjoy!

## Note on Compiling the Code on Older VC++ Compilers

This code has been written and compiled with Visual Studio 2019.
If you want to back-port it to older C++ compilers (like VS 2008)
that don't implement C++11+ features, you can simply:

* replace all the occurrences of `nullptr` with `NULL`
* replace all the occurrences of `constexpr` with `static const`
