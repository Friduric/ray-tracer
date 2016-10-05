## Introduction
This is a Monte Carlo ray tracer implementation that models intransparent objects using Lambertian reflections.

## A few troubleshooting tips
1. IMPORTANT: Use the 32-bit binaries (build using x86!).

2. OPTIONAL: Linker -> Command Line -> Add "/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT"

3. OPTIONAL: Remove warning LNK4099: 
	3.1 Change C/C++ -> Output Files -> Program Database File to: 
		"$(TargetDir)$(TargetName).pdb"
	3.2 Change Linker -> Debugging -> Generate Program Database File to: 
		"$(TargetDir)$(TargetName).pdb"

4. OPTIONAL: Adding /FORCE:MULTIPLE to the Linker command line helps remove errors LNK1169 and LNK2005.
