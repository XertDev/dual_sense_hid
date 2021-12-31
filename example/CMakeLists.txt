# External common
find_package(fmt REQUIRED)
find_package(tabulate REQUIRED)

# Targets

add_executable(print_state)
set_target_properties(print_state PROPERTIES FOLDER example)

target_link_libraries(print_state PRIVATE dual_sense fmt::fmt tabulate::tabulate)
target_sources(print_state PRIVATE print_state.cpp)