@echo off
setlocal
cd /d "%~dp0\.."

if not exist build_tests mkdir build_tests

echo Building component tests...
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror ^
    tests\test_components.c ^
    controllers\motion_controller.c controllers\smoke_controller.c controllers\temperature_controller.c ^
    devices\ac.c devices\light.c devices\motion_detector.c devices\smoke_detector.c devices\sprinkler.c devices\temperature_sensor.c ^
    drivers\gpio.c drivers\motion_detector_driver.c drivers\smoke_detector_driver.c drivers\temperature_driver.c ^
    -o build_tests\test_components.exe
if errorlevel 1 exit /b 1

build_tests\test_components.exe
if errorlevel 1 exit /b 1

echo.
echo Building integration tests...
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror ^
    tests\test_integration.c tests\test_time.c ^
    house\house.c house\room.c ^
    controllers\motion_controller.c controllers\smoke_controller.c controllers\temperature_controller.c ^
    devices\ac.c devices\light.c devices\motion_detector.c devices\smoke_detector.c devices\sprinkler.c devices\temperature_sensor.c ^
    drivers\gpio.c drivers\motion_detector_driver.c drivers\smoke_detector_driver.c drivers\temperature_driver.c ^
    -o build_tests\test_integration.exe
if errorlevel 1 exit /b 1

build_tests\test_integration.exe
if errorlevel 1 exit /b 1

echo.
echo All tests passed.
endlocal
