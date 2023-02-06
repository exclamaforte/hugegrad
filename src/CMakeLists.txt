find_package(fmt)

add_library(hugegrad derivative.hpp tensor.cpp tensor.hpp scalar.cpp scalar.hpp operation.hpp)
target_link_libraries(hugegrad fmt::fmt-header-only)

add_executable(main main.cpp)

target_link_libraries(main hugegrad fmt::fmt-header-only)

