@echo off 
SET DevEnvDir=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\

set version=14.25.28610

SET INCLUDE=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\ATLMFC\include;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\include;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\include\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.17763.0\ucrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.17763.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.17763.0\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.17763.0\winrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.17763.0\cppwinrt
SET LIB=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\ATLMFC\lib\x86;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\lib\x86;C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6.1\lib\um\x86;C:\Program Files (x86)\Windows Kits\10\lib\10.0.17763.0\ucrt\x86;C:\Program Files (x86)\Windows Kits\10\lib\10.0.17763.0\um\x86;
SET LIBPATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\ATLMFC\lib\x86;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\lib\x86;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\lib\x86\store\references;C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.17763.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.17763.0;C:\Windows\Microsoft.NET\Framework\v4.0.30319;
SET LOCALAPPDATA=C:\Users\admin\AppData\Local
SET Path=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\bin\HostX86\x86;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\VC\VCPackages;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\bin\Roslyn;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Team Tools\Performance Tools;C:\Program Files (x86)\Microsoft Visual Studio\Shared\Common\VSPerfCollectionTools\vs2019\;C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.6.1 Tools\;C:\Program Files (x86)\HTML Help Workshop;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\FSharp\;C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\x86;C:\Program Files (x86)\Windows Kits\10\bin\x86;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\\MSBuild\Current\Bin;C:\Windows\Microsoft.NET\Framework\v4.0.30319;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\dotnet\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files\Git\cmd;C:\Program Files\Git\mingw64\bin;C:\Program Files\Git\usr\bin;C:\Strawberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;C:\WINDOWS\System32\OpenSSH\;C:\Program Files (x86)\Prince\engine\bin;C:\ProgramData\chocolatey\bin;C:\Program Files\CMake\bin;C:\Program Files (x86)\GitExtensions\;C:\Program Files (x86)\Calibre2\;C:\texlive\2019\bin\win32;C:\Users\admin\AppData\Local\Microsoft\WindowsApps;C:\Users\admin\AppData\Local\Programs\MiKTeX 2.9\miktex\bin\x64\;C:\Users\admin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\admin\AppData\Local\Pandoc\;%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;C:\Users\admin\.dotnet\tools;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja
SET UniversalCRTSdkDir=C:\Program Files (x86)\Windows Kits\10\
SET VCIDEInstallDir=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\VC\
SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\
SET VCToolsInstallDir=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\%version%\
SET VCToolsRedistDir=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\%version%\
SET VCToolsVersion=%version%
SET VisualStudioVersion=16.0
SET VS160COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\
SET VSCMD_ARG_app_plat=Desktop
SET VSCMD_ARG_HOST_ARCH=x86
SET VSCMD_ARG_TGT_ARCH=x86
SET VSCMD_VER=16.1.3
SET VSINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\
SET windir=C:\WINDOWS
SET WindowsLibPath=C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.17763.0;C:\Program Files (x86)\Windows Kits\10\References\10.0.17763.0
SET WindowsSdkBinPath=C:\Program Files (x86)\Windows Kits\10\bin\
SET WindowsSdkDir=C:\Program Files (x86)\Windows Kits\10\
SET WindowsSDKLibVersion=10.0.17763.0\
SET WindowsSdkVerBinPath=C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\
SET WindowsSDKVersion=10.0.17763.0\
SET WindowsSDK_ExecutablePath_x64=C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.6.1 Tools\x64\
SET WindowsSDK_ExecutablePath_x86=C:\Program Files (x86)\Microsoft SDKs\Windows\v10.0A\bin\NETFX 4.6.1 Tools\
SET __VSCMD_PREINIT_PATH=C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\dotnet\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files\Git\cmd;C:\Program Files\Git\mingw64\bin;C:\Program Files\Git\usr\bin;C:\Strawberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;C:\WINDOWS\System32\OpenSSH\;C:\Program Files (x86)\Prince\engine\bin;C:\ProgramData\chocolatey\bin;C:\Program Files\CMake\bin;C:\Program Files (x86)\GitExtensions\;C:\Program Files (x86)\Calibre2\;C:\texlive\2019\bin\win32;C:\Users\admin\AppData\Local\Microsoft\WindowsApps;C:\Users\admin\AppData\Local\Programs\MiKTeX 2.9\miktex\bin\x64\;C:\Users\admin\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\admin\AppData\Local\Pandoc\;%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;C:\Users\admin\.dotnet\tools

cl.exe  /JMC /permissive- /GS /W3 /Zc:wchar_t^
 /ZI /Gm- /Od /sdl /Zc:inline /fp:precise ^
 /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /D "_CRT_SECURE_NO_WARNINGS" ^
 /errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd /FC /EHsc /nologo   h2c.c 
