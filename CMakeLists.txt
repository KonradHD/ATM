cmake_minimum_required(VERSION 3.10)
project(ATM C)

set(CMAKE_C_STANDARD 11)

add_executable(ATM
        main.c
        globals.c
        save.c
        initializers.c
        validations.c
        cash_manager.c
        signal_handlers.c
        history.c
)

target_link_libraries(ATM pthread m)