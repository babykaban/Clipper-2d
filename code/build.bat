@echo off

set BuildPath=..\

set CommonCompilerFlagsO= -fp:except- -fp:fast -nologo -EHsc -Od -wd4702 -wd4505  
set CommonLinkerFlagsO= -opt:ref user32.lib gdi32.lib winmm.lib

set CommonCompilerFlagsD= -EHsc -Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4456 -wd4146 -FC -Z7
set CommonLinkerFlagsD= -incremental:no -opt:ref user32.lib

IF NOT EXIST %BuildPath%\build mkdir %BuildPath%\build
pushd %BuildPath%\build

REM 64-bit build

REM RENEWED CLIPPER
cl %CommonCompilerFlagsO% ..\code\main.cpp -Fmmain.map /link %CommonLinkerFlags%
REM ORIGINAL CLIPPER
cl %CommonCompilerFlagsO% /std:c++17  ..\code\test_clipper2.cpp -Fmtest_clipper2.map /link %CommonLinkerFlags2% 

cl %CommonCompilerFlagsD% ..\code\test_arena.cpp /link %CommonLinkerFlags%

cl %CommonCompilerFlagsO% ..\code\generate_polygon_file.cpp -Fmmain.map /link %CommonLinkerFlags%

popd

