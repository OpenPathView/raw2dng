#ifndef __WINSTR_H
#define __WINSTR_H

// Fix to make it work in linux
#include <string.h>
typedef char TCHAR;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;

#ifdef UNICODE
 typedef LPWSTR LPTSTR;
 typedef LPWSTR LPCTSTR;
#else
 typedef LPSTR LPTSTR;
 typedef const char* LPCTSTR;
#endif

typedef unsigned long DWORD;
typedef unsigned char BYTE;


#ifndef __SWAP_H
#include "swap.h"
#endif

class WinString
{
public:
   WinString() : buf(0), len(0), size(0) {}
   WinString(WinString const&);
   WinString(int sz) : buf(0), len(0), size(sz) 
   {
      if( size ) { 
         buf = new TCHAR[size]; 
         buf[0] = 0; 
      }
   }
   WinString(LPCTSTR);
   WinString(LPCTSTR, int ln);
   WinString& operator=(LPCTSTR);
   WinString& operator=(WinString const&);
   WinString& operator+=(WinString const& a) { append(a.buf, a.len); return *this; }
   WinString& operator+=(LPCTSTR s) { if(s) append(s, strlen(s)); return *this; }
   WinString& operator<<(WinString const& a) { append(a.buf, a.len); return *this; }
   WinString& operator<<(LPCTSTR s) { if(s) append(s, strlen(s)); return *this; }
   WinString operator+(WinString const& a) const;
   WinString operator+(LPCTSTR s) const;
#ifdef UNICODE
   WinString(const char* s);
   WinString(const char* s, int len);
   WinString& operator=(const char* s) { return (*this) = WinString(s); }
   WinString& operator+=(const char*s) { return (*this) += WinString(s); }
   WinString& operator<<(const char*s) { return (*this) << WinString(s); }
   WinString operator+(const char*s) const;
#endif

   ~WinString() { delete [] buf; }


   operator LPCTSTR() const { return buf; }
   int length() const { return len; }
   LPTSTR buffer() { return buf; }

   friend WinString operator + (LPCTSTR s, WinString const& a);

   void swap(WinString& a)
   {
      ::swap(buf, a.buf);
      ::swap(len, a.len);
      ::swap(size, a.size);
   }

protected:

   LPTSTR buf;
   int len;
   int size;

   void append(LPCTSTR, int);
   void cleanup()
   {
      delete [] buf;
      buf = 0;
      size = 0;
      len = 0;
   }
   void init(LPCTSTR s, int ln)
   {
      len = ln;
      size = len+1;
      buf = new TCHAR[size];
      memcpy( buf, s, sizeof(TCHAR)*size );
   }

};

class TNumBuf
{
public:
   TCHAR buf[16];
};

LPCTSTR uint2str( unsigned, const TNumBuf& = TNumBuf() );
LPCTSTR int2str( int, const TNumBuf& = TNumBuf() );

#endif
