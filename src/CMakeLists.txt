add_library(hugegrad derivative.hpp tensor.cpp tensor.hpp)

add_executable(main main.cpp)

find_package(fmt)

target_link_libraries(main hugegrad fmt::fmt-header-only)

