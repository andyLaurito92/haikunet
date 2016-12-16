@echo off
set PATH=%PATH%;%SystemRoot%\System32
set PATH=%PATH%;C:\Program Files\Subversion\bin
set PATH=%PATH%;C:\Program Files\Ethalone\Ghost Installer\Bin\
set PATH=%PATH%;C:\Program Files\GnuWin32\bin
echo "Retrieving latest from SVN"
svn up
svnversion >rev1
tr -d '\n' <rev1 >rev2
tr -d '\r' <rev2 >rev
set /p version= <rev
copy version.major+rev version /Y /b
rmdir /s /q tmp_win_installer 
svn export win_installer tmp_win_installer
svn export atomics tmp_win_installer\powerdevs\atomics
svn export build tmp_win_installer\powerdevs\build 
svn export engine tmp_win_installer\powerdevs\engine 
svn export examples tmp_win_installer\powerdevs\examples
svn export library tmp_win_installer\powerdevs\library
svn export output tmp_win_installer\powerdevs\output
copy version tmp_win_installer\powerdevs
copy doc\PD_UserGuide.pdf tmp_win_installer\powerdevs\doc
copy COPYING tmp_win_installer\powerdevs
echo "Building Binaries";
cd src/uname
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdae
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdif
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdme_v2
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make 
cd ../pdppt
qmake -r -spec win32-g++ "CONFIG+=release"
mingw32-make clean
mingw32-make
copy %QTDIR%\bin\QtCore4.dll ..\..\tmp_win_installer\powerdevs\bin
copy %QTDIR%\bin\QtGui4.dll ..\..\tmp_win_installer\powerdevs\bin
copy %QTDIR%\bin\QtSvg4.dll  ..\..\tmp_win_installer\powerdevs\bin
copy %QTDIR%\bin\mingwm10.dll ..\..\tmp_win_installer\powerdevs\bin
copy %QTDIR%\bin\libgcc_s_dw2-1.dll  ..\..\tmp_win_installer\powerdevs\bin
copy "C:\mingw\bin\libstdc++-6.dll" ..\..\tmp_win_installer\powerdevs\bin
cd ../../tmp_win_installer
gibuild PowerDEVS.gpr
cd ../
copy tmp_win_installer\Output\* .
ren powerdevs_install.exe powerdevs_install_%version%.exe
rem rmdir /s /q tmp_win_installer 
