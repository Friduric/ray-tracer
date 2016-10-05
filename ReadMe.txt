A Monte Carlo ray tracer implementation.

---------------------------------------------------
- ... if you fail to build, try the following ... -
---------------------------------------------------
1. IMPORTANT: Use the 32-bit binaries (build using x86!).

2. Linker -> Command Line
	-> Add "/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT"

3. OPTIONAL: Remove warning LNK4099: 
	6.1 Change C/C++ -> Output Files -> Program Database File to:
	-> "$(TargetDir)$(TargetName).pdb"
	6.2 Change Linker -> Debugging -> Generate Program Database File to:
	-> "$(TargetDir)$(TargetName).pdb"

4. Adding /FORCE:MULTIPLE to the Linker command line helps remove errors LNK1169 and LNK2005.