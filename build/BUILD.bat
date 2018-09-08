@echo off
mkdir ..\build

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -Zi
pushd ..\build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\src\FluffyFortnight.cpp -FmFluffyFortnight.map /LD /link -incremental:no -opt:ref -PDB:FluffyFortnight_%random%.pdb /DLL -EXPORT:renderGame
cl %CommonCompilerFlags% ..\src\win32_FluffyFortnight.cpp -Fmwin32_FluffyFortnight.map user32.lib gdi32.lib
popd