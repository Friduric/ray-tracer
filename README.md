## Description
A Monte Carlo ray tracer implementation using photon mapping. 

Currently supports:
- Intransparent materials using Oren-Nayar and Lambertian BRDFs.
- Photon mapping to increase performance and approximate indirect lighting.
- Transparent and reflective materials.

TODO: 
- Shadow photons.
- Caustics by photon mapping.
- kd-tree instead of octree.

## Sample images
<p align="center">
This is the third image that was rendered by this ray tracer after about 7 days of coding:
</p>
<p align="center">
<img src="https://github.com/Friduric/raytracer/blob/master/first_refraction_test.png" width="400" height="400">
</p>
<p align="center">
This is the second image that was rendered by this ray tracer after about 4 days of coding:
</p>
<p align="center">
<img src="https://github.com/Friduric/raytracer/blob/master/second_lambertian_test.png" width="400" height="400">
</p>
<p align="center">
This is the first image that was rendered by this ray tracer after about 2 days of coding: 
</p>
<p align="center">
<img src="https://github.com/Friduric/raytracer/blob/master/first_lambertian_test.png" width="400" height="400">
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
