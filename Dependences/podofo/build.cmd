del cmakecache.txt
set FTDIR=C:\freetype-2.12.1
set FTLIBDIR=C:\freetype-2.12.1\objs\Win32
set JPEGDIR=C:\jpeg-9e
set ZLIBDIR=C:\Program Files (x86)\GnuWin32
cmake -G "Visual Studio 16 2019" podofo-src -DCMAKE_C_COMPILER=C:/MinGW/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/MinGW/bin/g++.exe -DCMAKE_INCLUDE_PATH="%FTDIR%\include;%JPEGDIR%\include;%JPEGDIR%;%ZLIBDIR%\include" -DCMAKE_LIBRARY_PATH="%FTLIBDIR%;%FTDIR%\lib;%JPEGDIR%\lib;%JPEGDIR%;%ZLIBDIR%\lib" -DPODOFO_BUILD_SHARED:BOOL=FALSE -DFREETYPE_LIBRARY_NAMES_DEBUG=freetype239MT_D -DFREETYPE_LIBRARY_NAMES_RELEASE=freetype239MT