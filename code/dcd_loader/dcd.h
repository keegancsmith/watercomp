#pragma once

// #include "dcdplugin.h"
// #include "molfile_plugin.h"

/***************************************************************************
 *cr                                                                       
 *cr            (C) Copyright 1995-2006 The Board of Trustees of the           
 *cr                        University of Illinois                       
 *cr                         All Rights Reserved                        
 *cr                                                                   
 ***************************************************************************/

/***************************************************************************
 * RCS INFORMATION:
 *
 *      $RCSfile: dcdplugin.c,v $
 *      $Author: johns $       $Locker:  $             $State: Exp $
 *      $Revision: 1.71 $       $Date: 2006/06/19 16:38:21 $
 *
 ***************************************************************************
 * DESCRIPTION:
 *   Code for reading and writing CHARMM, NAMD, and X-PLOR format 
 *   molecular dynamic trajectory files.
 *
 * TODO:
 *   Integrate improvements from the NAMD source tree
 *    - NAMD's writer code has better type-correctness for the sizes
 *      of "int".  NAMD uses "int32" explicitly, which is required on
 *      machines like the T3E.  VMD's version of the code doesn't do that
 *      presently.
 *
 *  Try various alternative I/O API options:
 *   - use mmap(), with read-once flags
 *   - use O_DIRECT open mode on new revs of Linux kernel 
 *   - use directio() call on a file descriptor to enable on Solaris
 *   - use aio_open()/read()/write()
 *   - use readv/writev() etc.
 *
 *  Standalone test binary compilation flags:
 *  cc -fast -xarch=v9a -I../../include -DTEST_DCDPLUGIN dcdplugin.c \
 *    -o ~/bin/readdcd -lm
 *
 *  Profiling flags:
 *  cc -xpg -fast -xarch=v9a -g -I../../include -DTEST_DCDPLUGIN dcdplugin.c \
 *    -o ~/bin/readdcd -lm
 *
 ***************************************************************************/

#include "largefiles.h"   /* platform dependent 64-bit file I/O defines */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "endianswap.h"
#include "fastio.h"
#include "molfile_plugin.h"

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661922
#endif

#define RECSCALE32BIT 1
#define RECSCALE64BIT 2
#define RECSCALEMAX   2

// #define TEST_DCDPLUGIN

typedef struct {
  fio_fd fd;
  int natoms;
  int nsets;
  int setsread;
  int istart;
  int nsavc;
  double delta;
  int nfixed;
  float *x, *y, *z;
  int *freeind;
  float *fixedcoords;
  int reverse;
  int charmm;  
  int first;
  int with_unitcell;
} dcdhandle;

/* Define error codes that may be returned by the DCD routines */
#define DCD_SUCCESS      0  /* No problems                     */
#define DCD_EOF         -1  /* Normal EOF                      */
#define DCD_DNE         -2  /* DCD file does not exist         */
#define DCD_OPENFAILED  -3  /* Open of DCD file failed         */
#define DCD_BADREAD     -4  /* read call on DCD file failed    */
#define DCD_BADEOF      -5  /* premature EOF found in DCD file */
#define DCD_BADFORMAT   -6  /* format of DCD file is wrong     */
#define DCD_FILEEXISTS  -7  /* output file already exists      */
#define DCD_BADMALLOC   -8  /* malloc failed                   */
#define DCD_BADWRITE    -9  /* write call on DCD file failed   */

/* Define feature flags for this DCD file */
#define DCD_IS_XPLOR        0x00
#define DCD_IS_CHARMM       0x01
#define DCD_HAS_4DIMS       0x02
#define DCD_HAS_EXTRA_BLOCK 0x04
#define DCD_HAS_64BIT_REC   0x08

/* defines used by write_dcdstep */
#define NFILE_POS 8L
#define NSTEP_POS 20L

/* READ Macro to make porting easier */
#define READ(fd, buf, size)  fio_fread(((void *) buf), (size), 1, (fd))

/* WRITE Macro to make porting easier */
#define WRITE(fd, buf, size) fio_fwrite(((void *) buf), (size), 1, (fd))

/* XXX This is broken - fread never returns -1 */
#define CHECK_FREAD(X, msg) if (X==-1) { return(DCD_BADREAD); }
#define CHECK_FEOF(X, msg)  if (X==0)  { return(DCD_BADEOF); }


/* print DCD error in a human readable way */
void print_dcderror(const char *func, int errcode); 



/*
 * Read the header information from a dcd file.
 * Input: fd - a file struct opened for binary reading.
 * Output: 0 on success, negative error code on failure.
 * Side effects: *natoms set to number of atoms per frame
 *               *nsets set to number of frames in dcd file
 *               *istart set to starting timestep of dcd file
 *               *nsavc set to timesteps between dcd saves
 *               *delta set to value of trajectory timestep
 *               *nfixed set to number of fixed atoms 
 *               *freeind may be set to heap-allocated space
 *               *reverse set to one if reverse-endian, zero if not.
 *               *charmm set to internal code for handling charmm data.
 */
int read_dcdheader(fio_fd fd, int *N, int *NSET, int *ISTART, 
                   int *NSAVC, double *DELTA, int *NAMNF, 
                   int **FREEINDEXES, float **fixedcoords, int *reverseEndian, 
                   int *charmm);
                   

int read_charmm_extrablock(fio_fd fd, int charmm, int reverseEndian,
                                  float *unitcell);
                                  

int read_fixed_atoms(fio_fd fd, int N, int num_free, const int *indexes,
                            int reverseEndian, const float *fixedcoords, 
                            float *freeatoms, float *pos, int charmm);
                            
  
int read_charmm_4dim(fio_fd fd, int charmm, int reverseEndian);

int read_dcdstep(fio_fd fd, int N, float *X, float *Y, float *Z, 
                        float *unitcell, int num_fixed,
                        int first, int *indexes, float *fixedcoords, 
                         int reverseEndian, int charmm);

int skip_dcdstep(fio_fd fd, int natoms, int nfixed, int charmm);



int write_dcdstep(fio_fd fd, int curframe, int curstep, int N, 
                  const float *X, const float *Y, const float *Z, 
                          const double *unitcell, int charmm); 

int write_dcdheader(fio_fd fd, const char *remarks, int N, 
                    int ISTART, int NSAVC, double DELTA, int with_unitcell,
                    int charmm);
                    
void close_dcd_read(int *indexes, float *fixedcoords);
void *open_dcd_read(const char *path, const char *filetype, int *natoms);
int read_next_timestep(void *v, int natoms, molfile_timestep_t *ts);
void close_file_read(void *v);
void *open_dcd_write(const char *path, const char *filetype, int natoms);
int write_timestep(void *v, const molfile_timestep_t *ts);
void close_file_write(void *v);

// VMDPLUGIN_API int VMDPLUGIN_init();
// VMDPLUGIN_API int VMDPLUGIN_register(void *v, vmdplugin_register_cb cb);
// VMDPLUGIN_API int VMDPLUGIN_fini();
