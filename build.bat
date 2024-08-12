cd D:/TelegramQuickView/out/build

"C:\Windows\system32\cmd.exe" /c "%SYSTEMROOT%\System32\chcp.com 65001 >NUL && "D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"  -G "Ninja"  -DCMAKE_C_COMPILER:STRING="D:/VS/2022/VC/Tools/MSVC/14.40.33807/bin/Hostx64/x64/cl.exe" -DCMAKE_CXX_COMPILER:STRING="D:/VS/2022/VC/Tools/MSVC/14.40.33807/bin/Hostx64/x64/cl.exe" -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="D:/TelegramQuickView/out/install/x64-release"   -DCMAKE_MAKE_PROGRAM="D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "D:\TelegramQuickView" 2>&1"

pause