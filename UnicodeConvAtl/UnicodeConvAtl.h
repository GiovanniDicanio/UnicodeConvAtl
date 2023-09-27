#pragma once

////////////////////////////////////////////////////////////////////////////////
// Unicode UTF-16/UTF-8 conversion functions for ATL (and MFC) CStringA/W
//
//                  Copyright (C) by Giovanni Dicanio
//                    <giovanni.dicanio AT gmail.com>
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// This is a header-only C++ file that implements a couple of functions
// to simply and conveniently convert Unicode text between UTF-16 and UTF-8.
//
// CStringW is used to store UTF-16-encoded text.
// CStringA is used to store UTF-8-encoded text.
//
// The exported functions are:
//
//      * Convert from UTF-16 to UTF-8:
//        CStringA ToUtf8(CStringW const& utf16)
//
//      * Convert from UTF-8 to UTF-16:
//        CStringW ToUtf16(CStringA const& utf8)
//
// These functions live under the UnicodeConvAtl namespace.
//
// This code compiles cleanly at warning level 4 (/W4)
// on both 32-bit and 64-bit builds.
//
//
// NOTE ON COMPILING THE CODE ON OLDER VC++ COMPILERS
// ==================================================
//
// This code has been written and compiled with Visual Studio 2019.
// If you want to back-port it to older C++ compilers (like VS 2008)
// that don't implement C++11+ features, you can simply:
//
//  - replace all the occurrences of nullptr with NULL
//  - replace all the occurrences of constexpr with static const
//
//
//------------------------------------------------------------------------------
//
// The MIT License(MIT)
//
// Copyright(c) 2010-2023 by Giovanni Dicanio
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//------------------------------------------------------------------------------


//==============================================================================
//                              Includes
//==============================================================================

#include <windows.h>    // Win32 Platform SDK

#include <atldef.h>     // ATLASSERT, AtlThrow, AtlThrowLastWin32
#include <atlstr.h>     // CStringA/W


//==============================================================================
//                          Function Implementations
//==============================================================================

namespace UnicodeConvAtl {

//------------------------------------------------------------------------------
// Convert from UTF-16 CStringW to UTF-8 CStringA.
// Signal errors using AtlThrow.
//------------------------------------------------------------------------------
inline CStringA ToUtf8(CStringW const& utf16)
{
    // Special case of empty input string
    if (utf16.IsEmpty())
    {
        // Empty input --> return empty output string
        return CStringA();
    }

    // Safely fail if an invalid UTF-16 character sequence is encountered
    constexpr DWORD kFlags = WC_ERR_INVALID_CHARS;

    const int utf16Length = utf16.GetLength();

    // Get the length, in chars, of the resulting UTF-8 string
    const int utf8Length = ::WideCharToMultiByte(
        CP_UTF8,            // convert to UTF-8
        kFlags,             // conversion flags
        utf16,              // source UTF-16 string
        utf16Length,        // length of source UTF-16 string, in wchar_ts
        nullptr,            // unused - no conversion required in this step
        0,                  // request size of destination buffer, in chars
        nullptr, nullptr    // unused
    );
    if (utf8Length == 0)
    {
        // Conversion error: capture error code and throw
        AtlThrowLastWin32();
    }

    // Make room in the destination string for the converted bits
    CStringA utf8;
    char* utf8Buffer = utf8.GetBuffer(utf8Length);
    ATLASSERT(utf8Buffer != nullptr);

    // Do the actual conversion from UTF-16 to UTF-8
    int result = ::WideCharToMultiByte(
        CP_UTF8,            // convert to UTF-8
        kFlags,             // conversion flags
        utf16,              // source UTF-16 string
        utf16Length,        // length of source UTF-16 string, in wchar_ts
        utf8Buffer,         // pointer to destination buffer
        utf8Length,         // size of destination buffer, in chars
        nullptr, nullptr    // unused
    );
    if (result == 0)
    {
        // Conversion error: capture error code and throw
        AtlThrowLastWin32();
    }

    // Don't forget to call ReleaseBuffer on the CString object!
    utf8.ReleaseBuffer(utf8Length);

    // It is good coding practice to clear the CString buffer pointer
    // that was returned by CString::GetBuffer after a matching call
    // to CString::ReleaseBuffer.
    // However, in this case we just return the result string
    // from the function, so we can skip that line:
    //
    // utf8Buffer = nullptr;

    return utf8;
}


//------------------------------------------------------------------------------
// Convert from UTF-8 CStringA to UTF-16 CStringW.
// Signal errors using AtlThrow.
//------------------------------------------------------------------------------
inline CStringW ToUtf16(CStringA const& utf8)
{
    // Special case of empty input string
    if (utf8.IsEmpty())
    {
        // Empty input --> return empty output string
        return CStringW();
    }

    // Safely fail if an invalid UTF-8 character sequence is encountered
    constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

    const int utf8Length = utf8.GetLength();

    // Get the size of the destination UTF-16 string
    const int utf16Length = ::MultiByteToWideChar(
        CP_UTF8,       // source string is in UTF-8
        kFlags,        // conversion flags
        utf8,          // source UTF-8 string pointer
        utf8Length,    // length of the source UTF-8 string, in chars
        nullptr,       // unused - no conversion done in this step
        0              // request size of destination buffer, in wchar_ts
    );
    if (utf16Length == 0)
    {
        // Conversion error: capture error code and throw
        AtlThrowLastWin32();
    }

    // Make room in the destination string for the converted bits
    CStringW utf16;
    wchar_t* utf16Buffer = utf16.GetBuffer(utf8Length);
    ATLASSERT(utf16Buffer != nullptr);

    // Do the actual conversion from UTF-8 to UTF-16
    int result = ::MultiByteToWideChar(
        CP_UTF8,       // source string is in UTF-8
        kFlags,        // conversion flags
        utf8,          // source UTF-8 string pointer
        utf8Length,    // length of source UTF-8 string, in chars
        utf16Buffer,   // pointer to destination buffer
        utf16Length    // size of destination buffer, in wchar_ts
    );
    if (result == 0)
    {
        // Conversion error: capture error code and throw
        AtlThrowLastWin32();
    }

    // Don't forget to call ReleaseBuffer on the CString object!
    utf16.ReleaseBuffer(utf16Length);

    // It is good coding practice to clear the CString buffer pointer
    // that was returned by CString::GetBuffer after a matching call
    // to CString::ReleaseBuffer.
    // However, in this case we just return the result string
    // from the function, so we can skip that line:
    //
    // utf16Buffer = nullptr;

    return utf16;
}

} // namespace UnicodeConvAtl

