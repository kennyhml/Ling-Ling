cmake_minimum_required(VERSION 3.25)
project(LLPP)

set(CMAKE_CXX_STANDARD 23)

add_executable(LLPP WIN32
        LLPP/external/imgui/imconfig.h
        LLPP/external/imgui/imgui.cpp
        LLPP/external/imgui/imgui.h
        LLPP/external/imgui/imgui_demo.cpp
        LLPP/external/imgui/imgui_draw.cpp
        LLPP/external/imgui/imgui_impl_dx9.cpp
        LLPP/external/imgui/imgui_impl_dx9.h
        LLPP/external/imgui/imgui_impl_win32.cpp
        LLPP/external/imgui/imgui_impl_win32.h
        LLPP/external/imgui/imgui_internal.h
        LLPP/external/imgui/imgui_stdlib.cpp
        LLPP/external/imgui/imgui_stdlib.h
        LLPP/external/imgui/imgui_tables.cpp
        LLPP/external/imgui/imgui_widgets.cpp
        LLPP/external/imgui/imstb_rectpack.h
        LLPP/external/imgui/imstb_textedit.h
        LLPP/external/imgui/imstb_truetype.h
        LLPP/external/keyauth/auth.hpp
        LLPP/external/keyauth/skStr.h
        LLPP/src/auth/auth.cpp
        LLPP/src/auth/auth.h
        LLPP/src/bots/crafting/basepowderstation.cpp
        LLPP/src/bots/crafting/basepowderstation.h
        LLPP/src/bots/crafting/craftingmanager.cpp
        LLPP/src/bots/crafting/craftingmanager.h
        LLPP/src/bots/crafting/embeds.cpp
        LLPP/src/bots/crafting/embeds.h
        LLPP/src/bots/crafting/gunpowderstation.cpp
        LLPP/src/bots/crafting/gunpowderstation.h
        LLPP/src/bots/crafting/sparkpowderstation.cpp
        LLPP/src/bots/crafting/sparkpowderstation.h
        LLPP/src/bots/drops/config.h
        LLPP/src/bots/drops/cratemanager.cpp
        LLPP/src/bots/drops/cratemanager.h
        LLPP/src/bots/drops/embeds.cpp
        LLPP/src/bots/drops/embeds.h
        LLPP/src/bots/drops/lootcratestation.cpp
        LLPP/src/bots/drops/lootcratestation.h
        LLPP/src/bots/farm/commands.cpp
        LLPP/src/bots/farm/commands.h
        LLPP/src/bots/farm/farmbot.cpp
        LLPP/src/bots/farm/farmbot.h
        LLPP/src/bots/kitchen/cropmanager.cpp
        LLPP/src/bots/kitchen/cropmanager.h
        LLPP/src/bots/kitchen/cropstation.cpp
        LLPP/src/bots/kitchen/cropstation.h
        LLPP/src/bots/kitchen/embeds.cpp
        LLPP/src/bots/kitchen/embeds.h
        LLPP/src/bots/kitchen/sapmanager.cpp
        LLPP/src/bots/kitchen/sapmanager.h
        LLPP/src/bots/kitchen/sapstation.cpp
        LLPP/src/bots/kitchen/sapstation.h
        LLPP/src/bots/paste/embeds.cpp
        LLPP/src/bots/paste/embeds.h
        LLPP/src/bots/crafting/grindstation.cpp
        LLPP/src/bots/crafting/grindstation.h
        LLPP/src/bots/paste/pastemanager.cpp
        LLPP/src/bots/paste/pastemanager.h
        LLPP/src/bots/paste/pastestation.cpp
        LLPP/src/bots/paste/pastestation.h
        LLPP/src/bots/render/renderstation.cpp
        LLPP/src/bots/render/renderstation.h
        LLPP/src/bots/suicide/suicideembed.cpp
        LLPP/src/bots/suicide/suicideembed.h
        LLPP/src/bots/suicide/suicidestation.cpp
        LLPP/src/bots/suicide/suicidestation.h
        LLPP/src/common/hsvfilter.h
        LLPP/src/common/hsvtester.cpp
        LLPP/src/common/hsvtester.h
        LLPP/src/common/util.cpp
        LLPP/src/common/util.h
        LLPP/src/config/config.cpp
        LLPP/src/config/config.h
        LLPP/src/config/managedvar.cpp
        LLPP/src/config/managedvar.h
        LLPP/src/config/required.h
        LLPP/src/config/validate.h
        LLPP/src/core/basestation.cpp
        LLPP/src/core/basestation.h
        LLPP/src/core/discord.cpp
        LLPP/src/core/discord.h
        LLPP/src/core/istationmanager.h
        LLPP/src/core/recovery.cpp
        LLPP/src/core/recovery.h
        LLPP/src/core/stationresult.h
        LLPP/src/core/task.cpp
        LLPP/src/core/task.h
        LLPP/src/core/taskmanager.cpp
        LLPP/src/core/taskmanager.h
        LLPP/src/fonts/fa.h
        LLPP/src/fonts/IconsFontAwesome6.h
        LLPP/src/gui/custom.cpp
        LLPP/src/gui/custom.h
        LLPP/src/gui/gui.cpp
        LLPP/src/gui/gui.h
        LLPP/src/main.cpp
        LLPP/src/bots/crafting/forgestation.cpp
        LLPP/src/bots/crafting/forgestation.h
        LLPP/src/bots/crafting/arbstation.cpp
        LLPP/src/bots/crafting/arbstation.h
        LLPP/src/bots/crafting/arbstation.cpp
        LLPP/src/bots/parasaur/parasaurstation.cpp
        LLPP/src/bots/parasaur/parasaurstation.h
        LLPP/src/gui/parasaur.cpp
        LLPP/src/gui/parasaur.h
        LLPP/src/gui/common.cpp
        LLPP/src/gui/common.h
        LLPP/src/gui/util.cpp
        LLPP/src/gui/util.h
        LLPP/src/gui/state.h
        LLPP/src/gui/tabs.h
        LLPP/src/gui/custom/combo_dynamic.cpp
        LLPP/src/gui/custom/combo_dynamic.h
        LLPP/src/gui/custom/dialogs.cpp
        LLPP/src/gui/custom/dialogs.h
        LLPP/src/bots/parasaur/config.h
        LLPP/src/bots/parasaur/parasaurmanager.cpp
        LLPP/src/bots/parasaur/parasaurmanager.h
        LLPP/src/bots/parasaur/embeds.cpp
        LLPP/src/bots/parasaur/embeds.h
        LLPP/src/core/bedstation.cpp
        LLPP/src/core/bedstation.h
        LLPP/src/core/teleportstation.cpp
        LLPP/src/core/teleportstation.h
        LLPP/src/discord/bot.cpp
        LLPP/src/discord/bot.h
        LLPP/src/discord/tribelogs/alerts.cpp
        LLPP/src/discord/tribelogs/alerts.h
        LLPP/src/discord/tribelogs/stream.cpp
        LLPP/src/discord/tribelogs/stream.h
        LLPP/src/discord/tribelogs/handler.cpp
        LLPP/src/discord/tribelogs/handler.h
        LLPP/src/discord/icons.h
)
find_package(dpp CONFIG REQUIRED)
target_link_libraries(LLPP PRIVATE dpp::dpp)
include_directories("${VCPKG_INSTALLED_DIR}/x64-windows/include/dpp")

# opencv
set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")
find_package(OpenCV REQUIRED)
target_link_libraries(LLPP PRIVATE ${OpenCV_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

# asapp
include_directories("C:/dev/ASAPP/ASAPP/include")
target_link_libraries(LLPP PRIVATE
        $<$<CONFIG:Debug>: "C:/dev/ASAPP/build/debug/asapp.lib">
        $<$<CONFIG:Release>: "C:/dev/ASAPP/build/release/asapp.lib">
)

# tesseract
find_package(Tesseract CONFIG REQUIRED)
target_link_libraries(LLPP PRIVATE Tesseract::libtesseract)

# d3d9
target_link_libraries(LLPP PRIVATE d3d9.lib)

# auth
target_link_libraries(LLPP PRIVATE
        $<$<CONFIG:Debug>: "C:/dev/LLPP/LLPP/external/keyauth/authd.lib">
        $<$<CONFIG:Release>: "C:/dev/LLPP/LLPP/external/keyauth/auth.lib">
)
