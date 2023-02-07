find_package(fmt)

add_library(hugegrad derivative.hpp scalar.cpp scalar.hpp operation.hpp gen-vis.hpp)
target_link_libraries(hugegrad fmt::fmt-header-only)

