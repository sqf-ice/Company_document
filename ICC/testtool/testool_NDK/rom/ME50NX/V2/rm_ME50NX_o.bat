@echo off
del /Q /A:H ..\..\..\o\ME50NX\V2\*.* 2>NUL
del /Q ..\..\..\o\ME50NX\V2\*.* 2>NUL
del /Q /A:H ..\..\..\o\ME50NX\*.* 2>NUL
del /Q ..\..\..\o\ME50NX\*.* 2>NUL
del main 2>NUL
del *.ro* 2>NUL
del *.NLD 2>NUL
del *.lst 2>NUL
del *.axf 2>NUL
del *.err 2>NUL
del *.elf 2>NUL
echo on
