cmake_minimum_required(VERSION 3.25)
project(llpp)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(/utf-8)

add_executable(llpp
        src/main.cpp
        src/integration/discord/commands/config.cpp
        src/integration/discord/commands/config.h
        src/integration/discord/commands/setup.cpp
        src/integration/discord/commands/setup.h
        src/integration/discord/commands/common.h
        src/integration/webhook/webhook.h
        src/configuration/validate.cpp
        src/configuration/validate.h
        src/configuration/managedvar.h
        src/configuration/common.h
        src/utility/utility.h
        src/configuration/exceptions.h
        src/core/exceptions.h
        src/integration/discord/config.cpp
        src/integration/discord/config.h
        src/integration/discord/discord.cpp
        src/integration/discord/discord.h
        src/configuration/common.cpp
        src/utility/utility.cpp
        src/core/startup.cpp
        src/core/startup.h
        src/tasksystem/task.cpp
        src/tasksystem/task.h
        src/tasksystem/persistenttask.h
        src/tasksystem/persistenttask.cpp
)

add_dependencies(llpp asapp)
target_link_libraries(llpp PRIVATE asapp)
target_include_directories(llpp PRIVATE src)

find_package(dpp CONFIG REQUIRED)
target_link_libraries(llpp PRIVATE dpp::dpp)
include_directories("${VCPKG_INSTALLED_DIR}/x64-windows/include/dpp")

# opencv
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")
find_package(OpenCV REQUIRED)
target_link_libraries(llpp PRIVATE ${OpenCV_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

# tesseract
find_package(Tesseract CONFIG REQUIRED)
target_link_libraries(llpp PRIVATE Tesseract::libtesseract)