#ifndef MY_DOS_H
  #define MY_DOS_H

  #define  ff_name      name
  #define  ff_attrib    attrib

/* File attribute constants for attribute field */
  #define  FA_RDONLY    _A_RDONLY
  #define  FA_HIDDEN    _A_HIDDEN
  #define  FA_SYSTEM    _A_SYSTEM
  #define  FA_LABEL     _A_VOLID
  #define  FA_DIREC     _A_SUBDIR
  #define  FA_ARCH      _A_ARCH

  #define  findfirst(FILENAME, F_STRUCT, ATTR)  \
                  _dos_findfirst(FILENAME, ATTR, F_STRUCT)
  #define  findnext(F_STRUCT)   _dos_findnext(F_STRUCT)

  #define  getdisk()            _getdrive()

#endif
