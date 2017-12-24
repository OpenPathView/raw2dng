# raw2dng

This projet is a porting to GNU/Linux of the project [raw2dng](http://e2500.narod.ru/raw2dng_e.htm).

Extract from the presentation that can be found in the homepage of the project:


RAW to DNG converter converts native raw data of some digital still cameras to DNG format. DNG is Digital Negative format from Adobe. It is supported by Adobe Photoshop CS and late, and some other RAW converters. It's the best format for native raw. It uses lossless compression, stores EXIF data from paired JPEG file, has small preview and allows straight processing by Photoshop. 

## How to compile it

### Install depedencies

I used qmake to compile this project despite of the fact that I don't use QT... To build it you need:

* build-essential
* qmake

On ubuntu

```
apt-get install qt-sdk build-essential
```

### Build it

```
qmake
make
```

### Install it

```
$ sudo make install
cd common/ && make -f Makefile install
make[1]: Entering directory '/root/raw2dng/common'
make[1]: Nothing to be done for 'install'.
make[1]: Leaving directory '/root/raw2dng/common'
cd raw2dng/ && make -f Makefile install
make[1]: Entering directory '/root/raw2dng/raw2dng'
install -m 755 -p "raw2dng" "/usr/local/bin/raw2dng"
strip "/usr/local/bin/raw2dng"
make[1]: Leaving directory '/root/raw2dng/raw2dng'
cd raw2nef/ && make -f Makefile install
make[1]: Entering directory '/root/raw2dng/raw2nef'
install -m 755 -p "raw2nef" "/usr/local/bin/raw2nef"
strip "/usr/local/bin/raw2nef"
make[1]: Leaving directory '/root/raw2dng/raw2nef'
cd diadpix/ && make -f Makefile install
make[1]: Entering directory '/root/raw2dng/diadpix'
install -m 755 -p "diadpix" "/usr/local/bin/diadpix"
strip "/usr/local/bin/diadpix"
make[1]: Leaving directory '/root/raw2dng/diadpix'
```

## How to use it

For all this example, my raw2nef.ini file was:

```
;config by nutsey

24000000, 0, 4000, 3000, 0, 8000, 16, 4, GOPRO-H3BE

31850496, 0, 4608, 3456, 7, 9216, 16, 4, XIAOMI-YI
```

### Convert raw to dng

```
$ ./raw2dng Coolpix RAW data to DNG format converter
raw2dng v0.16.6.2, by paul69 <e2500@narod.ru>
[compiled: Dec 24 2017 03:03:25]

args: [options] input_file1 [input_file2...]

options:
   -auto - autodetect (default)
   -<camera id> - see raw2nef.ini

   -v - verbose
   -c - compatible with Adobe DNG Converter (default)
   -n - EXIF/JPEG file has the same number as RAW file
   -f - force loading incomplete raw files
   -O - optimize DNG file size (very slow)
   -e <filename> - EXIF/JPEG file name
   -o <filename> - DNG output file name
   -i <filename> - ini-file name (default is raw2nef.ini)
   With -e and -o options only one input file is allowed
```

Example:

```
$ raw2dng -auto -O -o test.dng -v YDXJ0014.RAW
raw2dng v0.16.6.2, by paul69 <e2500@narod.ru>
[compiled: Dec 24 2017 01:50:33]
input: YDXJ0014.RAW
warning: "YDXJ0015.jpg" - cannot open exif file
parse ini-file: "./raw2nef.ini"
camera model XIAOMI is found in ini-file
read raw data from "YDXJ0014.RAW"
write picture to: "test.dng"
OK
```

### Convert raw to nef

```
$ raw2nef 
Coolpix RAW data to E5400/E5700/E8700 NEF file converter
raw2nef v0.16.6.2, by paul69 <e2500@narod.ru>
[compiled: Dec 24 2017 02:03:41]

args: [options] input_file1 [input_file2...]

options:
   -auto - autodetect (default)
   -<camera id> - see raw2nef.ini

   -v - verbose
   -c - compatible with E5400/E5700/E8700
   -n - EXIF/JPEG file has the same number as RAW file
   -f - force loading incomplete raw files
   -e <filename> - EXIF/JPEG file name
   -o <filename> - NEF output file name
   -i <filename> - ini-file name (default is raw2nef.ini)
   With -e and -o options only one input file is allowed
```

Example

```
$ raw2nef -v -f -o test.nef YDXJ0014.RAW 
raw2nef v0.16.6.2, by paul69 <e2500@narod.ru>
[compiled: Dec 24 2017 02:03:41]
input: YDXJ0014.RAW
warning: "YDXJ0015.jpg" - cannot open exif file
parse ini-file: "raw2nef.ini"
camera model XIAOMI is found in ini-file
read raw data from "YDXJ0014.RAW"
write picture to: "test.nef"
OK
```

### Search black and luminous dead pixels

```
$ diadpix 
Search black and luminous dead pixels
dead_pix v0.16.6.2, by paul69 <e2500@narod.ru>
[compiled: Dec 24 2017 02:03:41]

args: [options] input_file

options:
   -auto - autodetect (default)
   -<camera id> - see raw2nef.ini

   -v - verbose
   -b - search luminous pixels on black shot
   -w - search black pixels on white shot
   -t100 - bright pixel detection threashold
   -d <filename> - dead pixels table output file
   -d+ <filename> - append existing file
   -i <filename> - ini-file name (default is raw2nef.ini)
```
