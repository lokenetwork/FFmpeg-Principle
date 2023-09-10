TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
#定义 FFmpeg 库的使用方法
LIBS_TYPE = msvc

#HEADERS += xxx.h


SOURCES += main.c

contains(QT_ARCH, i386) {
    message("32-bit")
}

contains(QT_ARCH, x86_64) {

    message("64-bit")

    message($$LIBS_TYPE)

    contains(LIBS_TYPE, mingw) {
        INCLUDEPATH += $$PWD/build64/ffmpeg-n4.4.1-mingw/include
        LIBS += $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libavformat.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libavcodec.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libavdevice.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libavfilter.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libavutil.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libpostproc.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libswresample.dll.a \
            $$PWD/build64/ffmpeg-n4.4.1-mingw/lib/libswscale.dll.a
    }

    contains(LIBS_TYPE, msvc) {
        QMAKE_CFLAGS += /utf-8
        INCLUDEPATH += $$PWD/build64/ffmpeg-n4.4.1-msvc/include
        LIBS += $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/avcodec.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/avdevice.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/avfilter.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/avformat.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/avutil.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/postproc.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/swresample.lib \
            $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/swscale.lib
        lib_install.files += $$PWD/build64/ffmpeg-n4.4.1-msvc/bin/*.dll
        CONFIG(debug, debug|release){
           lib_install.path = $${OUT_PWD}/Debug
        }
        CONFIG(release, debug|release){
           lib_install.path = $${OUT_PWD}/Release
        }
        other_install.files += $$PWD/juren-30s.mp4 $$PWD/logo.jpg
        other_install.path += $${OUT_PWD}
        INSTALLS += lib_install other_install
    }

    contains(LIBS_TYPE, msvc-static) {
    INCLUDEPATH += $$PWD/build64/ffmepg-4.4-msvc/include
    LIBS += $$PWD/build64/ffmepg-4.4-msvc/lib/libavcodec.a mfplat.lib mfuuid.lib ole32.lib strmiids.lib ole32.lib user32.lib \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libavdevice.a psapi.lib ole32.lib strmiids.lib uuid.lib oleaut32.lib shlwapi.lib gdi32.lib vfw32.lib \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libavfilter.a \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libavformat.a secur32.lib ws2_32.lib \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libavutil.a user32.lib bcrypt.lib \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libpostproc.a \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libswresample.a \
        $$PWD/build64/ffmepg-4.4-msvc/lib/libswscale.a
    }

}
