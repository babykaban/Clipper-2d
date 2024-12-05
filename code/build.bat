@echo off

set CommonCompilerFlags= -Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4456 -wd4146 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
cl %CommonCompilerFlags% ..\code\main.cpp -Fmmain.map /link %CommonLinkerFlags%
cl %CommonCompilerFlags% ..\code\generate_polygon_file.cpp -Fmmain.map /link %CommonLinkerFlags%
