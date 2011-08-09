/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */
 
/*
 *  This file contains the cpgplot interface to giza
 *  so that giza can be used as a drop-in replacement
 *  for the cpgplot library
 *
 */

#include "giza.h"
#include "cpgplot.h"

/***************************************************************
 * Function to convert PGPLOT units value to giza units value
 ***************************************************************/
int units_giza(int pgplotunits)
{
  int units;
  switch (pgplotunits)
  {
  case 0:
     units = GIZA_UNITS_NORMALIZED;
     break;
  case 1:
     units = GIZA_UNITS_INCHES;
     break;
  case 2:
     units = GIZA_UNITS_MM;
     break;
  case 3:
     units = GIZA_UNITS_PIXELS;
     break;
  case 4:
     units = GIZA_UNITS_WORLD;
     break;
  default: /* giza will give an error */
     units = pgplotunits;
     break;
  }
  
  return units;
}

/***************************************************************
 * Routine to convert PGPLOT "trans" array to giza "affine" array
 ***************************************************************/
void convert_tr_to_affine(const float *tr, float *affine)
{
  affine[0] = tr[1];
  affine[1] = tr[2];
  affine[2] = tr[4];
  affine[3] = tr[5];
  affine[4] = tr[0] + 0.5f*(tr[1]);
  affine[5] = tr[3] + 0.5f*(tr[5]);
}

/***************************************************************
 * PGARRO -- draw an arrow
 ***************************************************************/
void cpgarro(float x1, float y1, float x2, float y2)
{

}

/***************************************************************
 * PGASK -- control new page prompting
 ***************************************************************/
void cpgask(Logical flag)
{

}

/***************************************************************
 * PGAXIS -- draw an axis
 ***************************************************************/
void cpgaxis(const char *opt, float x1, float y1, float x2, float y2, \
             float v1, float v2, float step, int nsub, float dmajl, \
             float dmajr, float fmin, float disp, float orient)
{

}

/***************************************************************
 * PGBAND -- read cursor position, with anchor
 ***************************************************************/
int cpgband(int mode, int posn, float xref, float yref, float *x,\
            float *y, char *ch_scalar)
{

}

/***************************************************************
 * PGBBUF -- begin batch of output (buffer)
 ***************************************************************/
void cpgbbuf(void)
{

}

/***************************************************************
 * PGBEG -- open a graphics device
 ***************************************************************/
int cpgbeg(int unit, const char *file, int nxsub, int nysub)
{

}

/***************************************************************
 * PGBIN -- histogram of binned data
 ***************************************************************/
void cpgbin(int nbin, const float *x, const float *data, \
 Logical center)
{

}

/***************************************************************
 * PGBOX -- draw labeled frame around viewport
 ***************************************************************/
void cpgbox(const char *xopt, float xtick, int nxsub, \
 const char *yopt, float ytick, int nysub)
{

}

/***************************************************************
 * PGCIRC -- draw a circle, using fill-area attributes
 ***************************************************************/
void cpgcirc(float xcent, float ycent, float radius)
{

}

/***************************************************************
 * PGCLOS -- close the selected graphics device
 ***************************************************************/
void cpgclos(void)
{

}

/***************************************************************
 * PGCONB -- contour map of a 2D data array, with blanking
 ***************************************************************/
void cpgconb(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr, \
 float blank)
{

}

/***************************************************************
 * PGCONF -- fill between two contours
 ***************************************************************/
void cpgconf(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float c1, float c2, const float *tr)
{

}

/***************************************************************
 * PGCONL -- label contour map of a 2D data array
 ***************************************************************/
void cpgconl(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float c, const float *tr, const char *label, \
 int intval, int minint)
{

}

/***************************************************************
 * PGCONS -- contour map of a 2D data array (fast algorithm)
 ***************************************************************/
void cpgcons(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr)
{

}

/***************************************************************
 * PGCONT -- contour map of a 2D data array (contour-following)
 ***************************************************************/
void cpgcont(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr)
{

}

/***************************************************************
 * PGCTAB -- install the color table to be used by PGIMAG
 ***************************************************************/
void cpgctab(const float *l, const float *r, const float *g, \
 const float *b, int nc, float contra, float bright)
{

}

/***************************************************************
 * PGCURS -- read cursor position
 ***************************************************************/
int cpgcurs(float *x, float *y, char *ch_scalar)
{

}

/***************************************************************
 * PGDRAW -- draw a line from the current pen position to a point
 ***************************************************************/
void cpgdraw(float x, float y)
{

}

/***************************************************************
 * PGEBUF -- end batch of output (buffer)
 ***************************************************************/
void cpgebuf(void)
{

}

/***************************************************************
 * PGEND -- close all open graphics devices
 ***************************************************************/
void cpgend(void)
{

}

/***************************************************************
 * PGENV -- set window and viewport and draw labeled frame
 ***************************************************************/
void cpgenv(float xmin, float xmax, float ymin, float ymax, \
 int just, int axis)
{

}

/***************************************************************
 * PGERAS -- erase all graphics from current page
 ***************************************************************/
void cpgeras(void)
{

}

/***************************************************************
 * PGERR1 -- horizontal or vertical error bar
 ***************************************************************/
void cpgerr1(int dir, float x, float y, float e, float t)
{

}

/***************************************************************
 * PGERRB -- horizontal or vertical error bar
 ***************************************************************/
void cpgerrb(int dir, int n, const float *x, const float *y, \
 const float *e, float t)
{

}

/***************************************************************
 * PGERRX -- horizontal error bar
 ***************************************************************/
void cpgerrx(int n, const float *x1, const float *x2, \
 const float *y, float t)
{

}

/***************************************************************
 * PGERRY -- vertical error bar
 ***************************************************************/
void cpgerry(int n, const float *x, const float *y1, \
 const float *y2, float t)
{

}

/***************************************************************
 * PGETXT -- erase text from graphics display
 ***************************************************************/
void cpgetxt(void)
{

}

/***************************************************************
 * PGFUNT -- function defined by X = F(T), Y = G(T)

/***************************************************************
 * PGFUNX -- function defined by Y = F(X)

/***************************************************************
 * PGFUNY -- function defined by X = F(Y)

/***************************************************************
 * PGGRAY -- gray-scale map of a 2D data array
 ***************************************************************/
void cpggray(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float fg, float bg, const float *tr)
{

}

/***************************************************************
 * PGHI2D -- cross-sections through a 2D data array
 ***************************************************************/
void cpghi2d(const float *data, int nxv, int nyv, int ix1, \
 int ix2, int iy1, int iy2, const float *x, int ioff, float bias, \
 Logical center, float *ylims)
{

}

/***************************************************************
 * PGHIST -- histogram of unbinned data
 ***************************************************************/
void cpghist(int n, const float *data, float datmin, float datmax, \
 int nbin, int pgflag)
{

}

/***************************************************************
 * PGIDEN -- write username, date, and time at bottom of plot
 ***************************************************************/
void cpgiden(void)
{

}

/***************************************************************
 * PGIMAG -- color image from a 2D data array
 ***************************************************************/
void cpgimag(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float a1, float a2, const float *tr)
{

}

/***************************************************************
 * PGLAB -- write labels for x-axis, y-axis, and top of plot
 ***************************************************************/
void cpglab(const char *xlbl, const char *ylbl, const char *toplbl)
{

}

/***************************************************************
 * PGLCUR -- draw a line using the cursor
 ***************************************************************/
void cpglcur(int maxpt, int *npt, float *x, float *y)
{

}

/***************************************************************
 * PGLDEV -- list available device types on standard output
 ***************************************************************/
void cpgldev(void)
{

}

/***************************************************************
 * PGLEN -- find length of a string in a variety of units
 ***************************************************************/
void cpglen(int units, const char *string, float *xl, float *yl)
{

}

/***************************************************************
 * PGLINE -- draw a polyline (curve defined by line-segments)
 ***************************************************************/
void cpgline(int n, const float *xpts, const float *ypts)
{

}

/***************************************************************
 * PGMOVE -- move pen (change current pen position)
 ***************************************************************/
void cpgmove(float x, float y)
{

}

/***************************************************************
 * PGMTXT -- write text at position relative to viewport
 ***************************************************************/
void cpgmtxt(const char *side, float disp, float coord, \
 float fjust, const char *text)
{

}

/***************************************************************
 * PGNCUR -- mark a set of points using the cursor
 ***************************************************************/
void cpgncur(int maxpt, int *npt, float *x, float *y, int symbol)
{

}

/***************************************************************
 * PGNUMB -- convert a number into a plottable character string
 ***************************************************************/
void cpgnumb(int mm, int pp, int form, char *string, \
 int *string_length)
{

}

/***************************************************************
 * PGOLIN -- mark a set of points using the cursor
 ***************************************************************/
void cpgolin(int maxpt, int *npt, float *x, float *y, int symbol)
{

}

/***************************************************************
 * PGOPEN -- open a graphics device
 ***************************************************************/
int cpgopen(const char *device)
{

}

/***************************************************************
 * PGPAGE -- advance to new page
 ***************************************************************/
void cpgpage(void)
{

}

/***************************************************************
 * PGPANL -- switch to a different panel on the view surface
 ***************************************************************/
void cpgpanl(int nxc, int nyc)
{

}

/***************************************************************
 * PGPAP -- change the size of the view surface
 ***************************************************************/
void cpgpap(float width, float aspect)
{

}

/***************************************************************
 * PGPIXL -- draw pixels
 ***************************************************************/
void cpgpixl(const int *ia, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float x1, float x2, float y1, float y2)
{

}

/***************************************************************
 * PGPNTS -- draw several graph markers, not all the same
 ***************************************************************/
void cpgpnts(int n, const float *x, const float *y, \
 const int *symbol, int ns)
{

}

/***************************************************************
 * PGPOLY -- draw a polygon, using fill-area attributes
 ***************************************************************/
void cpgpoly(int n, const float *xpts, const float *ypts)
{

}

/***************************************************************
 * PGPT -- draw several graph markers
 ***************************************************************/
void cpgpt(int n, const float *xpts, const float *ypts, int symbol)
{

}

/***************************************************************
 * PGPT1 -- draw one graph marker
 ***************************************************************/
void cpgpt1(float xpt, float ypt, int symbol)
{

}

/***************************************************************
 * PGPTXT -- write text at arbitrary position and angle
 ***************************************************************/
void cpgptxt(float x, float y, float angle, float fjust, \
 const char *text)
{

}

/***************************************************************
 * PGQAH -- inquire arrow-head style
 ***************************************************************/
void cpgqah(int *fs, float *angle, float *barb)
{

}

/***************************************************************
 * PGQCF -- inquire character font
 ***************************************************************/
void cpgqcf(int *font)
{

}

/***************************************************************
 * PGQCH -- inquire character height
 ***************************************************************/
void cpgqch(float *size)
{

}

/***************************************************************
 * PGQCI -- inquire color index
 ***************************************************************/
void cpgqci(int *ci)
{

}

/***************************************************************
 * PGQCIR -- inquire color index range
 ***************************************************************/
void cpgqcir(int *icilo, int *icihi)
{

}

/***************************************************************
 * PGQCLP -- inquire clipping status
 ***************************************************************/
void cpgqclp(int *state)
{

}

/***************************************************************
 * PGQCOL -- inquire color capability
 ***************************************************************/
void cpgqcol(int *ci1, int *ci2)
{

}

/***************************************************************
 * PGQCR -- inquire color representation
 ***************************************************************/
void cpgqcr(int ci, float *cr, float *cg, float *cb)
{

}

/***************************************************************
 * PGQCS -- inquire character height in a variety of units
 ***************************************************************/
void cpgqcs(int units, float *xch, float *ych)
{

}

/***************************************************************
 * PGQDT -- inquire name of nth available device type
 ***************************************************************/
void cpgqdt(int n, char *type, int *type_length, char *descr, \
 int *descr_length, int *inter)
{

}

/***************************************************************
 * PGQFS -- inquire fill-area style
 ***************************************************************/
void cpgqfs(int *fs)
{

}

/***************************************************************
 * PGQHS -- inquire hatching style
 ***************************************************************/
void cpgqhs(float *angle, float *sepn, float* phase)
{

}

/***************************************************************
 * PGQID -- inquire current device identifier
 ***************************************************************/
void cpgqid(int *id)
{

}

/***************************************************************
 * PGQINF -- inquire PGPLOT general information
 ***************************************************************/
void cpgqinf(const char *item, char *value, int *value_length)
{

}

/***************************************************************
 * PGQITF -- inquire image transfer function
 ***************************************************************/
void cpgqitf(int *itf)
{

}

/***************************************************************
 * PGQLS -- inquire line style
 ***************************************************************/
void cpgqls(int *ls)
{

}

/***************************************************************
 * PGQLW -- inquire line width
 ***************************************************************/
void cpgqlw(int *lw)
{

}

/***************************************************************
 * PGQNDT -- inquire number of available device types
 ***************************************************************/
void cpgqndt(int *n)
{

}

/***************************************************************
 * PGQPOS -- inquire current pen position
 ***************************************************************/
void cpgqpos(float *x, float *y)
{

}

/***************************************************************
 * PGQTBG -- inquire text background color index
 ***************************************************************/
void cpgqtbg(int *tbci)
{

}

/***************************************************************
 * PGQTXT -- find bounding box of text string
 ***************************************************************/
void cpgqtxt(float x, float y, float angle, float fjust, \
 const char *text, float *xbox, float *ybox)
{

}

/***************************************************************
 * PGQVP -- inquire viewport size and position
 ***************************************************************/
void cpgqvp(int units, float *x1, float *x2, float *y1, float *y2)
{

}

/***************************************************************
 * PGQVSZ -- inquire size of view surface
 ***************************************************************/
void cpgqvsz(int units, float *x1, float *x2, float *y1, float *y2)
{

}

/***************************************************************
 * PGQWIN -- inquire window boundary coordinates
 ***************************************************************/
void cpgqwin(float *x1, float *x2, float *y1, float *y2)
{

}

/***************************************************************
 * PGRECT -- draw a rectangle, using fill-area attributes
 ***************************************************************/
void cpgrect(float x1, float x2, float y1, float y2)
{

}

/***************************************************************
 * PGRND -- find the smallest `round' number greater than x
 ***************************************************************/
float cpgrnd(float x, int *nsub)
{

}

/***************************************************************
 * PGRNGE -- choose axis limits
 ***************************************************************/
void cpgrnge(float x1, float x2, float *xlo, float *xhi)
{

}

/***************************************************************
 * PGSAH -- set arrow-head style
 ***************************************************************/
void cpgsah(int fs, float angle, float barb)
{

}

/***************************************************************
 * PGSAVE -- save PGPLOT attributes
 ***************************************************************/
void cpgsave(void)
{

}

/***************************************************************
 * PGUNSA -- restore PGPLOT attributes
 ***************************************************************/
void cpgunsa(void)
{

}

/***************************************************************
 * PGSCF -- set character font
 ***************************************************************/
void cpgscf(int font)
{

}

/***************************************************************
 * PGSCH -- set character height
 ***************************************************************/
void cpgsch(float size)
{

}

/***************************************************************
 * PGSCI -- set color index
 ***************************************************************/
void cpgsci(int ci)
{

}

/***************************************************************
 * PGSCIR -- set color index range
 ***************************************************************/
void cpgscir(int icilo, int icihi)
{

}

/***************************************************************
 * PGSCLP -- enable or disable clipping at edge of viewport
 ***************************************************************/
void cpgsclp(int state)
{

}

/***************************************************************
 * PGSCR -- set color representation
 ***************************************************************/
void cpgscr(int ci, float cr, float cg, float cb)
{

}

/***************************************************************
 * PGSCRL -- scroll window
 ***************************************************************/
void cpgscrl(float dx, float dy)
{

}

/***************************************************************
 * PGSCRN -- set color representation by name
 ***************************************************************/
void cpgscrn(int ci, const char *name, int *ier)
{

}

/***************************************************************
 * PGSFS -- set fill-area style
 ***************************************************************/
void cpgsfs(int fs)
{

}

/***************************************************************
 * PGSHLS -- set color representation using HLS system
 ***************************************************************/
void cpgshls(int ci, float ch, float cl, float cs)
{

}

/***************************************************************
 * PGSHS -- set hatching style
 ***************************************************************/
void cpgshs(float angle, float sepn, float phase)
{

}

/***************************************************************
 * PGSITF -- set image transfer function
 ***************************************************************/
void cpgsitf(int itf)
{

}

/***************************************************************
 * PGSLCT -- select an open graphics device
 ***************************************************************/
void cpgslct(int id)
{

}

/***************************************************************
 * PGSLS -- set line style
 ***************************************************************/
void cpgsls(int ls)
{

}

/***************************************************************
 * PGSLW -- set line width
 ***************************************************************/
void cpgslw(int lw)
{

}

/***************************************************************
 * PGSTBG -- set text background color index
 ***************************************************************/
void cpgstbg(int tbci)
{

}

/***************************************************************
 * PGSUBP -- subdivide view surface into panels
 ***************************************************************/
void cpgsubp(int nxsub, int nysub)
{

}

/***************************************************************
 * PGSVP -- set viewport (normalized device coordinates)
 ***************************************************************/
void cpgsvp(float xleft, float xright, float ybot, float ytop)
{

}

/***************************************************************
 * PGSWIN -- set window
 ***************************************************************/
void cpgswin(float x1, float x2, float y1, float y2)
{

}

/***************************************************************
 * PGTBOX -- draw frame and write (DD) HH MM SS.S labelling
 ***************************************************************/
void cpgtbox(const char *xopt, float xtick, int nxsub, \
 const char *yopt, float ytick, int nysub)
{

}

/***************************************************************
 * PGTEXT -- write text (horizontal, left-justified)
 ***************************************************************/
void cpgtext(float x, float y, const char *text)
{

}

/***************************************************************
 * PGTICK -- draw a single tick mark on an axis
 ***************************************************************/
void cpgtick(float x1, float y1, float x2, float y2, float v, \
 float tikl, float tikr, float disp, float orient, const char *str)
{

}

/***************************************************************
 * PGUPDT -- update display
 ***************************************************************/
void cpgupdt(void)
{

}

/***************************************************************
 * PGVECT -- vector map of a 2D data array, with blanking
 ***************************************************************/
void cpgvect(const float *a, const float *b, int idim, int jdim, \
 int i1, int i2, int j1, int j2, float c, int nc, \
 const float *tr, float blank)
{

}

/***************************************************************
 * PGVSIZ -- set viewport (inches)
 ***************************************************************/
void cpgvsiz(float xleft, float xright, float ybot, float ytop)
{

}

/***************************************************************
 * PGVSTD -- set standard (default) viewport
 ***************************************************************/
void cpgvstd(void)
{

}

/***************************************************************
 * PGWEDG -- annotate an image plot with a wedge
 ***************************************************************/
void cpgwedg(const char *side, float disp, float width, \
 float fg, float bg, const char *label)
{

}

/***************************************************************
 * PGWNAD -- set window and adjust viewport to same aspect ratio
 ***************************************************************/
void cpgwnad(float x1, float x2, float y1, float y2)
{

}
