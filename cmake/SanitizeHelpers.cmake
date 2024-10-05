# Address Sanitizer
set(ASAN_CFLAGS "-g -O1 -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
set(CMAKE_C_FLAGS_ASAN
    "${ASAN_CFLAGS}"
    CACHE STRING "Address Sanitizer C compiler flags"
    FORCE
    )
set(CMAKE_CXX_FLAGS_ASAN
    "${ASAN_CFLAGS}"
    CACHE STRING "Address Sanitizer CXX compiler flags"
    FORCE
    )
set(CMAKE_EXE_LINKER_FLAGS_ASAN
    "-fuse-ld=gold -fsanitize=address -fsanitize=leak"
    CACHE STRING "Address Sanitizer linker flags"
    FORCE
    )

# Undefined Behavior Sanitizer
set(UBSAN_CFLAGS "-g -O1 -fsanitize=undefined")
set(CMAKE_C_FLAGS_UBSAN
    "${UBSAN_CFLAGS}"
    CACHE STRING "Undefined Behavior Sanitizer C compiler flags"
    FORCE
    )
set(CMAKE_CXX_FLAGS_UBSAN
    "${UBSAN_CFLAGS}"
    CACHE STRING "Undefined Behavior Sanitizer CXX compiler flags"
    FORCE
    )
set(CMAKE_EXE_LINKER_FLAGS_UBSAN
    "-fuse-ld=gold -fsanitize=undefined"
    CACHE STRING "Undefined Behavior Sanitizer linker flags"
    FORCE
    )

# Thread Sanitizer
set(TSAN_CFLAGS "-g -O2 -fsanitize=thread")
set(CMAKE_C_FLAGS_TSAN
    "${TSAN_CFLAGS}"
    CACHE STRING "Thread Sanitizer C compiler flags"
    FORCE
    )
set(CMAKE_CXX_FLAGS_TSAN
    "${TSAN_CFLAGS}"
    CACHE STRING "Thread Sanitizer CXX compiler flags"
    FORCE
    )
set(CMAKE_EXE_LINKER_FLAGS_TSAN
    "-fuse-ld=gold -fsanitize=thread"
    CACHE STRING "Thread Sanitizer linker flags"
    FORCE
    )
