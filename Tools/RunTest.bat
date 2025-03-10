@echo off
setlocal enabledelayedexpansion
pushd %~dp0

rem ..\Build\vs2017\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=math.*
rem ..\Build\vs2017\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=util.*
rem ..\Build\vs2019\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=util.*
rem ..\Build\vs2022\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=util.*
rem ..\Build\vs2022\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=*
..\Build\vs2022\tofu_test\x64\Debug-v143\tofu_test.exe --iutest_filter=*

:END

popd
exit /b 0