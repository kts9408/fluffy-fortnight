@echo off
set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -Zi
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\src\FluffyFortnight.cpp -FmFluffyFortnight.map /LD /link -incremental:no -opt:ref -PDB:FluffyFortnight-%random%.pdb /DLL -EXPORT:renderGameGfx -EXPORT:initGame -EXPORT:renderGameAudio -EXPORT:updateGame
cl %CommonCompilerFlags% ..\src\win32_FluffyFortnight.cpp -Fmwin32_FluffyFortnight.map /link -PDB:win32_FluffyFortnight.pdb user32.lib gdi32.lib
