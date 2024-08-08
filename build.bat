cd D:/TelegramQuickView/out/build

rm -rf D:/TelegramQuickView/out/build

"C:\Windows\system32\cmd.exe" /c "%SYSTEMROOT%\System32\chcp.com 65001 >NUL && "D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"  -G "Ninja"  -DCMAKE_PREFIX_PATH:STRING="D:/Packages/Qt/6.7.0/msvc2019_64" -DCMAKE_BUILD_TYPE:STRING="Release"   -DCMAKE_MAKE_PROGRAM="D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "D:\TelegramQuickView" 2>&1"

start TelegramQuickView.exe
pause