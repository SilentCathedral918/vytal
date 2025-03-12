@echo off

%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/textured.vert -o graphics_pipelines/textured.vert.spv
%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/textured.frag -o graphics_pipelines/textured.frag.spv

%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/solid.vert -o graphics_pipelines/solid.vert.spv
%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/solid.frag -o graphics_pipelines/solid.frag.spv

%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/transparent.vert -o graphics_pipelines/transparent.vert.spv
%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/transparent.frag -o graphics_pipelines/transparent.frag.spv

%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/wireframe.vert -o graphics_pipelines/wireframe.vert.spv
%VYTAL_EXTERNAL_VULKAN%/Bin/glslc.exe graphics_pipelines/wireframe.frag -o graphics_pipelines/wireframe.frag.spv

echo Shaders compilation completed for all graphics pipelines.  

pause