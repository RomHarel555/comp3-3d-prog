@echo off
glslangValidator -V color.vert -o color.vert.spv
glslangValidator -V color.frag -o color.frag.spv
glslangValidator -V overlay.vert -o overlay.vert.spv
glslangValidator -V overlay.frag -o overlay.frag.spv
pause 