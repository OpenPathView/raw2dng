#include <stdio.h>
#include <string.h>
#pragma hdrstop

/*
   Coolpix RAW to DNG converter
   by paul69, v0.2, 2006-01-24, <e2500@narod.ru>

   Lossless compression : template implementation
*/

#include "write_dng.h"

////////////////////////////////////////////////////////////////////////////////

template <class TBitBuffer> class TBitBuffer_T
{
public:
   static bool lossless_pack(TBitBuffer& b, TScanData const* scan_data, color_buf Tile );
   static bool std_lossless_pack(TBitBuffer& b, TScanData const* scan_data, color_buf Tile );
   static void pack(TBitBuffer& b, THuffTab2 const* HT, int Pt, color_ptr pix, int predictor, int dx, int dy );
};

template <class TWriter> class TWriter_T
{
public:
   static void save( TScanData* scan_data, TWriter& out, color_buf picture );
};

template <class TBitBuffer>
bool TBitBuffer_T<TBitBuffer>::std_lossless_pack(TBitBuffer& b, TScanData const* scan_data, color_buf Tile )
{
   THuffTab2 const* HT[2];
   HT[0] = &scan_data->tabs[0];
   HT[1] = &scan_data->tabs[1];

   TFramePar const& frame = scan_data->frame;
   int dy = frame.n_mcu_X*frame.mcu;

   color_ptr tile_ = Tile;

   {
      color_ptr tile = tile_;

      b.put_dc( HT[0], tile[0] - 0x8000 );
      b.put_dc( HT[1], tile[1] - 0x8000 );

      tile += 2;
      for(int i_mcu=1; i_mcu < (int)frame.n_mcu_X; ++i_mcu, tile += 2 )
      {
         b.put_dc( HT[0], tile[0] - tile[-2] );
         b.put_dc( HT[1], tile[1] - tile[1-2] );
      }
   }

   tile_ += dy;

   for( int i_line = 1; i_line < (int)frame.n_mcu_Y; ++i_line, tile_ += dy )
   {
      color_ptr tile = tile_;

      b.put_dc( HT[0], tile[0] - tile[-dy] );
      b.put_dc( HT[1], tile[1] - tile[1-dy] );

      tile += 2;
      for(int i_mcu=1; i_mcu < (int)frame.n_mcu_X; ++i_mcu, tile += 2 )
      {
         b.put_dc( HT[0], tile[0] - tile[-2] );
         b.put_dc( HT[1], tile[1] - tile[1-2] );
      }
   }
   b.flash(-1);
   return true;
}

template <class TBitBuffer>
void TBitBuffer_T<TBitBuffer>::pack(TBitBuffer& b, THuffTab2 const* HT,
   int Pt, color_ptr pix, int predictor, int dx, int dy )
{
   int Px;
   switch( predictor )
   {
   default:
   case -1: Px = 0x8000 >> Pt; break;
   case 0: Px = 0; break;
   case 1: Px = pix[-dx]; break;
   case 2: Px = pix[-dy]; break;
   case 3: Px = pix[-dx-dy]; break;
   case 4: Px = pix[-dx] + pix[-dy] - pix[-dx-dy]; break;
   case 5: Px = pix[-dx] + ((int)(pix[-dy] - pix[-dx-dy])>>1); break;
   case 6: Px = pix[-dy] + ((int)(pix[-dx] - pix[-dx-dy])>>1); break;
   case 7: Px = (int)(pix[-dx] + pix[-dy])>>1; break;
   }

   b.put_dc( HT, pix[0] - Px );
}

template <class TBitBuffer>
bool TBitBuffer_T<TBitBuffer>::lossless_pack(TBitBuffer& b,
   TScanData const* scan_data, color_buf Tile )
{
   THuffTab2 const* HT[10];

   TScanPar const& scan = scan_data->scan;
   TFramePar const& frame = scan_data->frame;
   int offset[10];

   int scan_mcu_size = 0;
   for(int i=0, k=0; i<(int)scan.Ns; ++i)
   {
      int N = frame.item[ scan.pars[i].i ].N;
      scan_mcu_size += N;
      int off = i >= 2 ? frame.n_mcu_X*frame.mcu/2 + i-2 : i;
      for(int j=0; j<N; ++j, ++k)
      {
         HT[k] = &scan_data->tabs[ scan.pars[i].Td ];
         offset[k] = off + j;
      }
   }
   //D_ASSERT(k == frame.mcu)
   if( scan.Ns == 2 && scan_mcu_size == 2 && scan.Al == 0 && scan.Ss == 1
    && HT[0] == &scan_data->tabs[0] && HT[1] == &scan_data->tabs[1] )
      return std_lossless_pack(b, scan_data, Tile );

   int dx = 1;
   int dy = frame.n_mcu_X*frame.mcu;

   if( scan.Ns == 2 )
      dx *= 2;

   if( scan.Ns == 4 )
      dx *= 2;

   color_ptr tile_ = Tile;
   for( int i_line = 0; i_line < (int)frame.n_mcu_Y; ++i_line, tile_ += dy )
   {
      int predictor = i_line == 0 ? -1 : 2;
      int predictor2 = i_line == 0 ? 1 : scan.Ss;
      color_ptr tile = tile_;

      for(int i_mcu=0; i_mcu < (int)frame.n_mcu_X; ++i_mcu, tile += dx )
      {
         // encode MCU
         for(int i=0; i<scan_mcu_size; ++i)
         {
            pack(b, HT[i], scan.Al, tile+offset[i], predictor, dx, dy );
         }

         predictor = predictor2;
      }
   }
   b.flash(-1);
   return true;
}
/////////////////////////////////////////////////////
class TNullWriter_T
{
   unsigned curr;
   unsigned buff_size;

public:
   TNullWriter_T() : curr(0), buff_size(0) {}

   int okay() const { return 1; }

   long tell() { return curr; }
   void seek( long p ) { curr = p; }
   void skip( long p ) { curr += p; }

   int write( const void*, unsigned size ) {
      curr += size;
      if( curr > buff_size )
         buff_size = curr;
      return size;
   }
   int put( int c ) {
      ++curr;
      if( curr > buff_size )
         buff_size = curr;
      return 1;
   }
};

class TFileWriter_T
{
   FILE* f;

public:

   TFileWriter_T(FILE* _f) : f(_f) {}

   int okay() const { return f != 0; }

   long tell() { return ftell(f); }
   void seek( long p ) { fseek(f, p, SEEK_SET); }
   void skip( long p ) { fseek(f, p, SEEK_CUR); }

   int write( const void* buf, unsigned size )
   {
      return fwrite( buf, size, 1, f) ? size : 0;
   }
   int put( int c )
   {
      return fputc( c, f ) != EOF;
   }
};


class TBitAdjust_T
{
public:
   TBitAdjust_T() {}

   void put_dc(THuffTab2 const* tab, int x)
   {
      int len = get_len(x);

      tab->count[len] ++;
   }

   void flash(int)
   {
   }
};

template <class TWriter> class TBitWriter_T
{
public:
   TBitWriter_T( TWriter& o ) : out(o) { i_bit=32; bitbuf=0; }

   TWriter& out;

   void put_dc(THuffTab2 const* tab, int x)
   {
      int len = get_len(x);

      put( tab->hufftab[len] );

#if DNG_VER == 0x01010000
      if( x != 0 && len < 16 )
      {
         put( len, x );
      }
#else
      // bug for len == 16
      if( x != 0 )
      {
         put( len, x );
      }
#endif
   }

   void put(int len, int x)
   {
      if( i_bit < len )
         make_space();

      i_bit -= len;

      if( x >= 0 )
      {
         bitbuf |= x << i_bit;
      }
      else
      {
         bitbuf |= ((x-1)&((1<<len)-1)) << i_bit;
      }
   }

   void put(THuffRec r)
   {
      if( i_bit < r.len )
         make_space();

      bitbuf |= r.code << (i_bit -= r.len);
   }

   void make_space();
   void flash(int);

   int i_bit;
   unsigned bitbuf;
};

template <class TWriter> 
void TBitWriter_T<TWriter>::flash(int fill)
{
   if( i_bit < 24 )
      make_space();

   if( i_bit < 32 )
   {
      if( fill == -1 )
         bitbuf |= (1<<i_bit)-1;

      i_bit -= 8;
      make_space();
   }
}

template <class TWriter>
void TBitWriter_T<TWriter>::make_space()
{
   if( i_bit >= 24 )
      return;

   do
   {
      out.put( bitbuf >> 24 );

      if( (bitbuf >> 24) == 0xFF )
         out.put(0);

      bitbuf <<= 8;
      i_bit += 8;
   } while( i_bit <= 24 );
}


template <class TWriter>
void TWriter_T<TWriter>::save( TScanData* scan_data, TWriter& out, color_buf picture )
{
   // SOI
   out.put(0xFF);
   out.put(0xD8);

   // SOF3
   out.put(0xFF);
   out.put(0xC3);

   TFramePar const frame = scan_data->frame;

   int size = 2+1+2+2+1 + frame.Nf*(1+1+1);
   out.put(size>>8);
   out.put(size&255);

   out.put(frame.P); //P
   out.put(frame.Y>>8); //Y
   out.put(frame.Y&255); //Y
   out.put(frame.X>>8); //X
   out.put(frame.X&255); //X
   out.put(frame.Nf);

   for(int i=0; i<(int)frame.Nf; ++i)
   {
      out.put( frame.item[i].C);
      out.put( (frame.item[i].H << 4)|frame.item[i].V );
      out.put( frame.item[i].Tq);
   }

   THuffTab2* tabs = scan_data->tabs;

   int nTabs = 0;
   for(int i=0; i<4; ++i)
   {
      THuffTab2* t = &tabs[i];
      if( !t->used ) continue;

      t->Th = nTabs;
      ++nTabs;
   }

   // DHT
   out.put(0xFF);
   out.put(0xC4);

   size = 2;
   for(int i=0; i<4; ++i)
   {
      THuffTab2 const* t = &tabs[i];
      if( !t->used ) continue;

      size += 1+16 + t->nCode;
   }

   out.put(size>>8);
   out.put(size&255);

   for(int i=0; i<4; ++i)
   {
      THuffTab2 const* t = &tabs[i];
      if( !t->used ) continue;

      out.put( (t->Tc<<4)|t->Th );

      for(int i = 0; i<16; ++i)
         out.put(t->bits[i]);

      for(int i = 0; i<t->nCode; ++i)
         out.put(t->code[i]);
   }

   // SOS
   out.put(0xFF);
   out.put(0xDA);

   TScanPar const scan = scan_data->scan;
   
   size = 2+1+scan.Ns*(1+1)+1+1+1;// SOS

   out.put(size>>8);
   out.put(size&255);
   out.put(scan.Ns); // Ns

   for(int i=0; i<(int)scan.Ns; ++i)
   {
      out.put( scan.pars[i].Cs );

      int Td = tabs[ scan.pars[i].Td ].Th;
      int Ta = 0;

      out.put( (Td<<4)|Ta ); // Td,Ta
   }
   out.put(scan.Ss); // Ss
   out.put(scan.Se); // Se
   out.put( (scan.Ah<<4) | scan.Al ); // Ah,Al

   TBitWriter_T<TWriter> bitbuf(out);

   TBitBuffer_T< TBitWriter_T<TWriter> >::lossless_pack(bitbuf, scan_data, picture);

   out.put(0xFF);
   out.put(0xD9);
}

class TScanData_T : public TScanData
{
public:
   TScanData_T(int W, int H, color_buf );
   TScanData_T(int W, int H, int Pt, int predictor, color_buf );
};

TScanData_T::TScanData_T(int Width, int Height, color_buf picture)
   : TScanData(Width, Height)
{
   // initialize huff tables
   TBitAdjust_T adj;
   TBitBuffer_T<TBitAdjust_T>::std_lossless_pack(adj, this, picture);

   tabs[0].generate_codes();
   tabs[1].generate_codes();
}

TScanData_T::TScanData_T(int Width, int Height, int Pt,
   int predictor, color_buf picture)
: TScanData( Width,  Height,  Pt, predictor )
{
   // initialize huff tables
   TBitAdjust_T adj;
   TBitBuffer_T<TBitAdjust_T>::lossless_pack(adj, this, picture);

   tabs[0].generate_codes();
   tabs[1].generate_codes();

   if( tabs[0] == tabs[1] )
   {
      scan.pars[1].Td = 0;
      tabs[1].used = 0;
   }

   if( predictor > 1 )
   {
      tabs[2].generate_codes();
      tabs[3].generate_codes();

      if( tabs[0] == tabs[2] )
      {
         scan.pars[2].Td = 0;
         tabs[2].used = 0;
      }
      if( tabs[0] == tabs[3] )
      {
         scan.pars[3].Td = 0;
         tabs[3].used = 0;
      }

      if( tabs[1] == tabs[2] )
      {
         scan.pars[2].Td = 1;
         tabs[2].used = 0;
      }
      if( tabs[1] == tabs[3] )
      {
         scan.pars[3].Td = 1;
         tabs[3].used = 0;
      }

      if( tabs[2] == tabs[3] )
      {
         scan.pars[3].Td = 2;
         tabs[3].used = 0;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////

//unsigned get_tile_size( color_buf tile, int W, int H )
//{
//   TScanData_T scan_data( W, H, tile );

//   TNullWriter_T writer;
//   TWriter_T<TNullWriter_T>::save( &scan_data, writer, tile );
//   return writer.tell();
//}

//unsigned get_tile_size( color_buf tile, int W, int H, int Pt, int predictor )
//{
//   TScanData_T scan_data( W, H, Pt, predictor, tile );

//   TNullWriter_T writer;
//   TWriter_T<TNullWriter_T>::save( &scan_data, writer, tile );
//   return writer.tell();
//}

//void write_tile( FILE* out, color_buf tile, int W, int H )
//{
//   TScanData_T scan_data( W, H, tile );

//   TFileWriter_T writer( out );

//   TWriter_T<TFileWriter_T>::save( &scan_data, writer, tile );
//}

//void write_tile( FILE* out, color_buf tile, int W, int H, int Pt, int predictor )
//{
//   TScanData_T scan_data( W, H, Pt, predictor, tile );

//   TFileWriter_T writer( out );

//   TWriter_T<TFileWriter_T>::save( &scan_data, writer, tile );
//}
