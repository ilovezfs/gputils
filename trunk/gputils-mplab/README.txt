This is a mtc/ini interface for MPLAB.  It allows gputils to be selected as 
a toolset in MPLAB.  This is a preliminary release.

Please report bugs to Craig Franklin <craigfranklin@users.sourceforge.net>.

You must use MPLAB version 6.62 or later.

KNOWN BUGS

1.  The path to the executables must be set.  Even though you specify the full
path to the tools and it prints the full path when the project is built, the
full path isn't used.

2.  All source files that are compiled or assembled, but not included, must
be located in the project directory.  Again, this is strange, because every
where else in the IDE the full path is known.  It is not used in the mtc/ini
interface.

3.  This interface should set the default tool locations, but it doesn't.  This
will be fixed in the next version.
