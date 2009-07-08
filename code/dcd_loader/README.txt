I took the code from one of the vmd's plugin source code.
Basically follow the ifdef'd out main method at the 
bottom of the dcdplugin.h file. When using the code, you
only need to include dcdplugin.h. Make sure you include
it before any system files are included as it does stuff 
to allow for fast IO.