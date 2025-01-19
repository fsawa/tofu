@echo off
setlocal enabledelayedexpansion
pushd %~dp0

cd ..
set "PROGRAM_ROOT=%CD%"
echo PROGRAM_ROOT=%PROGRAM_ROOT%

start Build\vs2022\tofu.sln

:END

popd
exit /b 0
