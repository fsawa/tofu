@echo off
setlocal enabledelayedexpansion
pushd %~dp0

..\Build\vs2022\tofu_test\x64\Debug\tofu_test.exe --iutest_filter=*

echo Done Debug-clang

:END

popd
exit /b 0