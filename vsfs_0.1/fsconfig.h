#define BSIZE     4096
#define BNUM      64
#define SECSIZE   512
#define SECNUM    ((BNUM*BSIZE)/SECSIZE)
#define MAXFNAME  32
#define NDIRECT   12
#define MAXFSIZE  (NDIRECT*BSIZE)

#define DIR_TYPE  1
#define FILE_TYPE 2

#define FSMAGIC 0x10203040

#define ROOTINO 1