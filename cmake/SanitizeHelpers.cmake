set(COMMON_SAN_CFLAGS "-fuse-ld=gold -fno-sanitize-recover")

# Address Sanitizer
set(ASAN_CFLAGS "-g -O1 ${COMMON_SAN_CFLAGS} -fsanitize=address -fsanitize=leak")
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

# Undefined Behavior Sanitizer
set(USAN_CFLAGS "${COMMON_SAN_CFLAGS} -fsanitize=undefined")
set(CMAKE_C_FLAGS_USAN
    "${USAN_CFLAGS}"
    CACHE STRING "Undefined Behavior Sanitizer C compiler flags"
    FORCE
)
set(CMAKE_CXX_FLAGS_USAN
    "${USAN_CFLAGS}"
    CACHE STRING "Undefined Behavior Sanitizer CXX compiler flags"
    FORCE
)

# Thread Sanitizer
set(TSAN_CFLAGS "${COMMON_SAN_CFLAGS} -fsanitize=thread")
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
