#include <stdio.h>
#include <string.h>
#pragma hdrstop

/*
   Coolpix RAW to Coolpix NEF/DNG converter
   by paul69, v0.14, 2005-12-20, <e2500@narod.ru>

   RAW Data Reader
*/

#include "raw2nef.h"

int read_raw(FILE* in, color_t ccd[], TCCDParam const& ccd_pars)
{
   unsigned raw_width = ccd_pars.raw_width;
   unsigned ccd_size = raw_width * ccd_pars.raw_height;

   if( ccd_pars.flags & TCCDParam::fInterlaced )
   {
      for(int f=0; f<2; ++f)
      {
         unsigned line = f * raw_width;
         unsigned delta = 2 * raw_width;

         unsigned i, x;

         if( ccd_pars.flags & TCCDParam::fARM )
         {
            fseek( in, f ? ccd_pars.file_size/2 : 0, SEEK_SET );
            unsigned field_size = ccd_pars.raw_height/2 * ccd_pars.row_length;

            for(x=i=0; i<field_size; i += 12)
            {
               byte bitbuf[12];
               if( !fread( bitbuf, 12, 1, in) )
                  return e_unexpected_eof;

               if( x == raw_width )
               {
                  line += delta;
                  x = 0;

                  // check space for row
                  if( line + raw_width > ccd_size )
                     return e_too_large_file;
               }

               ccd[line + x] = (color_t)(((bitbuf[3]<<4)|((bitbuf[2]>>4)&15)));
               ++x;

               ccd[line + x] = (color_t)((((bitbuf[2]&15)<<8)|(bitbuf[1])));
               ++x;

               ccd[line + x] = (color_t)(((bitbuf[0]<<4)|((bitbuf[7]>>4)&15)));
               ++x;

               ccd[line + x] = (color_t)((((bitbuf[7]&15)<<8)|(bitbuf[6])));
               ++x;

               ccd[line + x] = (color_t)(((bitbuf[5]<<4)|((bitbuf[4]>>4)&15)));
               ++x;

               ccd[line + x] = (color_t)((((bitbuf[4]&15)<<8)|(bitbuf[11])));
               ++x;

               ccd[line + x] = (color_t)(((bitbuf[10]<<4)|((bitbuf[9]>>4)&15)));
               ++x;

               ccd[line + x] = (color_t)((((bitbuf[9]&15)<<8)|(bitbuf[8])));
               ++x;
            }
         }
         else
         if( ccd_pars.bits_per_sample == 12 )
         {
            unsigned offset = ccd_pars.data_offset + (f ? (ccd_pars.file_size - ccd_pars.data_offset)/2 : 0);
            fseek( in, offset, SEEK_SET );

            unsigned field_size = ccd_pars.raw_height/2 * ccd_pars.row_length;

            for(x=i=0; i<field_size; i += 3)
            {
               byte bitbuf[3];
               if( !fread( bitbuf, 3, 1, in) )
                  return e_unexpected_eof;

               if( x == raw_width )
               {
                  line += delta;
                  x = 0;

                  // check space for row
                  if( line + raw_width > ccd_size )
                     return e_too_large_file;
               }

               ccd[line + x] = (color_t)(((bitbuf[0]<<4)|((bitbuf[1]>>4)&15)));
               ++x;

               ccd[line + x] = (color_t)((((bitbuf[1]&15)<<8)|(bitbuf[2])));
               ++x;
            }
         }
         else
         if( ccd_pars.bits_per_sample == 10 )
         {
            unsigned row_offset = f ? ccd_pars.file_size/2 : 0;
            unsigned rows = ccd_pars.raw_height/2;
            unsigned row_size = ccd_pars.row_length;

            for(int y=0; y<(int)rows; ++y, row_offset += row_size, line += delta)
            {
               // check space for row
               if( line + raw_width > ccd_size )
                  return e_too_large_file;

               fseek( in, row_offset, SEEK_SET );

               for(x=i=0; i<row_size; i += 5)
               {
                  byte bitbuf[5];

                  if( !fread( bitbuf, 5, 1, in) )
                     return e_unexpected_eof;

                  // unpack 5 bytes to 4 samples, each sample multiply by 4
                  ccd[line + x] = (color_t)((((unsigned)bitbuf[0]<<2)|((bitbuf[1]>>6)&3))<<2);
                  if( ++x == raw_width ) break;

                  ccd[line + x] = (color_t)((((bitbuf[1]&63)<<4)|((bitbuf[2]>>4)&15))<<2);
                  if( ++x == raw_width ) break;

                  ccd[line + x] = (color_t)((((bitbuf[2]&15)<<6)|((bitbuf[3]>>2)&63))<<2);
                  if( ++x == raw_width ) break;

                  ccd[line + x] = (color_t)((((bitbuf[3]&3)<<8)|(bitbuf[4]))<<2);
                  if( ++x == raw_width ) break;
               }
            }
         }
         else
            return e_not_supported;
      }
      return e_success;
   }
   else
   {
      if( ccd_pars.bits_per_sample == 16 )
      {
         unsigned line = 0;
         unsigned delta = raw_width;

         unsigned row_offset = ccd_pars.data_offset;

         unsigned row_size = ccd_pars.row_length;
         unsigned rows = ccd_pars.raw_height;

         for(unsigned y=0; y<rows; ++y, row_offset += row_size, line += delta)
         {
            // check space for row
            if( line + raw_width > ccd_size )
               return e_too_large_file;

            fseek( in, row_offset, SEEK_SET );

            unsigned i, x;
            for(x=i=0; i<row_size; i += 2)
            {
               byte bitbuf[2];
               if( !fread( bitbuf, 2, 1, in ) )
                  return e_unexpected_eof;

               unsigned sample;
               if( ccd_pars.flags & TCCDParam::fIntelByteOrder )
                  sample = (bitbuf[1]<<8)|bitbuf[0];
               else
                  sample = (bitbuf[0]<<8)|bitbuf[1];

               ccd[line + x] = (color_t)(sample >> ccd_pars.data_shift);

               if( ++x == raw_width ) break;
            }
         }
         return e_success;
      }

      if( ccd_pars.bits_per_sample == 12 )
      {
         if( ccd_pars.flags & TCCDParam::f10PixPer128bits )
         {
            unsigned line = 0;
            unsigned delta = raw_width;

            unsigned row_offset = ccd_pars.data_offset;

            unsigned row_size = ccd_pars.row_length;
            unsigned rows = ccd_pars.raw_height;

            for(unsigned y=0; y<rows; ++y, row_offset += row_size, line += delta)
            {
               // check space for row
               if( line + raw_width > ccd_size )
                  return e_too_large_file;

               fseek( in, row_offset, SEEK_SET );

               unsigned i, k, x;
               for(x=i=0; i<row_size; i += 10)
               {
                  byte bitbuf[16];
                  if( !fread( bitbuf, 16, 1, in) )
                     return e_unexpected_eof;

                  byte const* p = bitbuf;
                  for(k=0; k<5; ++k, p += 3)
                  {
                     ccd[line + x] = (color_t)(((p[1]&15) << 8) | (p[0]));
                     if( ++x == raw_width ) break;

                     ccd[line + x] = (color_t)(((p[2]) << 4) | (p[1] >> 4));
                     if( ++x == raw_width ) break;
                  }
                  if( k < 5 ) break;
               }
            }
         }
         else
         {
            unsigned line = 0;
            unsigned delta = raw_width;

            unsigned row_offset = ccd_pars.data_offset;

            unsigned row_size = ccd_pars.row_length;
            unsigned rows = ccd_pars.raw_height;

            for(unsigned y=0; y<rows; ++y, row_offset += row_size, line += delta)
            {
               // check space for row
               if( line + raw_width > ccd_size )
                  return e_too_large_file;

               fseek( in, row_offset, SEEK_SET );

               unsigned i, x;
               for(x=i=0; i<row_size; i += 3)
               {
                  byte bitbuf[3];
                  if( !fread( bitbuf, 3, 1, in) )
                     return e_unexpected_eof;

                  ccd[line + x] = (color_t)(((bitbuf[0]<<4)|((bitbuf[1]>>4)&15)));
                  if( ++x == raw_width ) break;

                  ccd[line + x] = (color_t)((((bitbuf[1]&15)<<8)|(bitbuf[2])));
                  if( ++x == raw_width ) break;
               }
            }
         }
         return e_success;
      }
   }
   return e_not_supported;
}

