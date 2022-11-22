# Install script for directory: C:/Users/iunin/podofo/podofo-src/man

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE FILES
    "C:/Users/iunin/podofo/podofo-src/man/podofobox.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofocolor.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofocountpages.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofocrop.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofogc.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoencrypt.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoimg2pdf.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoimgextract.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoimpose.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoincrementalupdates.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofomerge.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofopages.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofopdfinfo.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofotxt2pdf.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofotxtextract.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofouncompress.1"
    "C:/Users/iunin/podofo/podofo-src/man/podofoxmp.1"
    )
endif()

