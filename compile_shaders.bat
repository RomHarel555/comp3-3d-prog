@echo off
glslangValidator -V color.vert -o color.vert.spv
glslangValidator -V color.frag -o color.frag.spv
glslangValidator -V overlay.vert -o overlay.vert.spv
glslangValidator -V overlay.frag -o overlay.frag.spv

REM Also generate files with underscore naming
copy color.vert.spv color_vert.spv
copy color.frag.spv color_frag.spv

echo "All shaders compiled successfully"
pause 