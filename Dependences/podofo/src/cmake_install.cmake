# Install script for directory: C:/Users/iunin/podofo/podofo-src/src

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/podofo" TYPE FILE FILES
    "C:/Users/iunin/podofo/podofo-src/src/podofo-base.h"
    "C:/Users/iunin/podofo/podofo-src/src/podofo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/podofo/base" TYPE FILE FILES
    "C:/Users/iunin/podofo/podofo_config.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/Pdf3rdPtyForwardDecl.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfArray.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfCanvas.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfColor.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfCompilerCompat.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfCompilerCompatPrivate.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfContentsTokenizer.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfData.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfDataType.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfDate.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfDefines.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfDefinesPrivate.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfDictionary.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfEncoding.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfEncodingFactory.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfEncrypt.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfExtension.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfError.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfFileStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfFilter.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfFiltersPrivate.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfImmediateWriter.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfInputDevice.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfInputStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfLocale.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfMemStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfMemoryManagement.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfName.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfObject.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfObjectStreamParserObject.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfOutputDevice.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfOutputStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfParser.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfParserObject.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfRect.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfRefCountedBuffer.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfRefCountedInputDevice.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfReference.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfString.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfTokenizer.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfVariant.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfVecObjects.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfVersion.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfWriter.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfXRef.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfXRefStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/PdfXRefStreamParserObject.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/podofoapi.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/podofo/base/util" TYPE FILE FILES
    "C:/Users/iunin/podofo/podofo-src/src/base/util/PdfMutex.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/util/PdfMutexImpl_noop.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/util/PdfMutexImpl_win32.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/util/PdfMutexImpl_pthread.h"
    "C:/Users/iunin/podofo/podofo-src/src/base/util/PdfMutexWrapper.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/podofo/doc" TYPE FILE FILES
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfAcroForm.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfAction.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfAnnotation.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfCMapEncoding.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfContents.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfDestination.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfDifferenceEncoding.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfDocument.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfElement.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfEncodingObjectFactory.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfExtGState.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfField.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFileSpec.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFont.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontCID.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontCache.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontConfigWrapper.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontFactory.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontFactoryBase14Data.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontMetrics.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontMetricsBase14.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontMetricsFreetype.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontMetricsObject.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontSimple.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontTTFSubset.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontTrueType.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontType1.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontType3.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFontType1Base14.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfFunction.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfHintStream.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfIdentityEncoding.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfImage.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfInfo.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfMemDocument.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfNamesTree.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfOutlines.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfPage.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfPagesTree.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfPagesTreeCache.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfPainter.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfPainterMM.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfShadingPattern.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfSignOutputDevice.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfSignatureField.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfStreamedDocument.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfTable.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfTilingPattern.h"
    "C:/Users/iunin/podofo/podofo-src/src/doc/PdfXObject.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/iunin/podofo/src/Debug/podofo.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/iunin/podofo/src/Release/podofo.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/iunin/podofo/src/MinSizeRel/podofo.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/iunin/podofo/src/RelWithDebInfo/podofo.lib")
  endif()
endif()

