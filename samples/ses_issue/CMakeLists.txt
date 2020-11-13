cmake_minimum_required(VERSION 3.14)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

# NORDIC SDK APP START
target_sources(app PRIVATE main.c)
# NORDIC SDK APP END

add_custom_command(TARGET app
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/test_ses_issue_1
                   )

add_custom_command(TARGET app
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/test_ses_issue_2
                   )
