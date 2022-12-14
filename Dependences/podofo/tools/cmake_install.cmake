# Install script for directory: C:/Users/iunin/podofo/podofo-src/tools

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/PoDoFo")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/iunin/podofo/tools/podofobox/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofocolor/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofocountpages/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofocrop/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoencrypt/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofogc/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoimgextract/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoimg2pdf/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofomerge/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofopages/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofopdfinfo/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofotxt2pdf/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofotxtextract/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofouncompress/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoimpose/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoincrementalupdates/cmake_install.cmake")
  include("C:/Users/iunin/podofo/tools/podofoxmp/cmake_install.cmake")

endif()

