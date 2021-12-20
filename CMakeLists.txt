cmake_minimum_required (VERSION 3.12)
project("naphy")
set(CMAKE_CXX_STANDARD 14)

# .cpp
file(GLOB_RECURSE naphy_src CONFIGURE_DEPENDS "src/*/*.cpp")
file(GLOB_RECURSE naphy_src CONFIGURE_DEPENDS "src/*.cpp")

# .h
include_directories(src/)
include_directories(include/)

# executable
add_executable(naphy "src/main.cpp" ${naphy_src})

# libs
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(naphy 
    # SDL

    -lmingw32 
    ${LIB_DIR}/libSDL2main.a 
    ${LIB_DIR}/SDL2.dll

    # SDL_Image

    ${LIB_DIR}/zlib1.dll
    ${LIB_DIR}/libpng16-16.dll
    ${LIB_DIR}/SDL2_image.dll
)

# copy res/ folder into build/res
# that way we have all the resources already there without manual copying
file(COPY res DESTINATION ./)