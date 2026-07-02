cmake_minimum_required(VERSION 3.10)
project(Clical VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(READLINE REQUIRED readline)
pkg_check_modules(NLOHMANN_JSON REQUIRED nlohmann_json)

include_directories(${READLINE_INCLUDE_DIRS} ${NLOHMANN_JSON_INCLUDE_DIRS})
link_directories(${READLINE_LIBRARY_DIRS})

add_library(calendar_lib STATIC
    src/calendar.cpp
    src/command_parser.cpp
    src/storage.cpp
    src/event.cpp
    src/recurring_event.cpp
    src/utils.cpp
)

# Main file
add_executable(clical src/main.cpp)
target_link_libraries(clical calendar_lib ${READLINE_LIBRARIES} ${NLOHMANN_JSON_LIBRARIES})

# Install for system
install(TARGETS clical DESTINATION bin)
install(FILES calendar_data.json DESTINATION share/clcalendar/ OPTIONAL)

include(CPack)

set(CPACK_PACKAGE_NAME "clical")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Command-Line Calendar with Advanced Features")
set(CPACK_PACKAGE_VENDOR "Shahin")
set(CPACK_PACKAGE_CONTACT "amirmohammadkazemi2002@gmail.com")
set(CPACK_PACKAGE_HOMEPAGE "https://github.com/amirohammadkazemi/Command-Line-Calendar")

# Debian/Ubuntu
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libreadline8, nlohmann-json3-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "amirmohammad <amirmohammadkazemi2002@gmail.com>")

# Fedora/RHEL
set(CPACK_RPM_PACKAGE_REQUIRES "readline, nlohmann-json-devel")
set(CPACK_RPM_PACKAGE_SUMMARY "Command-Line Calendar")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")

set(CPACK_GENERATOR "DEB;RPM;TGZ")

include(CPack)