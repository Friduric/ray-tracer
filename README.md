## Description
A Monte Carlo ray tracer implementation using photon mapping. 

Currently supports:
- Intransparent materials using Oren-Nayar and Lambertian BRDFs.
- Photon mapping to increase performance and approximate indirect lighting.
- Transparent and reflective materials.
- Shadow, indirect and direct photons.
- Parallelized/multi-threaded rendering using OMP.

TODO: 
- Caustic photons.
- Optimized ray casting using an octree and AABBs.

## Sample images
<p align="center">
An image that was rendered using (only) this renderer:
</p>
<p align="center">
<img src="https://github.com/Friduric/raytracer/blob/master/first_refraction_test.png" width="400" height="400">
</p>

## A few troubleshooting tips
- IMPORTANT: Use the 32-bit binaries (build using x86!). Otherwise GLM might bug out.

- OPTIONAL: Linker -> Command Line -> Add "/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT"

- OPTIONAL: Remove warning LNK4099:

  - Change C/C++ -> Output Files -> Program Database File to: "$(TargetDir)$(TargetName).pdb"

  - Change Linker -> Debugging -> Generate Program Database File to: "$(TargetDir)$(TargetName).pdb"

- OPTIONAL: Adding /FORCE:MULTIPLE to the Linker command line helps remove errors LNK1169 and LNK2005.

## Tags
TNCG15, Linköping University, Global Illumination, Advanced Rendering, Computer-Graphics, TSBK03, TSBK07
