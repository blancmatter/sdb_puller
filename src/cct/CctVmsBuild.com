$!
$! Filename  : CctVmsBuild.com
$!
$! Purpose   : Command-file to build the CCT for the VMS operating system.
$!
$! Author(s) : Andrzej S. Piascik (asp)
$!             Martyn J. Ford     (mjf)
$! 
$! Copyright (c) Telescope Technologies Limited (TTL), 2001
$!
$! Compile stage
$ cc /include=TCSLIBC_DIR cctmain.c, cctinit.c, cctutil.c, cctosinterface.c
$
$! Link stage
$ link /exec=Cct.exe cctmain, cctinit, cctutil, cctosinterface, -
tcslibc_dir:tcslib.olb/lib
$
