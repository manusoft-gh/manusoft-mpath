# manusoft-mpath
Windows/MFC file and folder path class

This is a simple Windows file path utility class that uses a CString
data member to manage the wrapped path.

To use the class, add the MPath.cpp and MPath.h files to your project
and include the header file:

#include "MPath.h"

For extended capabilities, define MPATH_EXTENDED:
#define MPATH_EXTENDED
