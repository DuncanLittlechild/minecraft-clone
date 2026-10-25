cmake_minimum_required(VERSION 3.24)
project(fastNoiseSIMD CXX)

set(FN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/FastNoiseSIMD")

# List the x86 sources explicitly (the NEON/ARM and AVX-512 files aren't wanted here)
add_library(fastNoiseSIMD STATIC
    "${FN_DIR}/FastNoiseSIMD.cpp"
    "${FN_DIR}/FastNoiseSIMD_internal.cpp"
    "${FN_DIR}/FastNoiseSIMD_sse2.cpp"
    "${FN_DIR}/FastNoiseSIMD_sse41.cpp"
    "${FN_DIR}/FastNoiseSIMD_avx2.cpp"
)

target_include_directories(fastNoiseSIMD PUBLIC "${FN_DIR}")

if(MSVC)
    set_source_files_properties("${FN_DIR}/FastNoiseSIMD_avx2.cpp" PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
else()
    # GCC/Clang need the instruction set enabled per file
    set_source_files_properties("${FN_DIR}/FastNoiseSIMD_sse2.cpp"  PROPERTIES COMPILE_OPTIONS "-msse2")
    set_source_files_properties("${FN_DIR}/FastNoiseSIMD_sse41.cpp" PROPERTIES COMPILE_OPTIONS "-msse4.1")
    set_source_files_properties("${FN_DIR}/FastNoiseSIMD_avx2.cpp"  PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma")
endif()

# FastNoiseSIMD treats "_WIN32" as "MSVC". On MinGW, neutralise the two MSVC-only bits.
if(WIN32 AND NOT MSVC)
    target_compile_definitions(fastNoiseSIMD PRIVATE
        __vectorcall=                  # MSVC calling convention GCC doesn't have
        _XCR_XFEATURE_ENABLED_MASK=0   # MSVC-only constant used for AVX detection
    )
    set_source_files_properties("${FN_DIR}/FastNoiseSIMD.cpp" PROPERTIES COMPILE_OPTIONS "-mxsave")  # allows _xgetbv()
endif()