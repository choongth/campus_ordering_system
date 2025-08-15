@echo off
echo Compiling Campus Management System...
taskkill /f /im campus.exe 2>nul
del campus.exe 2>nul
gcc -o campus.exe main.c login.c registration.c share.c student.c restaurant.c delivery_personnel.c admin.c database.c safe_input.c safe_loop.c data_parser.c input_validator.c -I.
if %errorlevel% == 0 (
    echo Compilation successful!
    echo Running the system...
    campus.exe
) else (
    echo Compilation failed!
    pause
)