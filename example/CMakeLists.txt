# External common
find_package(fmt REQUIRED)
find_package(tabulate REQUIRED)

# Targets

add_executable(print_state)
set_target_properties(print_state PROPERTIES FOLDER example)

target_link_libraries(print_state PRIVATE dual_sense_hid fmt::fmt tabulate::tabulate)
target_sources(print_state PRIVATE print_state.cpp)


add_executable(led_effects)
set_target_properties(led_effects PROPERTIES FOLDER example)

target_link_libraries(led_effects PRIVATE dual_sense_hid fmt::fmt tabulate::tabulate)
target_sources(led_effects PRIVATE led_effects.cpp)