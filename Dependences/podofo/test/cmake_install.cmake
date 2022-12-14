# Install script for directory: C:/Users/iunin/podofo/podofo-src/test

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
  include("C:/Users/iunin/podofo/test/ContentParser/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/CreationTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/FilterTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/FormTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/LargeTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/ObjectParserTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/ParserTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/SignatureTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/TokenizerTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/VariantTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/WatermarkTest/cmake_install.cmake")
  include("C:/Users/iunin/podofo/test/unit/cmake_install.cmake")

endif()

