#ifndef __RAW2DNG_H
#define __RAW2DNG_H

class TRawParam;
class TAdvParam
{
public:
   TAdvParam()
   {
      compatible = 1;
      verbose = 0;
      deadpix = 0;

      age = 0;
   }

   int compatible; // 0 - use default, 1 - set, -1 - reset
   int verbose;
   int deadpix;

   WinString deadpix_fn;
   WinString options;
   int age;
   static int static_age;
};

class TPictureFile
{
public:
   TPictureFile(LPCTSTR);

   int index;

   WinString file_path;
   WinString file_name;
   unsigned file_size;

   enum {
      eCannotOpenFile = 1<<0,
      eJPEGFormat = 1<<1,
      eNoExifFile = 1<<2,
      eCannotReadExif = 1<<3,
      eAmbiguityModel = 1<<4,
      eUnsupportedFormat = 1<<5,
      fExifReady = 1<<6,
      fStateReady = 1<<7,
   };

   enum
   {
      fOutputDefined = 1<<1,
      fExifDefined = 1<<2,
   };

   enum {
      fNotReady,
      fStartProcess,
      fProcessFault,
      fInProgress,
      fSuccess,
      fFault,
   };

   unsigned flags;
   unsigned state;
   int result;

   TRawParam* camera;
   TRawParam* user_camera; // manual setting

   WinString model;
   WinString file_type;

   WinString listing;
   WinString comment;

   TIFF_Content tiff;
   IFDir* CFA;

   buff_t<char> ascii_fn;
   WinString exif_fn;

   WinString output_fn;

   TAdvParam adv;

   TPictureFile* next;
   bool visible;
};

struct TPictureFileRef
{
   TPictureFileRef(TPictureFile* f) : file(f), next(0) {}
   TPictureFile* file;
   TPictureFileRef* next;
};

class TRawParam
{
public:
   char* name;
   TCCDParam pars;
   TRawParam* next;

   TRawParam() : name(0) {}
   ~TRawParam(){ delete [] name; }
};

struct TStdParam
{
   bool incomplete;
   bool compatible;
   bool endian;
   bool optimize;
   bool verbose;
};


enum 
{
   ON_SIZE_MOVE_X = 1<<0,
   ON_SIZE_MOVE_Y = 1<<1,
   ON_SIZE_RESIZE_W = 1<<2,
   ON_SIZE_RESIZE_H = 1<<3,
   ON_SIZE_BOTTOM = 1<<4,
   ON_SIZE_MOVE = ON_SIZE_MOVE_X|ON_SIZE_MOVE_Y,
   ON_SIZE_RESIZE = ON_SIZE_RESIZE_W|ON_SIZE_RESIZE_H,
   ON_SIZE_CHANGE = ON_SIZE_MOVE|ON_SIZE_RESIZE|ON_SIZE_BOTTOM,

   ON_START_DISABLE = 1<<5,
};

struct TDlgItemFlags
{  
   int id;
   unsigned flags;
};

#endif
