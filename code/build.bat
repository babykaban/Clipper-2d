@echo off

set BuildPath=..\

set CommonCompilerFlags2= -fp:except- -fp:fast -nologo -EHsc -O2 -wd4702 -wd4505  
set CommonLinkerFlags2= -opt:ref user32.lib gdi32.lib winmm.lib

set CommonCompilerFlags= -Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4456 -wd4146 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib

IF NOT EXIST %BuildPath%\build mkdir %BuildPath%\build
pushd %BuildPath%\build

REM 64-bit build
cl %CommonCompilerFlags2% ..\code\main.cpp -Fmmain.map /link %CommonLinkerFlags%
REM cl %CommonCompilerFlags% ..\code\generate_polygon_file.cpp -Fmmain.map /link %CommonLinkerFlags%

cl %CommonCompilerFlags2% /std:c++17  ..\code\test_clipper2.cpp /link %CommonLinkerFlags2% 

popd

