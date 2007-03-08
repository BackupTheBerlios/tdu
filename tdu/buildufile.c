/* buildufile -- summarize disk usage in a special format
 * derived work from gnu du/coreutils
 * v 0.1d

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

//TODO: BUG: manchmal verlinkt er unterverz., die zu anderem dir gehoeren

#include <sys/types.h> //?
#include <sys/stat.h>  //?
#include <fts.h>
#include <error.h>     //?
#include <errno.h>     //?
#include <assert.h>    // debug
#define BIT_FLAGS (FTS_PHYSICAL)
#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
//#include "system.h"
#include <stdint.h>
#include "hash.h"
#include <string.h>    // strlen
#include <inttypes.h>  // PRId64
#include <signal.h> // alarm/sigaction

#define UINTMAX_SIZE (sizeof(uintmax_t))
#define INITIAL_TABLE_SIZE 103

#define SAME_INODE(Stat_buf_1, Stat_buf_2) \
  ((Stat_buf_1).st_ino == (Stat_buf_2).st_ino \
     && (Stat_buf_1).st_dev == (Stat_buf_2).st_dev)

#define IS_DIR_TYPE(Type)       \
  ((Type) == FTS_DP             \
   || (Type) == FTS_DNR)

#ifndef S_BLKSIZE
# error "S_BLKSIZE not defined - Search Source for S_BLKSIZE"
  // Contact author to support your architecture
#endif

/* #ifndef _SYS_STAT_H 
# error "_SYS_STAT_H not defined - Search Source for _SYS_STAT_H"
  // `st_blocks' must be a member of `struct stat'
  // the author does only know of gnu glibc which defines _SYS_STAT_H
#endif */

/* Define a class for collecting directory information. */
struct duinfo
{
    /* Size of files in directory.  */
    uintmax_t size;

    /* Latest time stamp found.  If tmax.tv_sec == TYPE_MINIMUM (time_t)
     *      && tmax.tv_nsec < 0, no time stamp has been found.  */
    struct timespec tmax;
};

/* A structure for per-directory level information.  */
struct dulevel
{
  /* Entries in this directory.  */
  uintmax_t ent;

  /* Total for subdirectories.  */
  uintmax_t subdir;

  /* allocated sublist entrys */
  int suballoc;

  /* number of current subdirs */
  int subcount;

  /* pointer to sublist */
  long int *sublist;

  /* pointer to position of this entry in outfile */
  long int mypos;
};

/* Hash structure for inode and device numbers.  The separate entry
   structure makes it easier to rehash "in place".  */
struct entry
{
  ino_t st_ino;
  dev_t st_dev;
};

/* A set of dev/ino pairs.  */
static Hash_table *htab;

static size_t
entry_hash (void const *x, size_t table_size)
{
  struct entry const *p = x;

  /* Ignoring the device number here should be fine.  */
  /* The cast to uintmax_t prevents negative remainders
     if st_ino is negative.  */
  return (uintmax_t) p->st_ino % table_size;
}

/* Compare two dev/ino pairs.  Return true if they are the same.  */
static bool
entry_compare (void const *x, void const *y)
{
  struct entry const *a = x;
  struct entry const *b = y;
  return SAME_INODE (*a, *b) ? true : false;
}

/* Try to insert the INO/DEV pair into the global table, HTAB.
   If the pair is successfully inserted, return zero.
   Upon failed memory allocation exit nonzero.
   If the pair is already in the table, return nonzero.  */
static int
hash_ins (ino_t ino, dev_t dev)
{
  struct entry *ent;
  struct entry *ent_from_table;

  ent = malloc (sizeof *ent);
  ent->st_ino = ino;
  ent->st_dev = dev;

  ent_from_table = hash_insert (htab, ent);
  if (ent_from_table == NULL)
    {
      /* Insertion failed due to lack of memory.  */
      //TODO: ??
      fprintf(stderr,"ERROR: xalloc_die.. ??");
      exit(-2);
      //xalloc_die ();
    }

  if (ent_from_table == ent)
    {
      /* Insertion succeeded.  */
      return 0;
    }

  /* That pair is already in the table, so ENT was not inserted.  Free it.  */
  free (ent);

  return 1;
}

/* Initialize the hash table.  */
static void
hash_init (void)
{
  htab = hash_initialize (INITIAL_TABLE_SIZE, NULL,
                          entry_hash, entry_compare, free);
  if (htab == NULL)
  {
    //TODO: ??
    fprintf(stderr, "ERROR: xalloc_die.. ??\n");
    exit(-2);
    //xalloc_die ();
  }
}

static bool printit = true; //if true, printout status
static int test = 0;
static char *scanroot;
static long int nums = 0; // number of entrys scanned / in file
static long int pointerpos = 0; // position of pointer to first entry in file
static long int endtimepos = 0; // position of end time
static long int numspos = 0; // position of directory quantity
static uintmax_t total = 0;
static unsigned char prefixes[8] = " kMGTPE?";
//static uintmax_t bytepos = 10;

static FILE *outfile = NULL;

static void check_sig (int signr) {
  if (printit) fprintf(stderr, "ERROR: SIGALRM while printit is still true\n");
  printit = true;
}

static void
set_currnums()
{
  long int old_pos;
 
  old_pos = ftell (outfile);
  fseek(outfile,numspos,SEEK_SET);
  fwrite (&nums,(size_t) sizeof nums,1,outfile);
  fseek(outfile,old_pos,SEEK_SET);
}

static void
set_startpointer()
{
  long int old_pos;

  old_pos = ftell (outfile);
  fseek(outfile,pointerpos,SEEK_SET);
  fwrite(&old_pos,(size_t) sizeof(long int),1,outfile);
  fseek(outfile,old_pos,SEEK_SET);
  //printf ("Vorher: %d geschrieben nach: %d nachher: %d\n",
  //        (int) old_pos,(int) pointerpos, (int) ftell (outfile));
}

static void
set_subdir(struct dulevel *dulvl,size_t level,size_t sub_level)
{
  // prev_level is subdir, so write it down
  if (dulvl[level].suballoc < dulvl[level].subcount+1)
  {
    //list_alloc(dulvl[level])
    if (dulvl[level].suballoc==0)
    {
      dulvl[level].suballoc = 10;
      dulvl[level].sublist = malloc (dulvl[level].suballoc * sizeof (dulvl[sub_level].mypos));
      if (dulvl[level].sublist == NULL)
      {
        //TODO: fehlermeldung
        exit (-1);
      }
    }
    else
    {
      dulvl[level].suballoc = 2 * dulvl[level].suballoc;
      dulvl[level].sublist = 
             realloc (dulvl[level].sublist,dulvl[level].suballoc * sizeof (dulvl[sub_level].mypos));
      if (dulvl[level].sublist == NULL)
      {
        //TODO: fehlermeldung
        exit (-1);
      }
    }
  }
  //list_add(dulvl[level].sublist,ftell(outfile));
  //dulvl[level].sublist[dulvl[level].subcount]=ftell(outfile);
  //printf ("DEBUG1\n"); fflush(stdout);
  dulvl[level].sublist[dulvl[level].subcount]=dulvl[sub_level].mypos;//TODO:mypos???
  //printf ("DEBUG2\n"); fflush(stdout);
  //printf ("level:%i subcount:%i newpos:%i\n",(int)level,(int)dulvl[level].subcount,(int)dulvl[sub_level].mypos);
  dulvl[level].subcount++;

  //write_ent(dulvl[prev_level]);
  //printf ("%jd\t%s\n",size_to_print, ent->fts_name);
}

static bool
process_file (FTS *fts, FTSENT *ent)
{
  //struct duinfo dui;
  uintmax_t size; //dui
  uintmax_t size_to_print; //dui_to_print
  uintmax_t hsize; //berechnung der h-readable ausgabe von size_to_print
  unsigned char tempstr[30];  //for storing temporary strings <=20 bytes
  static struct dulevel *dulvl;
  static size_t n_alloc = 0;
  static size_t prev_level = 0;
  size_t level;
  int strsize;

  switch (ent->fts_info)
  {
    case FTS_NS: //no stat
    case FTS_ERR: //error
      return false;
  }
  
  if (ent->fts_info != FTS_D )
  {
    if( ! S_ISDIR(ent->fts_statp->st_mode) && 1 < ent->fts_statp->st_nlink
      && hash_ins (ent->fts_statp->st_ino, ent->fts_statp->st_dev))
      {
        //duinfo_init (&dui);
        size=0;
      }
    else
      {
        //duinfo_set (&dui, (uintmax_t) ST_NBLOCKS (*sb) * ST_NBLOCKSIZE,)
        //size = (uintmax_t) ST_NBLOCKS (*sb) * ST_NBLOCKSIZE;
        //size = (uintmax_t) ent->fts_statp->st_blocks * ent->fts_statp->st_blksize;
        //size = (uintmax_t) ST_NBLOCKS (*ent->fts_statp) * ST_NBLOCKSIZE;
        //TODO: nur fuer systeme mit: S_BLKSIZE und HAVE_STRUCT_STAT_ST_BLOCKS
        //      ^^ siehe auch ifndef oben
        size = (uintmax_t) ent->fts_statp->st_blocks * S_BLKSIZE;
//printf ("DEBUG :%i, %i, %i\n", (int) ST_NBLOCKS (*ent->fts_statp), (int) ST_NBLOCKSIZE,(int) size); fflush(stdout);
//printf ("DEBUG ent->fts_statp->st_blocks, ent->fts_statp->st_blksize:%i, %i\n",(int) ent->fts_statp->st_blocks,(int) ent->fts_statp->st_blksize); fflush(stdout);
        //size = (uintmax_t) 
        //             4096 * ent->fts_statp->st_blocks;
                     //ent->fts_statp->st_blksize * ent->fts_statp->st_blocks;
      }
    level = ent->fts_level;
    size_to_print = size;
    //printf("size: %jd level: %zd vorher: %zd\n", size, level, prev_level);
    if (n_alloc == 0)
      {
        //printf("erste Alloziiert\n");
        n_alloc = level + 10;
        dulvl = calloc (n_alloc,  sizeof *dulvl);
        if (dulvl==NULL)
        {
          exit(-1); // TODO: fehlermeldung
        }
      }
    else
      {
        if (level == prev_level)
        {
          //dulvl[level].subcount=0;
            //nothing
          //if ( IS_DIR_TYPE (ent->fts_info)) // && level!=0)
          //  set_subdir(dulvl,level-1,level);
          dulvl[prev_level].subcount=0;
        }
        else if (level > prev_level)
        {
          //if (prev_level!=0)
          //  set_subdir(dulvl,prev_level-1,prev_level);
          /* Descending the hierarchy.
             Clear the accumulators for *all* levels between prev_level
             and the current one.  The depth may change dramatically,
             e.g., from 1 to 10.  */
          size_t i;

          while (n_alloc <= level)
            {
              //printf("nächste Alloziiert\n");
              //dulvl = realloc (dulvl, level * (2 * sizeof *dulvl));
              n_alloc = n_alloc * 2;
              dulvl = realloc (dulvl, n_alloc * (sizeof *dulvl));
              if (dulvl==NULL)
              {
                exit(-1); // TODO: fehlermeldung
              }
              //initialize new mem to zero
              //bzero (dulvl + ((sizeof *dulvl) * (n_alloc / 2))
              //       ,(sizeof *dulvl) * (n_alloc / 2));
              memset(dulvl + (n_alloc / 2),0,(sizeof *dulvl) * (n_alloc / 2));
            }

          //even if it's zeroed before first use, delete old values
          for (i = prev_level + 1; i <= level; i++)
            {
              dulvl[i].ent=0;
              dulvl[i].subdir=0;
              dulvl[i].subcount=0;
              //dulvl[i].suballoc=0;
            }
          dulvl[prev_level].subcount=0;
        }
        else /* level < prev_level */
        {
          /* Ascending the hierarchy.
             Process a directory only after all entries in that
             directory have been processed.  When the depth decreases,
             propagate sums from the children (prev_level) to the parent.             Here, the current level is always one smaller than the
             previous one.  */
          assert (level == prev_level - 1);
          size_to_print += dulvl[prev_level].ent;
          size_to_print += dulvl[prev_level].subdir;
          dulvl[level].subdir += dulvl[prev_level].ent;
          dulvl[level].subdir += dulvl[prev_level].subdir;
 
          //set_subdir(dulvl,level,prev_level);

        }

      }

    //printf("size: %jd level: %zd stp: %jd\n", size, level, size_to_print);

    total += size;

    if ( IS_DIR_TYPE (ent->fts_info) || level==0) 
    {
      //printf ("ANFANG\n"); fflush(stdout);
      int i;
      nums++;
      //total weggelassen
      //printf ("Level:%d ", level);
      //TODO: print only every other second 
      if (printit || level==0) {
        printf ("pos:%10d ",(int)ftell(outfile));
        //printf ("du:%jd ",size_to_print);
        hsize = size_to_print;
        i=0;
        for (;hsize>1024;i++) {
          hsize = hsize / 1024;
        }
        if (i>7) i=7; //overrunprotect: prefixes[i]
        snprintf(tempstr,20,"%3ju%cB",hsize,prefixes[i]);
        printf ("du: %s ",tempstr);
        hsize = total;
        i=0;
        for (;hsize>1024;i++) {
          hsize = hsize / 1024;
        }
        snprintf(tempstr, 20, "%3jd%cB", hsize, prefixes[i]);
        printf ("total: %s ",tempstr);
        //printf ("total:%jd ",total);
        if (strlen(ent->fts_path) > 33) {
          snprintf(tempstr,30,"%s",ent->fts_path);
          strcpy (tempstr,ent->fts_path+(strlen(ent->fts_path)-30));
          printf ("name:...%s", tempstr);
        } else {
          printf ("name:%s", ent->fts_path);
        }
        /*printf ("subcount:%d ",(int)dulvl[level].subcount);
        if (dulvl[level].subcount>0)
        {
          printf ("sublist:");
          for (i=0;i<dulvl[level].subcount;i++)
          {
            printf ("%d ",(int) *(dulvl[level].sublist+(i)));
          }
        }*/
        printf ("\r");
        if (level==0) printf ("\n");
        fflush (stdout);
        printit = false;
        alarm(1);
      }
      if (level==0)
      {
        set_startpointer();
      }
      set_currnums();

      //write to file
      dulvl[level].mypos = ftell(outfile);
      //write dsize
      fwrite (&size_to_print,(size_t) sizeof(size_to_print),1,outfile);
      strsize = strlen(ent->fts_name);
      //if fts_name (current fs-entry name) is scanroot (argv[1]) name it "."
      //TODO: guenstigeren namen geben: ... root...
      if (level==0 && ! strcmp(scanroot,ent->fts_name)) {
        strsize=4;
        fwrite (&strsize,(size_t) sizeof(strsize),1,outfile);
        //fwrite ("\4\0\0\0",(size_t) sizeof(strsize),1,outfile);
        fwrite ("root",(size_t) sizeof(char),4,outfile);
      } 
      else {
        fwrite (&strsize,(size_t) sizeof(strsize),1,outfile);
        fwrite (ent->fts_name,(size_t) sizeof(char),strsize,outfile);
      }
      //size(subdir pointerlist)
      fwrite (&dulvl[level].subcount,
              (size_t) sizeof(dulvl[level].subcount),1,outfile);
      fwrite (dulvl[level].sublist,
              //(size_t) sizeof(long int),dulvl[level].subcount,outfile);
              //(size_t) sizeof(dulvl[level].mypos),dulvl[level].subcount,outfile);
              (size_t) sizeof(dulvl->sublist),dulvl[level].subcount,outfile);

      //printf ("level:%d\n",level);
      //fflush(stdout);
      //printf ("DEBUG n_alloc:%i\n",n_alloc); fflush(stdout);
      if (level>0)
        set_subdir(dulvl,level-1,level);
      //printf ("DURCH\n");
      //fflush(stdout);
 
    }
    dulvl[level].ent += size;
    prev_level = level;
  }
  return true;
}


main (int argc, char *argv[])
{
  int fehler=0;
  char *dir;
  char* files[2];
  int written;
  time_t jetzt = 0;

  if (argc != 3) 
  {
   fprintf (stderr, "ERROR: wrong parameter count\n");
   fprintf (stderr, "This Programm is part of another one, it's not intended to be used stand-alone.\n");
   exit(-1);
  }
  files[0]=argv[1];
  files[1]=NULL;
  FTS *fts = fts_open (files, BIT_FLAGS, NULL);
  //printf("vorhashinit!\n");
  hash_init();
  //printf("bin drin!\n");
  outfile = fopen (argv[2], "w");
  if (outfile == NULL)
  {
    fprintf (stderr, "ERROR: cannot open file for output\n");
    exit(-1);
  }
  written = fprintf (outfile,"TDUv0.1---");
  if (written != 10)
  {
    if (written < 10)
      fprintf (stderr, "ERROR: cannot write to file\n");
    else
      fprintf (stderr, "ERROR: character more than one byte, contact author\n");
    exit(-1);
  }

  //write nums (zero at this time) and 2 dates 
  //ending time (zero at this time) and starting time (now)
  numspos = ftell(outfile);
  fwrite (&nums,(size_t) sizeof nums,1,outfile);
  endtimepos = ftell(outfile);
  fwrite (&jetzt,(size_t) sizeof jetzt,1,outfile); // zero
  jetzt = time ();
  fwrite (&jetzt,(size_t) sizeof jetzt,1,outfile); // now

  //write length of name and name of starting dir
  scanroot=argv[1];
  written=strlen(argv[1]);
  fwrite (&written,(size_t) sizeof(written),1,outfile);
  fwrite (argv[1],(size_t) sizeof(char),strlen(argv[1]),outfile);
  //bytepos += (uintmax_t) (sizeof written);
  //bytepos += strlen(argv[1]);
  //printf ("bytepos: %d\n",bytepos);

  //write nullpointer (zeroed) for first entry and save its location
  fwrite (&pointerpos,(size_t) (sizeof pointerpos),1,outfile);
  pointerpos = ftell (outfile) - (sizeof pointerpos);

  //alarm will be used to print status once per second
  struct sigaction act;
  act.sa_handler=check_sig;
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_RESTART;
  if (sigaction(SIGALRM, &act , NULL) < 0) {
    fprintf (stderr, "ERROR: signal/alarm couldnt be set, contact author.\n");
    exit(-1);
  }
  alarm(1);
    
  while (1)
  {
    FTSENT *ent;
    ent = fts_read(fts);
    //printf("fts gelesen!\n");
    if (ent == NULL)
      {
        //printf("fts==NULL!\n");
        if (errno != 0)
	  {
	    error (0, errno, "fts_read_failed");
            fehler = 1;
	  }
	break;
      }
    //fprintf(stderr, "Eine Schleife: %s \n", ent->fts_path); //debug
    process_file(fts,ent);
  }
//  fclose (outfile);
  fts_close (fts);
//  outfile = fopen (argv[2], "w");
  jetzt = time ();
  fseek(outfile,endtimepos,SEEK_SET);
  fwrite (&jetzt,(size_t) sizeof jetzt,1,outfile); // now
  //TODO: why does this fclose SIGSEG segfault?
//  fclose (outfile);
  exit(fehler);
}

