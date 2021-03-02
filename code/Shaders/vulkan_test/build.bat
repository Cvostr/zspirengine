glslc.exe 3d/vs.vert -o 3d/vert.spv
glslc.exe 3d/fs.frag -o 3d/frag.spv

glslc.exe deffered/vs.vert -o deffered/vert.spv
glslc.exe deffered/fs.frag -o deffered/frag.spv

glslc.exe shadowmap/vs.vert -o shadowmap/vert.spv
glslc.exe shadowmap/fs.frag -o shadowmap/frag.spv
glslc.exe shadowmap/gs.geom -o shadowmap/geom.spv

glslc.exe final/vs.vert -o final/vert.spv
glslc.exe final/fs.frag -o final/frag.spv

@pause