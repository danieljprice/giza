/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the GPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef struct _tick_ntick {
    const double   ticks;
    const int      nsubs;
} tick_ntick;

typedef struct _tmstamp {
    char   asign;
    int    dd, hh, mm;
    double ss, tfrac;
} tmstamp;

/* located in cpgplot.c */
extern int        units_giza(int pgplotunits);


/* Local support routines */
void              pgtbx1(char axis, int* doday, int dopara, double tmin, double tmax, double* tick, int* nsub, double* tscale );
tick_ntick const* pgtbx2(double tock, tick_ntick const* ticks, double* tick, int* nsub);
void              pgtbx3(int doday, int npl, double tscale, double tints, int nticmx, tick_ntick const** tickptrptr, tick_ntick const* ticks,
                         char axis, int dopara, char const* str, double* tick, int* nsub );
void              pgtbx4(int doday, char const* suptyp, char axis, int convtl, int first, double tmin, double tmax, int tscale,
                         double tick, int do2, int dopara, int mod24);
void              pgtbx5(int doday, double tsec, tmstamp* ts);
void              pgtbx6(int doday, int mod24, int tscale, tmstamp const* ts, int ival[3], double* rval, int writ[4]);
void              pgtbx7(char const* suptyp, char signf, char asign, int ival[3], double rval, int writ[4], int sprec, int do2, char* text, int* tlen, int* last);

#define MAXf(a, b) (a>b ? a : b)

int pgnpl(int nmax, int n) {
    int npl;

    if( n==0 )
        npl = 1;
    else
        npl = ((int)log10(abs(n))) + 1;
    if( n<0 )
        npl++;
    if( nmax>0 && npl>nmax )
        _giza_error("pgnpl", "output conversion error likely; number too big for format");
    return npl;
}

/**
 * Drawing: giza_box_time
 *
 * Synopsis: Same as giza_box, but labels axes with time-style labels
 *           i.e. DD HH MM SS as used in RA - DEC plots for Astronomy.
 *           If this option is used then giza_set_window should have been
 *           called with min/max given in seconds of time.
 *
 * Input:
 *  -xopt  :- As for giza_box, plus additional options below
 *  -xtick :- The distance, in world coordinates,
 *            between major ticks on the x-axis.
 *            If 0.0 the interval is chosen
 *            by giza_box_time.
 *  -nxsub :- The number of minor ticks to be placed
 *            between each major tick. If 0 the
 *            number is chosen by giza_box.
 *  -yopt  :- Similar to xdraw_minticks but for the y-axis.
 *  -ytick :- Similar to xtick but for the y-axis.
 *  -nysub :- Similar to nxsub but for the y-axis.
 *
 * Options for xopt and yopt (same as for giza_box, plus those below):
 *  -Z :- for time-style labelling: (DD) HH MM SS.S
 *  -Y :- exclude the day field so labels are just HH MM SS.S (hours can be >24)
 *  -X :- keep HH field in range (0..24)
 *  -H :- use d,h,m,s superscripts to label numbers
 *  -D :- use degree o, minute ' and second '' superscripts to label numbers
 *  -F :- do not draw the first label (left- or bottom-most)
 *  -O :- omit leading zeros in numbers < 10, i.e. 3 instead of 03
 *
 * See Also: giza_box, giza_box_float, giza_box_time_float
 */
    void
     giza_box_time (const char *xopt, double xtick, int nxsub,
             const char *yopt, double ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_box_time"))
      return;

  /* follow PGPLOT approach: check if any time labelling is required at all
   * then do something useful, otherwise identical to PGBOX. */
  /* LOGICAL XTIME, YTIME, FIRST, DODAYX, DODAYY, DO2, DOPARA, MOD24*/
  int       xtime = 0, ytime = 0, dodayx = 0, dodayy = 0, mod24 = 0, dopara = 0, first = 0, do2 = 0;
  int       nxsubd = nxsub, nysubd = nysub;
  char*     suptyp = NULL;
  char*     cptr;
  double    xtickd=xtick, ytickd=ytick, tscalx, tscaly;
  /* Need to make a copy of the x/y options for we may need to modify them */
  char*     xopt_cp  = strdup(xopt);
  char*     yopt_cp  = (xopt_cp == NULL ? NULL : strdup(yopt));

  if( xopt_cp==NULL || yopt_cp==NULL ) {
      if( xopt_cp!=NULL )
          free(xopt_cp);
      if( yopt_cp!=NULL )
          free(yopt_cp);
      _giza_error("giza_box_time", "out of memory/failure to strdup() x or y opt");
      return;
  }
  /* Now that we have a copy of the strings we can easily upcase them -
     makes parsing easier */
  for(cptr = xopt_cp; *cptr; cptr++ )
      *cptr = toupper(*cptr);
  for(cptr = yopt_cp; *cptr; cptr++ )
      *cptr = toupper(*cptr);

  /* X-axis */
  if( strchr(xopt_cp, 'Z') ) {
      /* possible request time axis - check if OK, not < 1ms */
      if( fabs(Dev[id].Win.xmax - Dev[id].Win.xmin)<0.001 )
          _giza_error("giza_box_time", "X-axis time interval too small (< 1 ms) for time labels");
      else {
          xtime  = 1;
          dodayx = 1;
          if( strchr(xopt_cp, 'Y') || strchr(xopt_cp, 'D') )
              dodayx = 0;
          dopara = 1;
          pgtbx1('X', &dodayx, dopara, Dev[id].Win.xmin, Dev[id].Win.xmax, &xtickd, &nxsubd, &tscalx);
      }
  }

  /* Repeat for Y-axis */
  if( strchr(yopt_cp, 'Z') ) {
      /* possible request time axis - check if OK, not < 1ms */
      if( fabs(Dev[id].Win.ymax - Dev[id].Win.ymin)<0.001 )
          _giza_error("giza_box_time", "Y-axis time interval too small (< 1 ms) for time labels");
      else {
          ytime  = 1;
          dodayy = 1;
          if( strchr(yopt_cp, 'Y') || strchr(yopt_cp, 'D') )
              dodayy = 0;
          dopara = 1;
          if( strchr(yopt_cp, 'V') )
              dopara = 0;
          pgtbx1('Y', &dodayy, dopara, Dev[id].Win.ymin, Dev[id].Win.ymax, &ytickd, &nysubd, &tscaly);
      }
  }
  /*
     C  Parse options list.  For call to PGBOX when doing time labelling, we 
     C  don't want L (log), N or M (write numeric labels). 
     */
  char             x_repl[3] = {0,}, y_repl[3] = {0,}; /* remember replacements */
  char*            p;
  char*            d;
  char const*      r;
  char const*const repl = "LNM";
  if( xtime ) {
      for(r = repl, d=x_repl; *r; r++ )
          if( (p=strchr(xopt_cp, *r))!=NULL )
              *d++ = *p, *p = ' ';
  }
  if( ytime ) {
      for(r = repl, d=y_repl; *r; r++ )
          if( (p=strchr(yopt_cp, *r))!=NULL )
              *d++ = *p, *p = ' ';
  }

  /* Now we let the orginal box() fn draw w/o labels */
  giza_box(xopt_cp, xtickd, nxsubd, yopt_cp, ytickd, nysubd);

  /*
     C  Add (DD) HH MM SS labels if desired.  Go back to the original user
     C  specified options list.
     */
  /* put back what we replaced */
  for( p=x_repl, d=strchr(xopt_cp, ' '); *p; p++, d=strchr(d, ' ') )
      *d = *p; 
  for( p=y_repl, d=strchr(yopt_cp, ' '); *p; p++, d=strchr(d, ' ') )
      *d = *p; 

  /* Handle x-axis */
  if( xtime && (strchr(xopt_cp, 'N') || strchr(xopt_cp, 'M')) ) {
      first = 1;
      if( strchr(xopt_cp, 'F') )
          first = 0;
      suptyp = "NONE";
      if( strchr(xopt_cp, 'D') )
          suptyp = "DMS";
      if( strchr(xopt_cp, 'H') )
          suptyp = "DHMS";

      do2 = 1;
      if( strchr(xopt_cp, 'O') )
          do2 = 0;

      dopara = 1;
      mod24  = 0;
      if( strchr(xopt_cp, 'X') )
          mod24 = 1;

      if( strchr(xopt_cp, 'N') )
          pgtbx4(dodayx, suptyp, 'X', 1, first, Dev[id].Win.xmin, Dev[id].Win.xmax, tscalx, xtickd, do2, dopara, mod24);
      if( strchr(xopt_cp, 'M') )
          pgtbx4(dodayx, suptyp, 'X', 0, first, Dev[id].Win.xmin, Dev[id].Win.xmax, tscalx, xtickd, do2, dopara, mod24);

  }
  /* Repeat for y-axis */
  if( ytime && (strchr(yopt_cp, 'N') || strchr(yopt_cp, 'M')) ) {
      first = 1;
      if( strchr(yopt_cp, 'F') )
          first = 0;
      suptyp = "NONE";
      if( strchr(yopt_cp, 'D') )
          suptyp = "DMS";
      if( strchr(yopt_cp, 'H') )
          suptyp = "DHMS";

      dopara = 1;
      if( strchr(yopt_cp, 'V') )
          dopara = 0;

      do2 = 1;
      if( dopara && strchr(yopt_cp, 'O') )
          do2 = 0;

      mod24  = 0;
      if( strchr(xopt_cp, 'X') )
          mod24 = 1;

      if( strchr(xopt_cp, 'N') )
          pgtbx4(dodayy, suptyp, 'Y', 1, first, Dev[id].Win.ymin, Dev[id].Win.ymax, tscaly, ytickd, do2, dopara, mod24);
      if( strchr(xopt_cp, 'M') )
          pgtbx4(dodayy, suptyp, 'Y', 0, first, Dev[id].Win.ymin, Dev[id].Win.ymax, tscaly, ytickd, do2, dopara, mod24);
  }
  free(xopt_cp);
  free(yopt_cp);
}

/**
 * Drawing: giza_box_time_float
 *
 * Synopsis: Same functionality as giza_box_time but takes floats instead of doubles.
 *
 * See Also: giza_box_time, giza_box
 */
    void
giza_box_time_float (const char *xopt, float xtick, int nxsub,
        const char *yopt, float ytick, int nysub)
{
    if (!_giza_check_device_ready ("giza_box_time_float"))
        return;

    giza_box_time (xopt, (double) xtick, nxsub, yopt, (double) ytick, nysub);
}

/***************************************************************************************************
 *
 *
 *                   Below follow local support routines for PGTBOX(...) 
 *
 *                     They should not be called by the user directly.
 *
 *
 ***************************************************************************************************/

/*
C Work out what the finest units the time labels will be in and
C return the tick increments if the user does not set them.
C
C This is a support routine for PGTBOX and should not 
C be called by the user.
C
C Input:
C  AXIS   :  'X' or 'Y' for use in determining if labels overwrite
C  TMIN   :  Start time in seconds 
C  TMAX   :  End   time in seconds
C  DOPARA :  True if label to be parallel to axis, else perpendicular
C Input/output:
C  DODAY  :  Write labels as DD HH MM SS.S else HH MM SS.S with
C            hours ranging above 24.  Useful for declination labels
C  TICK   :  Major tick interval in seconds.  If 0.0 on input, will 
C            be set here.
C  NSUB   :  Number of minor ticks between major ticks. If 0 on input
C            will be set here.
C Outputs:
C  TSCALE :  Determines finest unit of labelling 
C            (1 => ss, 60 => mm, 3600 => hh, 3600*24 => dd)
C 05-Sep-1988 - new routine (Neil Killeen)
C 08-Apr-1991 - correctly work out HH MM SS when the time > 60 h [nebk]
C 20-Apr-1991 - revise to add support for new DD (day) field and
C               do lots of work on tick algorithm [nebk]
C 10-Jun-1993 - deal with user given ticks & rename from PGTIME [nebk/jm]
*/
void pgtbx1( char axis, int* doday, int dopara, double tmin, double tmax, double* tick, int* nsub, double* tscale ) {
    /* Time scales + subdivisions that (1) look good and (2) feel natural to the user */
    /* A: when time scale is seconds */
    static const tick_ntick ticks1[] = { {0.001, 4}, {0.002, 4}, {0.005, 2},
                                         {0.01,  4},  {0.02, 4},  {0.05, 2},
                                         {0.1,   4},   {0.2, 4},   {0.5, 2},
                                         {1,     4},     {2, 4},     {3, 3},
                                         {4,     4},     {5, 5},     {6, 3},
                                         {10,    2},    {15, 3},    {20, 2},
                                         {30,    3},
                                         /* leave this one as last (sentinel) */
                                         {0,0} };
    /* B: when time scale is minutes */
    static const tick_ntick ticks2[] = { {1, 4}, {2, 4}, {3, 3}, {4, 4}, {5, 5},
                                         {6, 3}, {10,2}, {15,3}, {20,2}, {30,3},
                                         /* leave this one as last (sentinel) */
                                         {0,0} };
    /* C: when time scale is hours */
    static const tick_ntick ticks3[] = { {1,4},   {2,4},  {3,3},  {4,4},  {6,3}, {12,2},
                                         /* leave this one as last (sentinel) */
                                         {0,0} };
    /* D: time scale is days */
    static const tick_ntick ticks4[] = { {1,4},   {2,4},  {3,3},  {4,4},  {5,5},  {6,3}, {8,4}, {9,3},
                                         /* leave this one as last (sentinel) */
                                         {0,0} };
    const double secday              = 24*3600;

    int               npl, ntick, itick;
    char              str[15] = {0,};
    double            tock, tock2, tint, tints, tmins, tmaxs;
    const int         nticmx = 8;
    tick_ntick const* tickptr = NULL;

    if( fabs(tmin)<secday && fabs(tmax)<secday )
        *doday = 0;

    tint  = fabs(tmax - tmin);
    *tick = fabs(*tick);

    if( !_giza_equal(*tick, 0.) ) {
        if( *tick >= tint ) {
            _giza_error("pgtbx1", "user given tick bigger than time interval; will auto-tick");
            *tick = 0.0;
        } else if( *tick<0.001 ) {
            _giza_error("pgtbx1", "user given tick too small (< 1ms); will auto-tick");
            *tick = 0.0;
        } else {
            if( !_giza_equal(fmod(*tick, 60.), 0) ) {
                *tscale = 1;
            } else if( !_giza_equal(fmod(*tick, 3600.), 0) ) {
                *tscale = 60;
            } else if( !*doday ) {
                *tscale = 3600;
            } else if( !_giza_equal(fmod(*tick, secday), 0) ) {
                *tscale = 3600;
            } else {
                *tscale = secday;
            }
        }
        if( *nsub==0 )
            *nsub = 2;
        return;
    }
    /*
       C  Work out label units depending on time interval if user 
       C  wants auto-ticking
       */
    if( tint<(5*60) )
        *tscale = 1;
    else if( tint<(5*3600) )
        *tscale = 60;
    else {
        if( !*doday )
            *tscale = 3600;
        else {
            if( tint<(5*secday) )
                *tscale = 3600;
            else
                *tscale = secday;
        }
    }
    /*
       C  Divide interval into NTICK major ticks and NSUB minor intervals
       C  The tick choosing algorithm is not very robust, so watch out
       C  if you fiddle anything. 
       */
    tints = tint / *tscale;
    if( _giza_equal(*tscale, 1) ) {
        /*
        C  Time in seconds.  If the time interval is very small, may need to 
        C  label with up to 3 decimal places.  Have less ticks to help prevent
        C  label overwrite. STR is a dummy tick label to assess label 
        C  overwrite potential
        */
        if( dopara ) {
            if( tints<=0.01 ) {
                ntick = 4;
                strcpy(str,"60.423");
            } else if( tints<0.1 ) {
                ntick = 5;
                strcpy(str,"60.42");
            } else if( tints<=1 ) {
                ntick = 6;
                strcpy(str,"60.4");
            } else {
                ntick = 6;
                strcpy(str,"60s");
            }
        } else {
            /* not parallel */
            ntick  = 6;
            str[0] = '\0';
        }
        tock    = tints / ntick;
        tickptr = pgtbx2(tock, ticks1, tick, nsub);
        pgtbx3(*doday, 0, *tscale, tints, nticmx, &tickptr, ticks1, axis, dopara, str, tick, nsub);
    } else if( _giza_equal(*tscale, 60) ) {
        /* time in minutes */
        ntick   = 6;
        tock    = tints / ntick;
        tickptr = pgtbx2(tock, ticks2, tick, nsub);

        if( dopara )
            strcpy(str,"42m");
        else
            str[0] = '\0';
        pgtbx3(*doday, 0, *tscale, tints, nticmx, &tickptr, ticks2, axis, dopara, str, tick, nsub);
    } else {
        if( _giza_equal(*tscale, 3600) && *doday ) {
            /* time in hours with the day field */
            ntick   = 6;
            tock    = tints / ntick;
            tickptr = pgtbx2(tock, ticks3, tick, nsub);

            if( dopara )
                strcpy(str,"42h");
            else
                str[0] = '\0';
            pgtbx3(*doday, 0, *tscale, tints, nticmx, &tickptr, ticks3, axis, dopara, str, tick, nsub);
        } else {
            /*
               C  Time in hours with no day field or time in days. Have less
               C  ticks for big numbers or the parallel labels will overwrite.
               */
           if( dopara ) {
               tmins = fabs(tmin) / *tscale;
               tmaxs = fabs(tmax) / *tscale;
               npl   = pgnpl(-1, (int)rint(MAXf(tints, MAXf(tmins, tmaxs))));

               if( npl<=3 ) 
                   ntick = 6;
               else if( npl==4 )
                   ntick = 5;
               else 
                   ntick = 4;
               strcpy(str,"345678912");
               str[npl-1] = 'd';
               str[npl  ] = '\0';
           } else {
               str[0] = '\0';
               ntick  = 6;
           }
           tock    = tints / ntick;
           npl     = pgnpl(-1, (int)rint(tock));
           tock2   = tock / pow(10, npl-1);
           tickptr = pgtbx2(tock2, ticks4, tick, nsub);
           *tick   = *tick * pow(10, npl-1);
            pgtbx3(*doday, 0, *tscale, tints, nticmx, &tickptr, ticks4, axis, dopara, str, tick, nsub);
        }
    }
    /*
    C  Convert tick to seconds
    */
    *tick = *tick * *tscale;
}

/*
C Find the nearest tick in a list to a given value.
C
C This is a support routine for PGTBOX and should not be called
C by the user.
C
C Input:
C  TOCK   :  Try to find the nearest tick in the list to TOCK
C  NTICKS :  Number of ticks in list
C  TICKS  :  List of ticks
C  NSUBS  :  List of number of minor ticks between ticks to go with TICKS
C Output:
C  TICK   :  The selected tick
C  ITICK  :  The index of the selected tick from the list TICKS
C Input/output
C  NSUB   :  Number of minor ticks between major ticks. If 0 on input
C            will be set here.
C
C 10-Jun-1993 - new routine [nebk]


Harro Verkouter: use a list of C-structs to combine tick values + number of subdivisions 
                 in stead of separate lists.
*/
tick_ntick const* pgtbx2(double tock, tick_ntick const* ticks, double* tick, int* nsub) {
    int               nsubd = *nsub;
    double            dmin = strtod("Inf", NULL);
    tick_ntick const* rv = NULL;

    while( ticks->nsubs ) {
        double const diff = fabs(tock - ticks->ticks);
        if( diff<dmin ) {
            rv    = ticks;
            *tick = rv->ticks;
            if( nsubd==0 )
                *nsub = ticks->nsubs;
            dmin = diff;
        }
        ticks++;
    }
    return rv;
}

/*
C Try to see if label overwrite is going to occur with this tick 
C selection, or if there are going to be more than a reasonable
C number of ticks in the displayed time range.  If so, choose, 
C if available, the next tick (bigger separation) up in the list.
C If the overwrite requires that we would need to go up to the bext
C TSCALE, give up.  They will need to choose a smaller character size
C
C This is a support routine for PGTBOX and should not 
C be called by the user.
C
C Input:
C  DODAY  :  True if day field being used
C  NPL    :  Number of characters needed to format TICK on input
C  TSCALE :  Dictates what the finest units of the labelling are.
C            1 = sec, 60 = min, 3600 = hr, 24*3600 = days
C  TINTS  :  Absolute time interval in units of TSCALE
C  NTICMX :  Max. reasonable number of ticks to allow in the time range
C  NTICKS :  Number of ticks in list of ticks to choose from
C  TICKS  :  List of ticks from which the current tick was chosen
C  NSUBS  :  List of number of minor ticks/major tick to choose NSUB from
C  ITICK  :  Index of chosen tick in list TICKS
C  AXIS   :  'X' or 'Y' axis
C  DOPARA :  Labels parallel or perpendicualr to axis
C  STR    :  A typical formatted string used for checking overwrite
C Input/output:
C  TICK   :  Current major tick interval in units of TSCALE. May be 
C            made larger if possible if overwrite likely.
C  NSUB   :  Number of minor ticks between major ticks. 
C
C 10-Jun-1993 - new routine [nebk]
*/
void pgtbx3( int doday, int npl, double tscale, double tints, int nticmx, tick_ntick const** tickptrptr, tick_ntick const* ticks, char axis, int dopara, char const* str, double* tick, int* nsub ) {
    double       lens, lenx, leny, rel_tick_sz;
    int const    ntick = (int)(tints / *tick);

    /* test for overflow in normalized device coordinates; 
       if the total length of the tick strings becomes a significant
       fraction of the viewport it's time to find a better scale */
    giza_qtextlen(GIZA_UNITS_NORMALIZED, str, &lenx, &leny);

    lens = lenx;
    if( (dopara && axis=='Y') || (!dopara && axis=='X') )
        lens = leny;
    int const overflow = dopara && ((ntick * lens) > 0.85);

    /* No action req'd if neither overflow detected nor the amount of ticks
       larger than what's deemed a useful maximum */
    if( !overflow && ntick<=nticmx )
        return;

    /* OK see if there's a next-better suited tick interval */
    if( _giza_equal(tscale, 1) || _giza_equal(tscale, 60) || (_giza_equal(tscale, 3600) && doday) ) {
        /* C  Time in seconds or minutes, or in hours with a day field */
        /* only if there  IS a next tick_ntick struct and it still fits */
        if( ((*tickptrptr)+1)->nsubs && ((*tickptrptr)+1)->ticks<tints ) {
            /* move the pointer one up */
            *tickptrptr = (*tickptrptr)+1;
            *tick       = (*tickptrptr)->ticks;
            *nsub       = (*tickptrptr)->nsubs;
        }
    } else {
        /*C  Time in hours and no day field or time in days */
        /* Check IF there is a next one */
        if( ((*tickptrptr)+1)->nsubs ) {
            *tickptrptr = (*tickptrptr)+1;
            *tick       = (*tickptrptr)->ticks * pow(10, npl-1);
            *nsub       = (*tickptrptr)->nsubs;
        } else {
            /* Otherwise fall back to smallest scaled up by 10** something */
            if( (ticks->ticks * pow(10, npl)) < tints ) {
                *tickptrptr = ticks;
                *tick       = (*tickptrptr)->ticks * pow(10, npl);
                *nsub       = (*tickptrptr)->nsubs;
            }
        }
    }
}

/*
C Label an axis in (DD) HH MM SS.S style.    This is the main 
C workhorse of the PGTBOX routines.
C
C This is a support subroutine for PGTBOX and should not be 
C called by the user. 
C
C Inputs:
C  DODAY  :  Write labels as DD HH MM SS.S else HH MM SS.S with
C            hours ranging above 24.  Useful for declination labels
C  SUPTYP :  If 'DHMS' then superscript the fields with d, h, m, & s
C            If ' DMS' then superscript the fields with    o, '  & '' 
C              Good for declination plots.  You should obviously not 
C              ask for the day field for this to do anything sensible. 
C            If '    ' then no superscripting is done.
C  AXIS   :  'X' for x-axis, 'Y' for y-axis
C  CONVTL :  If .true., write the labels in the conventional axis 
C            locations (bottom and left for 'X' and 'Y').  Otherwise
C            write them on the top and right axes ('X' and 'Y')
C  FIRST  :  If .false. then omit the first label.
C  TMIN   :  Start time (seconds)
C  TMAX   :  End time (seconds)
C  TSCALE :  Determines finest units of axis
C              1 => ss, 60 => mm, 3600 => hh, 3600*24 => dd
C  TICK   :  Major tick interval in seconds
C  DO2    :  If .true., write labels less than 10 with a leading zero.
C  DOPARA :  Y axis label parallel to axis, else perpendicular
C  MOD24  :  HH field labelled as modulo 24
C
C 05-Sep-1988 - new routine (Neil Killeen)
C 20-Apr-1991 - add support for new DD (day) field [nebk]
C 10-Jun-1993 - complete rewrite & rename from PGTLAB. Fixes user given 
C               ticks bug too [nebk]
C 15-Jan-1995 - Add argument MOD24
*/
void pgtbx4(int doday, char const* suptyp, char axis, int convtl, int first, double tmin, double tmax, int tscale,
            double tick, int do2, int dopara, int mod24) {
    const int    maxtik = 1000;
    const int    t = 1;
    const int    f = 0;
   
    tmstamp      timestamps[maxtik], timel; 
    char         asignl;

    double       time, xlen, ylen, coord, fjust, rval, disp, xlen2, ylen2;
    int          is, sd, nt, izero, ipos, ineg, it, i, j, k, sprec, jst[2], jend[2], tlen, last;
    int          ival[3], ivalo[3], ivalz[3], ivalf[3], ivall[3], npass, inc;
    int          writ[4];
    char         signf, text[80];
    char const*  axloc = NULL;

    /* C  Direction signs*/
    sd = (tmax<tmin) ? -1 : 1;
    is = (tmin<0.0)  ? -1 : 1;

    /* C  Find first tick.  Return if none. */
    nt = tmin /tick;
    if( is*sd == 1 && fabs(tmin)>(fabs(nt)*tick) )
        nt += sd;
    time = nt * tick;

    if( (sd==1  && (time<tmin || time>tmax)) ||
        (sd==-1 && (time>tmin || time<tmax)) )
        return;
    /*
    C  Now step through time range in TICK increments and convert
    C  times in seconds at each tick to  +/- (DD) HH MM SS.S
    */
    it    = 0;
    izero = -1;
    while( 1 ) {
        if( (sd==1  && time>(tmax+1e-5)) ||
            (sd==-1 && time<(tmax-1e-5)) )
            break;
        if( it>=maxtik ) {
            _giza_error("pgtbx4", "storage exhausted -- you have asked for far too many ticks");
            break;
        }
        /*
        C  Convert to (DD) HH MM SS.S and find fraction of window that this
        C  tick falls at
        */
        pgtbx5(doday, time, &timestamps[it]);
        timestamps[it].tfrac = (time - tmin) / (tmax - tmin);

        /* note zero time */
        if( nt==0 )
            izero = it;
        /* go to next time */
        nt   = nt + sd;
        time = nt * tick;
        it   = it + 1;
    }
    sprec = 0;
    if( tscale==1 ) {
        if( tick < 0.01 )
            sprec = 3;
        else if( tick < 0.1 )
            sprec = 2;
        else if( tick < 1.0 )
            sprec = 1;
    }
    /* C  Label special case of first tick.  Prepare fields and label */
    pgtbx6(doday, mod24, tscale, &timestamps[0], ivalf, &rval, writ);
    signf = 'H';
    if( doday )
        signf = 'D';
    pgtbx7(suptyp, signf, timestamps[0].asign, ivalf, rval, writ, sprec, do2, text, &tlen, &last);

    /*
    C   Set label displacements from axes.  This is messy for labels oriented
    C   perpendicularly on the right hand axis as we need to know how long
    C   the longest string we are going to write is before we write any 
    C   labels as they are right justified.
    */
    if( axis=='X' ) {
        if( convtl ) {
            axloc = "B";
            if( strcmp(suptyp, "NONE") )
                disp = 1.4;
            else
                disp = 1.2;
        } else {
            axloc = "T";
            disp  = 0.7;
        }
    } else if( axis=='Y' ) {
        if( convtl ) {
            axloc = dopara ? "L" : "LV";
            disp  = 0.7;
        } else {
            if( dopara ) {
                axloc = "R";
                if( strcmp(suptyp, "NONE") )
                    disp = 1.7;
                else
                    disp = 1.9;
            } else {
                /* C  Work out number of characters in first label */
                char  tmp[128];
                float xl, yl, xch, ych;

                axloc = "RV";
                /* create string for length computation */
                if( timestamps[0].asign!='-' && tmin*tmax<0 )
                    sprintf(tmp, " -%s", text);
                else
                    sprintf(tmp, " %s", text);
                /* Can be either unit below because only interested in
                   ratio. So be sure to call with SAME unit */
                giza_qtextlen_float(GIZA_UNITS_NORMALIZED, tmp, &xl, &yl);
                giza_get_character_size_float(GIZA_UNITS_NORMALIZED, &xch, &ych);
                disp = xl / xch;
            }
        }
    }

    if( first ) {
        float xlen, ylen;

        giza_qtextlen_float(GIZA_UNITS_NORMALIZED, &text[last], &xlen, &ylen);
        fjust = 1.0;
        if( axis=='X' ) {
            coord = timestamps[0].tfrac + xlen/2;
        } else if( axis=='Y' ) {
            coord = timestamps[0].tfrac;
            if( dopara )
                coord += ylen/2;
        }
        giza_annotate_float(axloc, disp, coord, fjust, text);
    }

    if( it==1 )
        return;

    /*  C   Designate which field out of DD or HH will carry the sign, depending
        C   on whether you want the day field or not for the rest of the ticks */
    signf = doday ? 'D' : 'H';
    /*  C  Set up labelling justifications for the rest of the labels */
    if( axis=='X' )
        fjust = 0.5;
    else if( axis=='Y' )
        fjust = dopara ? 0.5 : 1.0;

    /*  C  Note zero crossings; IPOS is the first positive tick and
        C  INEG is the first negative tick on either side of 0 */
    ipos = ineg = -1;

    if( izero>=0 ) {
        /* check previous time stamp, if any */
        j = izero - 1;
        if( j>= 0 ) {
            if( timestamps[j].asign=='-' )
                ineg = j;
            else if( timestamps[j].asign==' ' )
                ipos = j;
        }
        /* check next time stamp, if any */
        j = izero + 1;
        if( j<it ) {
            if( timestamps[j].asign=='-' )
                ineg = j;
            else if( timestamps[j].asign==' ' )
                ipos = j;
        }
    }

    /*  C  Now label special case of zero tick. It carries the sign change
        C  when going from positive to negative time, left to right. */
    if( izero>0 ) {
        pgtbx6(doday, mod24, tscale, &timestamps[izero], ivalz, &rval, writ);
        if( timestamps[izero-1].asign==' ' )
            timestamps[izero].asign = '-';
        pgtbx7(suptyp, signf, timestamps[izero].asign, ivalz, rval, writ, sprec, do2, text, &tlen, &last);
        coord = timestamps[izero].tfrac;
        giza_annotate_float(axloc, disp, coord, fjust, text);
    }
    /* C   We may need an extra "virtual" tick if there is no zero crossing
       C   and SD=-1 & IS=1 or SD=1 & IS=-1.  It is used to work out which
       C   fields to label on the right most tick which is labelled first. */
    if( izero==-1 && sd*is==-1 ) {
        if( (sd==-1 && time<0.0) || (sd==1 && time>0.0) )
            time = 0.0;
        pgtbx5(doday, time, &timel);
        pgtbx6(doday, mod24, tscale, &timel, ivall, &rval, writ);
    }

    /* C  We want to label in the direction(s) away from zero, so we may  need
       C  two passes. Determine the start and end ticks for each required pass. */
    jst[1] = jend[1] = -1; /* sentinels */
    npass  = 1;
    /* Note: 'it' holds the /number of time ticks/ so array indices range from 0 .. it-1 */
    if( izero==-1 ) {
        if( is*sd==1 ) {
            jst[0]  = 0;
            jend[0] = it-1;
        } else {
            jst[0]  = it-1;
            jend[0] = 0;
        }
    } else {
        if( ineg==-1 || ipos==-1 ) {
            jst[0]  = izero;
            jend[0] = it-1;
            if( izero==(it-1) )
                jend[0] = 0;
        } else {
            npass = 2;
            jst[0]  = izero;
            jend[0] = 0;
            jst[1]  = izero;
            jend[1] = it-1;
        }
    }
    /* C  Now label the rest of the ticks.  Always label away from 0 */
    for( i=0; i<npass; i++ ) {
        /* C  Initialize previous tick values.  Use virtual tick if labelling
           C  left to right without a zero (one pass) */
        for(k=0; k<3; k++) {
            ivalo[k] = ivalz[k];
            if( izero==-1 ) {
                ivalo[k] = ivall[k];
                if( jst[i]==0 )
                    ivalo[k] = ivalf[k];
            }
        } /* loop over k=0..2 */
        /* Loop up or down */
        inc = (jend[i] < jst[i]) ? -1 : 1;

        for(j = jst[i]; inc < 0 ? j>=jend[i] : j<=jend[i]; j+=inc) {
            /* C  First and zero tick already labelled */
            if( j==0 || j==izero )
                continue;
            /* C  Prepare fields */
            pgtbx6(doday, mod24, tscale, &timestamps[j], ival, &rval, writ);
            /* C  Don't write unchanging fields */
            for(k=0; k<3; k++)
                if( ival[k]==ivalo[k] )
                    writ[k] = 0;
            /* prepare & write label */
            pgtbx7(suptyp, signf, timestamps[j].asign, ival, rval, writ, sprec, do2, text, &tlen, &last);
            coord = timestamps[j].tfrac;
            giza_annotate_float(axloc, disp, coord, fjust, text);
            /* C  Update old values */
            for(k=0; k<3; k++)
                ivalo[k] = ival[k];
        }
    } /* loop i over 0..npass-1 */
}


/*
C  Convert time in seconds to (DD) HH MM SS.S
C
C Input
C  DODAY  :  Use day field if true, else hours accumulates beyond 24
C  TSEC   :  Time in seconds (signed)
C Output
C  ASIGN  :  Sign, ' ' or '-'
C  D,H,M  :  DD, HH, MM (unsigned)
C  S      :  SS.S       (unsigned)
C
C 10-Jun-1993 - new routine [nebk]
*/
void pgtbx5(int doday, double tsec, tmstamp* ts) {
    int    it;
    double rem, tmp;

    /* remember sign and go to positive seconds */
    ts->asign = (tsec < 0) ? '-' : ' ';
    tsec      = fabs(tsec);
    /* take out the largest unit of time - days - if that's requested */
    if( doday ) {
        /* honour days */
        (void)modf(tsec / (24*3600), &tmp);
        /* tmp now has integer # of days as double */
        ts->dd = (int)tmp;
        /* subtract the days */
        tsec  -= ts->dd * (24*3600);
    } else {
        ts->dd = 0;
    }
    /* Next up: hours */
    (void)modf(tsec / 3600, &tmp);
    ts->hh = (int)tmp;
    tsec  -= ts->hh * 3600;
    /* where there's hours, there's typically also minutes */
    (void)modf(tsec / 60, &tmp);
    ts->mm = (int)tmp;
    tsec  -= ts->mm * 60;
    /* Now only left with seconds + fractional seconds */
    ts->ss = tsec;
}

/*
C   Find out which of the DD HH MM SS.S fields we want to write
C   into the label according to TSCALE and make a round off
C   error check.
C
C  Input:
C    DODAY  :  Use day field if true else hours accrue beyond 24
C    MOD24  :  HH field labelled as modulo 24
C    TSCALE :  Dictates which fields appear in labels
C    DD     :  Day of time  (will be 0 if DODAY=F and HH will compensate)
C    HH     :  Hour of time
C    MM     :  Minute of time
C    SS     :  Second of time
C  Output:
C    IVAL(3):  DD HH MM to write into label
C    RVAL   :  SS.S to write into label
C    WRIT(4):  T or F if DD,HH,MM,SS are to be written into the label
C              or not.  IVAL and RVAL fields are set explicitly to
C              zero if the corresponding WRIT field is false.
C              This really is overkill.
C
C  10-Jun-1993 - New routine [nebk]
C  16-Jan-1995 - Add argument MOD24
*/
void pgtbx6(int doday, int mod24, int tscale, tmstamp const* ts, int ival[3], double* rval, int writ[4]) {
    int wm;

    ival[0] = ts->dd;
    ival[1] = ts->hh;
    ival[2] = ts->mm;
    *rval   = ts->ss;

    if( tscale>1 ) {
        wm = (int) rint( ts->ss/60 );
        ival[2] += wm;
        if( ival[2]==60 ) {
            ival[2]  = 0;
            ival[1] += 1;
            if( doday && ival[1]==24 ) {
                ival[1]  = 0;
                ival[0] += 1;
            }
        }
    }
    if( mod24 ) 
        ival[1] %= 24;

    if( tscale==1 ) {
        /* C  Label contains (DD) HH MM */
        writ[0] = doday;
        writ[1] = 1;
        writ[2] = 1;
        writ[3] = 1;
    } else if( tscale==60 ) {
        /* C  Label contains (DD) HH MM */
        writ[0] = doday;
        writ[1] = 1;
        writ[2] = 1;
        *rval   = 0;
        writ[3] = 0;
    } else if( tscale==3600 ) {
        writ[0] = doday;
        writ[1] = 1;
        ival[2] = 0;
        writ[2] = 0;
        *rval   = 0;
        writ[3] = 0;
    } else if( tscale==3600*24 ) {
        writ[0] = 1;
        ival[1] = 0;
        writ[1] = 0;
        ival[2] = 0;
        writ[2] = 0;
        *rval   = 0;
        writ[3] = 0;
    }
}

/*
C Write (DD) HH MM SS.S time labels into a string
C
C This is a support routine for PGTBOX and should not be
C called by the user
C
C Inputs
C  SUPTYP :  '    ', 'DHMS', or ' DMS' for no superscript labelling,
C            d,h,m,s   or   o,','' superscripting
C  SIGNF  :  Tells which field the sign is associated with.  
C            One of 'D', 'H', 'M', or 'S'    
C  ASIGN  :  ' ' or '-' for positive or negative times
C  IVAL(3):  Day, hour, minutes of time
C  RVAL   :  Seconds of time
C  WRIT(4):  If .true. then write DD, HH, MM, SS  into label
C  SPREC  :  Number of places after the decimal to write seconds 
C            string to.  Must be in the range 0-3
C  DO2    :  If true, add a leading zero to numbers < 10
C Outputs
C  TEXT   :  Label
C  TLEN   :  Length of label
C  LAST   :  Is the location of the start character of the last 
C            field written into TEXT
C
C  05-Sep-1989 -- New routine (Neil Killeen)
C  20-Apr-1991 -- Complete rewrite; support for new DD (day) field and 
C                 superscripted labels [nebk]
C  14-May-1991 -- Removed BSL as a parameter (Char(92)) and made it
C                 a variable to appease Cray compiler [mjs/nebk]
C  10-Jun-1993 -- Rename from PGTLB1, add code to label superscript 
C                 seconds above the '.' and add DO2 option [nebk/jm]
*/
void pgtbx7(char const* suptyp, char signf, char asign, int ival[3], double rval, int writ[4], int sprec, int do2, char* text, int* tlen, int* last) {
    /* the superscripting strings */
    char const* super[3][4] = { {"\\ud\\d"       , "\\uh\\d"       , "\\um\\d"       , "\\us\\d"},
                                {"\\u\\(2199)\\d", "\\u\\(2729)\\d", "\\u\\(2727)\\d", "\\u\\(2728)\\d"},
                                {"\\u \\d"       , "\\u \\d"       , "\\u \\d"       , "\\u \\d" }};

    int          nch;
    char const** suppnt;

    if( strcmp(suptyp, "DHMS")==0 )
        suppnt = super[0];
    else if( strcmp(suptyp, "DMS")==0 )
        suppnt = super[1];
    else
        suppnt = super[2];

    *tlen   = 0;
    text[0] = '\0';

    /* C   Days field */
    if( writ[0] ) {
        /* depending on wether to print the sign or not choose the correct
         * format */
        char const* fmt = (signf=='D' && asign!=' ') ? "%3$c%1$d%2$s" : "%1$d%2$s" ;
        *last  = *tlen;
        nch   = sprintf(&text[*tlen], fmt, ival[0], suppnt[0], asign);
        *tlen = *tlen + nch;
    }

    /* C   Hours field */
    if( writ[1] ) {
        *last = *tlen;

        /* print it in two parts because all combinations of sign field yes/no and/or the leading
         * zeroes yes/no must be handled */
        if( signf=='H' && asign!=' ' ) {
            text[ *tlen ] = asign;
            *tlen         = *tlen + 1;
        }
        /* print the number with or without fill zeroes */
        nch   = sprintf(&text[*tlen], do2 ? "%02d%s" : "%d%s", ival[1], suppnt[1]);
        *tlen = *tlen + nch;
    }
    /* C   Minutes field */
    if( writ[2] ) {
        *last = *tlen;
        /* print it in two parts because all combinations of sign field yes/no and/or the leading
         * zeroes yes/no must be handled */
        if( signf=='M' && asign!=' ' ) {
            text[ *tlen ] = asign;
            *tlen         = *tlen + 1;
        }
        /* print the number with or without fill zeroes */
        nch   = sprintf(&text[*tlen], do2 ? "%02d%s" : "%d%s", ival[2], suppnt[2]);
        *tlen = *tlen + nch;
    }
    /* C   Seconds field */
    if( writ[3] ) {
        char tmp[32];

        *last = *tlen;
        /* format the ddd.^[s"][ddd] into tmp string first */
        if( sprec>=1 ) {
            /* C   Fractional label.  Upto 3 places after the decimal point allowed
               C   Muck around to get the superscript on top of the decimal point */
            double  isec, ifrac;
            /* split the number into integer and fractional part */
            (void)modf(rval, &isec);
            /* multiply the fractional part by 10^precision and take the
             * integer part of /that/ */
            (void)modf((rval - (int)isec)*pow(10.0, sprec), &ifrac);
            /* Now we can nicely format the before the decimal point, after
             * the decimal point /and/ (attempt to) make the superscript on
             * top of the decimal point */
            sprintf(tmp, do2 ? "%02d.\\b%s%d" : "%d.\\b%s%d", (int)isec, suppnt[3], (int)ifrac);
        } else {
            /* integer label */
            sprintf(tmp, do2 ? "%02d%s" : "%d%s", (int)rint(rval), suppnt[3]);
        }
        /* number was formatted according to spec, now write to output
         * label, first honouring the sign, if any */
        if( signf=='S' && asign!=' ' ) {
            text[ *tlen ] = asign;
            *tlen         = *tlen + 1;
        }
        nch   = sprintf(&text[*tlen], "%s", tmp);
        *tlen = *tlen + nch;
    }
    /* C   A trailing blank will occur if no superscripting wanted */
    if( *tlen>=5 && strcmp(&text[*tlen - 5], super[2][0])==0 ) {
        *tlen         = *tlen - 5;
        text[ *tlen ] = '\0';
    }
}
