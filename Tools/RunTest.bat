@echo off
setlocal enabledelayedexpansion
pushd %~dp0

call ..\Build\vs2017\tofu_test\x64\Debug\tofu_test.exe

:END

popd
exit /b 0