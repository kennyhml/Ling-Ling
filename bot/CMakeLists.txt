cmake_minimum_required(VERSION 3.25)
project(llpp)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(/utf-8)

add_executable(llpp WIN32
        src/main.cpp
        src/integrations/discord/commands/config.cpp
        src/integrations/discord/commands/config.h
        src/integrations/discord/commands/setup.cpp
        src/integrations/discord/commands/setup.h
        src/integrations/discord/commands/common.h
        src/integrations/webhook/webhook.h
        src/configuration/validate.cpp
        src/configuration/validate.h
        src/configuration/managedvar.h
        src/configuration/common.h
        src/utility/utility.h
        src/configuration/exceptions.h
        src/core/exceptions.h
        src/integrations/discord/config.cpp
        src/integrations/discord/config.h
        src/integrations/discord/discord.cpp
        src/integrations/discord/discord.h
        src/configuration/common.cpp
        src/utility/utility.cpp
        src/core/startup.cpp
        src/core/startup.h
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