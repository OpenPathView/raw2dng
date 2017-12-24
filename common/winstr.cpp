#define STRICT

#include <string.h>
#include "winstr.h"
#include "buffer.h"

#ifdef UNICODE

WinString::WinString(const char* s) : buf(0), len(0), size(0)
{
   if( !s ) return;
   int ascii_len = strlen( s );

   size = 1+MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
         s, ascii_len, NULL, 0 );

   buf = new TCHAR[size];

   len = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
         s, ascii_len, buf, size );

   buf[len] = 0;
}

WinString::WinString(const char* s, int ln) : buf(0), len(0), size(0)
{
   if( !s ) return;

   int ascii_len = 0;
   for(ascii_len; ascii_len<ln; ++ascii_len)
   {
      if( s[ascii_len] == 0 )
         break;
   }
   
   size = 1+MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
         s, ascii_len, NULL, 0 );

   buf = new TCHAR[size];

   len = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
         s, ascii_len, buf, size );

   buf[len] = 0;
}

WinString WinString::operator+(const char*s) const
{
   //WinString copy(*this);
   //copy += WinString(s);
   //return copy;
   WinString temp(s);
   WinString res( len + temp.len + 1 );
   res.append( buf, len );
   res.append( temp.buf, temp.len );
   return res;
}
#endif

WinString WinString::operator+(WinString const& a) const
{
   //WinString copy(*this);
   //copy += s;
   //return copy;
   WinString res( len + a.len + 1 );
   res.append( buf, len );
   res.append( a.buf, a.len );
   return res;
}

WinString WinString::operator+(LPCTSTR s) const
{
   //WinString copy(*this);
   //copy += s;
   //return copy;

   if( !s ) return *this;
   int s_len = strlen(s);
   WinString res( len + s_len + 1 );
   res.append( buf, len );
   res.append( s, s_len );
   return res;
}

WinString::WinString(WinString const& a) : buf(0), len(0), size(0)
{
   if( !a.buf ) return;

   init(a.buf, a.len);
}

WinString::WinString(LPCTSTR s) : buf(0), len(0), size(0)
{
   if( !s ) return;

   init( s, strlen( s ));
}

WinString::WinString(LPCTSTR s, int ln) : buf(0), len(0), size(0)
{
   if( !s ) return;

   for(len; len<ln; ++len)
   {
      if( s[len] == 0 )
         break;
   }      

   size = len+1;
   buf = new TCHAR[size];
   if( len )
      memcpy( buf, s, sizeof(TCHAR)*len );
      
   buf[len] = 0;
}

WinString& WinString::operator=(WinString const& a)
{
   if( &a == this )
      return *this;

   cleanup();

   if( !a.buf )
      return *this;

   init( a.buf, a.len);
   return *this;
}

WinString& WinString::operator=(LPCTSTR s)
{
   if( !s )
   {
      cleanup();
      return *this;
   }

   LPTSTR temp = buf;

   init( s, strlen( s ));

   delete [] temp;
   return *this;
}


WinString operator + (LPCTSTR s, WinString const& a)
{
   WinString res(s);
   res += a;
   return res;
}

void WinString::append(LPCTSTR s, int ln)
{
   if( !s || !ln) return;
#if 0
   LPTSTR temp = buf;
   int temp_len = len;

   len += ln;
   size = len+1;
   buf = new TCHAR[size];

   if( temp )
      memcpy( buf, temp, sizeof(TCHAR)*temp_len );

   memcpy( buf+temp_len, s, sizeof(TCHAR)*ln );
   buf[len] = 0;

   delete [] temp;
#else

   LPTSTR temp = 0;
   int temp_len = len;

   len += ln;
   if( len+1 > size )
   {
      temp = buf;
      size = len+1;
      buf = new TCHAR[size];

      if( temp )
         memcpy( buf, temp, sizeof(TCHAR)*temp_len );
   }

   memcpy( buf+temp_len, s, sizeof(TCHAR)*ln );
   buf[len] = 0;

   if( temp )
      delete [] temp;

#endif
}

LPCTSTR uint2str( unsigned u, TNumBuf& buf )
{
   LPTSTR p = &buf.buf[ sizeof(buf.buf) / sizeof(buf.buf[0])];
   *(--p) = 0;
   do
   {
      unsigned t = u / 10;
      int d = u - t*10;
      *(--p) = TCHAR( TCHAR('0') + d );
      u = t;
   } while( u );

   return p;
}

LPCTSTR int2str( int i, TNumBuf& buf )
{
   if( i < 0 )
   {
      LPTSTR p = const_cast<LPTSTR>( uint2str( (unsigned) -i, buf ) );
      *(--p) = TCHAR('-');
      return p;
   }
   return uint2str( (unsigned)i, buf );
}
