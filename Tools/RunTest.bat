@echo off
setlocal enabledelayedexpansion
pushd %~dp0

rem ..\Build\vs2017\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=math.*
rem ..\Build\vs2017\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=util.*
..\Build\vs2019\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=util.*

:END

popd
exit /b 0