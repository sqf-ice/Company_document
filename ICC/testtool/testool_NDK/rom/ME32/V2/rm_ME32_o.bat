@echo off
del /Q /A:H ..\..\..\o\ME32\V2\*.* 2>NUL
del /Q ..\..\..\o\ME32\V2\*.* 2>NUL
del /Q /A:H ..\..\..\o\ME32\*.* 2>NUL
del /Q ..\..\..\o\ME32\*.* 2>NUL
del main 2>NUL
del *.so 2>NUL
del *.a  2>NUL
del *.ro* 2>NUL
del *.NLD 2>NUL
del *.lst 2>NUL
del *.axf 2>NUL
del *.err 2>NUL
del *.elf 2>NUL
echo on
