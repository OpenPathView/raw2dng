TEMPLATE = lib
CONFIG = staticlib

SOURCES += \
    camera_id.cpp \
    dead_pix.cpp \
    get_len.cpp \
    jpeg.cpp \
    mask_dead.cpp \
    packtilet.cpp \
    packtilev.cpp \
    raw2nef.cpp \
    read_cfa.cpp \
    read_dng.cpp \
    read_raw.cpp \
    read_raw2.cpp \
    thumbnail.cpp \
    tiff_ifd.cpp \
    winstr.cpp \
    write_dng.cpp \
    write_ifd.cpp \
    read_exif.cpp \
    read_nikon.cpp \
    write_nef.cpp

DISTFILES +=

HEADERS += \
    buffer.h \
    file.h \
    listof.h \
    raw2dng.h \
    raw2nef.h \
    res.rc \
    res_en.rc \
    resource.h \
    resource.rc \
    resource_rus.rc \
    res_rus.rc \
    swap.h \
    tiff.h \
    winstr.h \
    write_dng.h \
    jpeg.h
