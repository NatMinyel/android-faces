# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++14 -Wno-error=format-security")
include(/home/nathnel/Downloads/dlib-19.4/dlib/cmake)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

set(DLIB_DIR /home/nathnel/Downloads/dlib-19.4)

include_directories(${DLIB_DIR} include)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED
${DLIB_DIR}/dlib/threads/threads_kernel_shared.cpp
            ${DLIB_DIR}/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp
            ${DLIB_DIR}/dlib/base64/base64_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_1.cpp
            ${DLIB_DIR}/dlib/threads/threads_kernel_2.cpp

            # ${DLIB_DIR}/dlib/image_loader/png_loader.cpp
            ${DLIB_DIR}/dlib/image_loader/jpeg_loader.cpp
            # ${DLIB_DIR}/dlib/image_saver/save_png.cpp
            # ${DLIB_DIR}/dlib/sockets/sockets_kernel_1.cpp
            # ${DLIB_DIR}/dlib/bsp/bsp.cpp

            # ${DLIB_DIR}/dlib/dir_nav/dir_nav_kernel_1.cpp
            # ${DLIB_DIR}/dlib/dir_nav/dir_nav_kernel_2.cpp
            # ${DLIB_DIR}/dlib/dir_nav/dir_nav_extensions.cpp
            # ${DLIB_DIR}/dlib/linker/linker_kernel_1.cpp
            # ${DLIB_DIR}/dlib/logger/extra_logger_headers.cpp
            # ${DLIB_DIR}/dlib/logger/logger_kernel_1.cpp
            # ${DLIB_DIR}/dlib/logger/logger_config_file.cpp
            # ${DLIB_DIR}/dlib/misc_api/misc_api_kernel_1.cpp
            # ${DLIB_DIR}/dlib/misc_api/misc_api_kernel_2.cpp
            # ${DLIB_DIR}/dlib/sockets/sockets_extensions.cpp
            # ${DLIB_DIR}/dlib/sockets/sockets_kernel_2.cpp
            # ${DLIB_DIR}/dlib/sockstreambuf/sockstreambuf.cpp
            # ${DLIB_DIR}/dlib/sockstreambuf/sockstreambuf_unbuffered.cpp
            # ${DLIB_DIR}/dlib/server/server_kernel.cpp
            # ${DLIB_DIR}/dlib/server/server_iostream.cpp
            # ${DLIB_DIR}/dlib/server/server_http.cpp
            # ${DLIB_DIR}/dlib/threads/multithreaded_object_extension.cpp
            # ${DLIB_DIR}/dlib/threads/threaded_object_extension.cpp
            # ${DLIB_DIR}/dlib/threads/threads_kernel_1.cpp
            # ${DLIB_DIR}/dlib/threads/threads_kernel_2.cpp
            # ${DLIB_DIR}/dlib/threads/threads_kernel_shared.cpp
            # ${DLIB_DIR}/dlib/threads/thread_pool_extension.cpp
            # ${DLIB_DIR}/dlib/threads/async.cpp
            # ${DLIB_DIR}/dlib/timer/timer.cpp
            # ${DLIB_DIR}/dlib/stack_trace.cpp

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                       -ljnigraphics

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       dlib::dlib)