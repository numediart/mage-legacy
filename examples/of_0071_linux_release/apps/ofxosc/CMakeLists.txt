SET(LIB_NAME ofxosc)

FILE(GLOB_RECURSE SRC ${SRC} *.cpp *.c)
FILE(GLOB_RECURSE HDR ${HDR} *.hpp *.h)

INCLUDE_DIRECTORIES(./src)
INCLUDE_DIRECTORIES(./libs/oscpack/src)
INCLUDE_DIRECTORIES(./libs/oscpack/src/osc)
INCLUDE_DIRECTORIES(./libs/oscpack/src/ip)
INCLUDE_DIRECTORIES(./libs/oscpack/src/ip/posix)

INCLUDE_DIRECTORIES(/usr/include/gstreamer-0.10)
INCLUDE_DIRECTORIES(/usr/include/glib-2.0)
INCLUDE_DIRECTORIES(/usr/lib/glib-2.0/include)
INCLUDE_DIRECTORIES(/usr/include/libxml2/)

#certainly not the cleanest way to do it, but it matches openframeworks' gcc command from the Makefile
#besides ... it works
#[OPENFRAMEWORKS]
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/glu/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/FreeImage/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/assimp/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/assimp/include/Compiler)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/cairo/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/cairo/include/cairo)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/cairo/include/pixman-1)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/cairo/include/libpng15)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/fmodex/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include/freetype2)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include/freetype2/freetype)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include/freetype2/freetype/config)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include/freetype2/freetype/internal)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/include/freetype2/freetype/internal/services)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/glew/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/glew/include/GL)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/kiss/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/portaudio/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/rtAudio/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/tess2/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/poco/include)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/math)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/gl)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/video)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/3d)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/types)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/graphics)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/app)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/events)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/sound)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/utils)
INCLUDE_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworks/communication)

LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/openFrameworksCompiled/lib/linux64/)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/poco/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/fmodex/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/freetype/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/glew/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/kiss/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/portaudio/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/rtAudio/lib/linux64)
LINK_DIRECTORIES(${OF_ROOT_PATH}/libs/tess2/lib/linux64)
#[/OPENFRAMEWORKS]

ADD_LIBRARY(${LIB_NAME} ${SRC} ${HDR})
TARGET_LINK_LIBRARIES(${LIB_NAME} pthread m)
TARGET_LINK_LIBRARIES(${LIB_NAME} gstreamer-0.10)

#[OPENFRAMEWORKS]
TARGET_LINK_LIBRARIES(${LIB_NAME} openFrameworks)
TARGET_LINK_LIBRARIES(${LIB_NAME} PocoNet)
TARGET_LINK_LIBRARIES(${LIB_NAME} PocoXML)
TARGET_LINK_LIBRARIES(${LIB_NAME} PocoUtil)
TARGET_LINK_LIBRARIES(${LIB_NAME} PocoFoundation)
TARGET_LINK_LIBRARIES(${LIB_NAME} freetype)
TARGET_LINK_LIBRARIES(${LIB_NAME} kiss)
TARGET_LINK_LIBRARIES(${LIB_NAME} tess2)
TARGET_LINK_LIBRARIES(${LIB_NAME} RtAudio)
TARGET_LINK_LIBRARIES(${LIB_NAME} fmodex)
TARGET_LINK_LIBRARIES(${LIB_NAME} GLEW)
TARGET_LINK_LIBRARIES(${LIB_NAME} gtk-x11-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} gdk-x11-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} atk-1.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} gio-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} pangoft2-1.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} gdk_pixbuf-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} pangocairo-1.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} cairo)
TARGET_LINK_LIBRARIES(${LIB_NAME} pango-1.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} fontconfig)
TARGET_LINK_LIBRARIES(${LIB_NAME} gobject-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} gmodule-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} gthread-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} rt)
TARGET_LINK_LIBRARIES(${LIB_NAME} glib-2.0)
TARGET_LINK_LIBRARIES(${LIB_NAME} mpg123)
TARGET_LINK_LIBRARIES(${LIB_NAME} glut)
TARGET_LINK_LIBRARIES(${LIB_NAME} GL)
TARGET_LINK_LIBRARIES(${LIB_NAME} asound)
TARGET_LINK_LIBRARIES(${LIB_NAME} openal)
TARGET_LINK_LIBRARIES(${LIB_NAME} sndfile)
TARGET_LINK_LIBRARIES(${LIB_NAME} vorbis)
TARGET_LINK_LIBRARIES(${LIB_NAME} FLAC)
TARGET_LINK_LIBRARIES(${LIB_NAME} ogg)
TARGET_LINK_LIBRARIES(${LIB_NAME} freeimage)
#[/OPENFRAMEWORKS]

TARGET_LINK_LIBRARIES(${LIB_NAME} portaudio)
TARGET_LINK_LIBRARIES(${LIB_NAME} jack)
