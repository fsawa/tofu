@echo off
setlocal enabledelayedexpansion
pushd %~dp0

if "%1" == "" (
	..\Build\vs2022\tofu_test\x64\Debug-v143\tofu_test.exe --iutest_filter=*
) else (
	..\Build\vs2022\tofu_test\x64\Debug-v143\tofu_test.exe --iutest_filter=%1
)

echo Done Debug-v143

:END

popd
exit /b 0