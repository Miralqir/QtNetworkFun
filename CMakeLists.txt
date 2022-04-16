cmake_minimum_required(VERSION 3.0)

# Set various project variables
project(Kommunicator VERSION 0.1 HOMEPAGE_URL "vintilammm.ddns.net" LANGUAGES CXX)
set(CPACK_PACKAGE_VENDOR "VintilaMMM")

# Set C++ standards
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Decide what type of libraries to compile
#set(BUILD_SHARED_LIBS ON)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

# Find the Qt libraries
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

# Define source files
set(qtnetworktest_SRC
    src/main.cpp
    src/testwindow.cpp
)

set(client-lib_INC
    src/client/clientlib.h
)

set(client-lib_SRC
    src/client/clientlib.cpp
)

set(client-gui_INC
    src/client/clientgui.h
)

set(client-gui_SRC
    src/client/maingui.cpp
    src/client/clientgui.cpp
)

set(server-lib_INC
    src/server/serverlib.h
)

set(server-lib_SRC
    src/server/serverlib.cpp
)

set(server-cli_INC
    src/server/servercli.h
)

set(server-cli_SRC
    src/server/maincli.cpp
    src/server/servercli.cpp
)

set(server-gui_INC
    src/server/servergui.h
)

set(server-gui_SRC
    src/server/maingui.cpp
    src/server/servergui.cpp
)

set(userdata_INCDIR
    src/common/userdata
)

set(userdata_INC
    src/common/userdata/userdata.h
)

set(userdata_SRC
    src/common/userdata/userdata.cpp
)

set(kommunicator_INCDIR
    src/common/kommunicator
)

set(kommunicator_INC
    src/common/kommunicator/kommunicator.h
)

set(kommunicator_SRC
    src/common/kommunicator/kommunicator.cpp
)

set(iconloader_INCDIR
    src/common/icons
)

set(iconloader_INC
    src/common/icons/iconloader.h
)

set(iconloader_SRC
    src/common/icons/iconloader.cpp
)

set(breeze-icons_RCC
    src/common/icons/breeze-icons.rcc
    src/common/icons/breeze-icons-dark.rcc
)

# Export various CMake variables
configure_file(src/common/project_information/project_information.h.in project_information/project_information.h @ONLY)

# Tell CMake to create executables and libraries
add_executable(qtnetworktest ${qtnetworktest_SRC})

add_library(client-lib ${client-lib_INC} ${client-lib_SRC} ${userdata_INC} ${userdata_SRC} ${kommunicator_INC} ${kommunicator_SRC})
add_executable(client-gui WIN32 ${client-gui_INC} ${client-gui_SRC} ${iconloader_INC} ${iconloader_SRC})

add_library(server-lib ${server-lib_INC} ${server-lib_SRC} ${userdata_INC} ${userdata_SRC} ${kommunicator_INC} ${kommunicator_SRC})
add_executable(server-cli ${server-cli_INC} ${server-cli_SRC})
add_executable(server-gui WIN32 ${server-gui_INC} ${server-gui_SRC} ${iconloader_INC} ${iconloader_SRC})

# Add include directories
include_directories(${CMAKE_BINARY_DIR}/project_information)

target_include_directories(client-lib PUBLIC ${userdata_INCDIR} ${kommunicator_INCDIR})
target_include_directories(client-gui PUBLIC ${iconloader_INCDIR})

target_include_directories(server-lib PUBLIC ${userdata_INCDIR} ${kommunicator_INCDIR})
target_include_directories(server-gui PUBLIC ${iconloader_INCDIR})

# Link libraries to their UI executables
# Also use the Widgets module from Qt 5.
target_link_libraries(qtnetworktest Qt5::Widgets)

target_link_libraries(client-lib Qt5::Network)
target_link_libraries(client-gui client-lib Qt5::Widgets)

target_link_libraries(server-lib Qt5::Network)
target_link_libraries(server-cli server-lib Qt5::Core)
target_link_libraries(server-gui server-lib Qt5::Widgets)

# Set output names
set_target_properties(client-lib PROPERTIES OUTPUT_NAME client)
set_target_properties(client-gui PROPERTIES OUTPUT_NAME clientgui)

set_target_properties(server-lib PROPERTIES OUTPUT_NAME server)
set_target_properties(server-gui PROPERTIES OUTPUT_NAME servergui)
set_target_properties(server-cli PROPERTIES OUTPUT_NAME servercli)

# Install the libraries and executables
# New way
install(TARGETS client-lib LIBRARY COMPONENT client-lib)
install(TARGETS client-gui RUNTIME COMPONENT client-gui)

install(TARGETS server-lib LIBRARY COMPONENT server-lib)
install(TARGETS server-cli RUNTIME COMPONENT server-cli)
install(TARGETS server-gui RUNTIME COMPONENT server-gui)

# Old way
#install(TARGETS qtnetworktest DESTINATION bin)

#install(TARGETS client-lib DESTINATION lib)
#install(TARGETS client-gui DESTINATION bin)

#install(TARGETS server-lib DESTINATION lib)
#install(TARGETS server-cli DESTINATION bin)
#install(TARGETS server-gui DESTINATION bin)

# Install icons
install(FILES breeze-icons_RCC DESTINATION usr/share/${CMAKE_PROJECT_NAME} COMPONENT icons)

# Install source files
install(DIRECTORY src/ DESTINATION usr/src/${CMAKE_PROJECT_NAME} COMPONENT sources EXCLUDE_FROM_ALL)

# Windows Qt5 deployment (broken?)
if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
    get_target_property(_qt5_qmake_location Qt5::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt5_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES IMPORTED_LOCATION ${imported_location})
    endif()
endif()

if(TARGET Qt5::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET client-lib
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt/client-lib"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt/client-lib" "$<TARGET_FILE_DIR:client-lib>/$<TARGET_FILE_NAME:client-lib>"
    )

    # copy deployment directory during installation
    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/client-lib" DESTINATION bin COMPONENT client-lib)
    
    # repeat for all targets
    add_custom_command(TARGET client-gui
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt/client-gui"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt/client-gui" "$<TARGET_FILE_DIR:client-gui>/$<TARGET_FILE_NAME:client-gui>"
    )

    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/client-gui" DESTINATION bin COMPONENT client-gui)

    add_custom_command(TARGET server-lib
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt/server-lib"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt/server-lib" "$<TARGET_FILE_DIR:server-lib>/$<TARGET_FILE_NAME:server-lib>"
    )

    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/server-lib" DESTINATION bin COMPONENT server-lib)
    
    add_custom_command(TARGET server-cli
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt/server-cli"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt/server-cli" "$<TARGET_FILE_DIR:server-cli>/$<TARGET_FILE_NAME:server-cli>"
    )

    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/server-cli" DESTINATION bin COMPONENT server-cli)
    
    add_custom_command(TARGET server-gui
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt/server-gui"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt/server-gui" "$<TARGET_FILE_DIR:server-gui>/$<TARGET_FILE_NAME:server-gui>"
    )

    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt/server-gui" DESTINATION bin COMPONENT server-gui)
endif()

# CPack specific settings
include(CPackComponent)
include(InstallRequiredSystemLibraries)
# Add license
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
# Handle different components
cpack_add_install_type(client_install DISPLAY_NAME "Client only")
cpack_add_install_type(server_install_cli DISPLAY_NAME "Server (command line) only")
cpack_add_install_type(server_install_gui DISPLAY_NAME "Server (gui) only")
cpack_add_install_type(server_install DISPLAY_NAME "Server only")
cpack_add_install_type(programs_install DISPLAY_NAME "All programs")
cpack_add_install_type(complete_install DISPLAY_NAME "Complete install")

cpack_add_component_group(client_group DISPLAY_NAME "Client")
cpack_add_component_group(server_group DISPLAY_NAME "Server")
cpack_add_component_group(resources_group DISPLAY_NAME "Resources")
cpack_add_component_group(sources_group DISPLAY_NAME "Program sources")

cpack_add_component(client-lib DISPLAY_NAME "Client libraries" GROUP client_group INSTALL_TYPES complete_install)
cpack_add_component(client-gui DISPLAY_NAME "Client executable" GROUP client_group DEPENDS client-lib INSTALL_TYPES client_install programs_install complete_install)

cpack_add_component(server-lib DISPLAY_NAME "Server libraries" DISABLED GROUP server_group INSTALL_TYPES complete_install)
cpack_add_component(server-cli DISPLAY_NAME "Server executable (command line)" DISABLED GROUP server_group DEPENDS server-lib INSTALL_TYPES server_install server_install_cli programs_install complete_install)
cpack_add_component(server-gui DISPLAY_NAME "Server executable (gui)" DISABLED GROUP server_group DEPENDS server-lib INSTALL_TYPES server_install server_install_gui programs_install complete_install)

# Get executable names
get_target_property(client-gui_EXE client-gui OUTPUT_NAME)
if(NOT ${client-gui_EXE})
    get_target_property(client-gui_EXE client-gui NAME)
endif()

get_target_property(server-cli_EXE server-cli OUTPUT_NAME)
if(NOT ${server-cli_EXE})
    get_target_property(server-cli_EXE server-cli NAME)
endif()

get_target_property(server-gui_EXE server-gui OUTPUT_NAME)
if(NOT ${server-gui_EXE})
    get_target_property(server-gui_EXE server-gui NAME)
endif()
# Set executables
set(CPACK_PACKAGE_EXECUTABLES "${client-gui_EXE};${CMAKE_PROJECT_NAME} client;${server-cli_EXE};${CMAKE_PROJECT_NAME} server (command line);${server-gui_EXE};${CMAKE_PROJECT_NAME} server (gui)")
set(CPACK_CREATE_DESKTOP_LINKS "${client-gui_EXE}")
# NSIS specific settings
# Uninstall older versions
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
# Desktop shortcut
set(CPACK_NSIS_MODIFY_PATH "ON")
# Run program option
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${client-gui_EXE}")
# Contact option
set(CPACK_NSIS_CONTACT "vintilammm@protonmail.com")
# Finally add CPack
include(CPack)
