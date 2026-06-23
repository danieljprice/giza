/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

/*
 *  This file contains the cpgplot interface to giza
 *  so that giza can be used as a drop-in replacement
 *  for the cpgplot library
 *
 */

#include "giza.h"
#include "giza-private.h"
#include "giza-io-private.h" /* for _giza_error() */
#include "giza-driver-xw-private.h"
#include "giza-driver-eps-private.h"
#include "giza-driver-osxcocoa-private.h"
#include "cpgplot.h"
#include <string.h>
#include <strings.h> /* for strcasecmp */
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
static int pgfont;

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
  default: /* will give an error in giza */
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
  affine[1] = tr[4];
  affine[2] = tr[2];
  affine[3] = tr[5];
  affine[4] = tr[0] + 0.5f*(tr[1]);
  affine[5] = tr[3] + 0.5f*(tr[5]);
}

/***************************************************************
 * cpgarro -- draw an arrow
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgarro(float x1, float y1, float x2, float y2)
{
  giza_arrow_float(x1,y1,x2,y2);
}

/***************************************************************
 * cpgask -- control new page prompting
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgask(Logical flag)
{
  if (flag) {
    giza_start_prompting();
  } else {
    giza_stop_prompting();
  }

}

/***************************************************************
 * cpgaxis -- draw an axis
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgaxis(const char *opt, float x1, float y1, float x2, float y2, \
             float v1, float v2, float step, int nsub, float dmajl, \
             float dmajr, float fmin, float disp, float orient)
{
   giza_axis_float(opt,x1,y1,x2,y2,v1,v2,step,nsub,dmajl,dmajr,fmin,disp,orient);
}

/***************************************************************
 * cpgband -- read cursor position, with anchor
 * Status: IMPLEMENTED
 ***************************************************************/
int cpgband(int mode, int posn, float xref, float yref, float *x,\
            float *y, char *ch_scalar)
{
   return giza_band_float(mode,posn,xref,yref,x,y,ch_scalar);
}

/***************************************************************
 * cpgbbuf -- begin batch of output (buffer)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgbbuf(void)
{
  giza_begin_buffer();
}

/***************************************************************
 * cpgbeg -- open a graphics device
 * Status: IMPLEMENTED
 ***************************************************************/
int cpgbeg(int unit, const char *file, int nxsub, int nysub)
{
  int id = cpgopen(file);
  if (id > 0 && (nxsub > 1 || nysub > 1)) {
     cpgsubp(nxsub, nysub);
  }
  return id;
}

/***************************************************************
 * cpgbin -- histogram of binned data
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgbin(int nbin, const float *x, const float *data, \
 Logical center)
{
  giza_histogram_binned_float(nbin,x,data,(int) center);
}

/***************************************************************
 * cpgbox -- draw labeled frame around viewport
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgbox(const char *xopt, float xtick, int nxsub, \
 const char *yopt, float ytick, int nysub)
{
  giza_box(xopt, xtick, nxsub, yopt, ytick, nysub);
}

/***************************************************************
 * cpgcirc -- draw a circle, using fill-area attributes
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgcirc(float xcent, float ycent, float radius)
{
  giza_circle_float(xcent,ycent,radius);
}

/***************************************************************
 * cpgclos -- close the selected graphics device
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgclos(void)
{
  giza_close_device();
}

/***************************************************************
 * cpgconb -- contour map of a 2D data array, with blanking
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgconb(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr, \
 float blank)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_contour_blanked_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,nc,c,affine,blank);
}

/***************************************************************
 * cpgconf -- fill between two contours
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgconf(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float c1, float c2, const float *tr)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_contour_fill_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,c1,c2,affine);
}

/***************************************************************
 * cpgconl -- label contour map of a 2D data array
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgconl(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float c, const float *tr, const char *label, \
 int intval, int minint)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_contour_labelled_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,c,affine,
                              label,intval,minint);
}

/***************************************************************
 * cpgcons -- contour map of a 2D data array (fast algorithm)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgcons(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_contour_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,nc,c,affine);
}

/***************************************************************
 * cpgcont -- contour map of a 2D data array (contour-following)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgcont(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, const float *c, int nc, const float *tr)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_contour_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,nc,c,affine);
}

/***************************************************************
 * cpgconx -- contour map of a 2D data array (non rectangular)
 * Status: IMPLEMENTED
 *
 * Like cpgcont but uses a user-supplied callback instead of a
 * tr matrix. The callback receives grid-index coordinates (1-based)
 * and is responsible for transforming to world coords and drawing.
 *
 * plot(visble, x, y, z):
 *   visble=0: move to (x,y)
 *   visble=1: draw to (x,y)
 *   x,y: interpolated grid indices (1-based, real)
 *   z: contour level
 ***************************************************************/
void cpgconx(const float *a, int idim, int jdim, int i1, int i2,
 int j1, int j2, const float *c, int nc,
 void (*plot)(int *visble, float *x, float *y, float *z))
{
  if (!plot) return;

#define xsect_cx(p1,p2) (hcx[p2]*xhcx[p1]-hcx[p1]*xhcx[p2])/(hcx[p2]-hcx[p1])
#define ysect_cx(p1,p2) (hcx[p2]*yhcx[p1]-hcx[p1]*yhcx[p2])/(hcx[p2]-hcx[p1])

  int sh[5];
  float hcx[5];
  float xhcx[5], yhcx[5];
  int im[4] = { 0, 1, 1, 0 };
  int jm[4] = { 0, 0, 1, 1 };
  float temp1, temp2, dmin, dmax;
  float x1 = 0.f, x2 = 0.f, y1 = 0.f, y2 = 0.f;
  int i, j, k, m;
  int m1, m2, m3, case_value;
  int castab[3][3][3] = {
    {{0, 0, 8}, {0, 2, 5}, {7, 6, 9}},
    {{0, 3, 4}, {1, 3, 1}, {4, 3, 0}},
    {{9, 6, 7}, {5, 2, 0}, {8, 0, 0}}
  };
  int nca = abs(nc);

  /* Iterate over grid cells using 0-based internal indices,
   * converting i1..i2 from 1-based pgplot to 0-based */
  int ci1 = i1 - 1, ci2 = i2 - 1;
  int cj1 = j1 - 1, cj2 = j2 - 1;

  for (j = cj2 - 1; j >= cj1; j--)
    {
      for (i = ci1; i < ci2; i++)
       {
         temp1 = MIN(a[j*idim+i], a[(j+1)*idim+i]);
         temp2 = MIN(a[j*idim+(i+1)], a[(j+1)*idim+(i+1)]);
         dmin = MIN(temp1, temp2);

         temp1 = MAX(a[j*idim+i], a[(j+1)*idim+i]);
         temp2 = MAX(a[j*idim+(i+1)], a[(j+1)*idim+(i+1)]);
         dmax = MAX(temp1, temp2);

         if (dmax < c[0] || dmin > c[nca - 1])
           continue;

         for (k = 0; k < nca; ++k)
           {
             if (c[k] < dmin || c[k] > dmax)
               continue;
             for (m = 4; m >= 0; m--)
               {
                 if (m > 0)
                   {
                     hcx[m] = a[(j + jm[m-1])*idim + (i + im[m-1])] - c[k];
                     /* Grid indices: 1-based for pgplot callback */
                     xhcx[m] = (float)(i + im[m-1] + 1);
                     yhcx[m] = (float)(j + jm[m-1] + 1);
                   }
                 else
                   {
                     hcx[0] = 0.25f * (hcx[1] + hcx[2] + hcx[3] + hcx[4]);
                     xhcx[0] = 0.5f * (float)(i + i + 1 + 1);
                     yhcx[0] = 0.5f * (float)(j + j + 1 + 1);
                   }
                 if (hcx[m] > 0.0f) sh[m] = 1;
                 else if (hcx[m] < 0.0f) sh[m] = -1;
                 else sh[m] = 0;
               }

             for (m = 1; m <= 4; ++m)
               {
                 m1 = m; m2 = 0;
                 m3 = (m != 4) ? m + 1 : 1;

                 if ((case_value =
                      castab[sh[m1]+1][sh[m2]+1][sh[m3]+1]) == 0)
                   continue;

                 switch (case_value)
                   {
                   case 1: x1=xhcx[m1]; y1=yhcx[m1]; x2=xhcx[m2]; y2=yhcx[m2]; break;
                   case 2: x1=xhcx[m2]; y1=yhcx[m2]; x2=xhcx[m3]; y2=yhcx[m3]; break;
                   case 3: x1=xhcx[m3]; y1=yhcx[m3]; x2=xhcx[m1]; y2=yhcx[m1]; break;
                   case 4: x1=xhcx[m1]; y1=yhcx[m1]; x2=xsect_cx(m2,m3); y2=ysect_cx(m2,m3); break;
                   case 5: x1=xhcx[m2]; y1=yhcx[m2]; x2=xsect_cx(m3,m1); y2=ysect_cx(m3,m1); break;
                   case 6: x1=xhcx[m3]; y1=yhcx[m3]; x2=xsect_cx(m3,m2); y2=ysect_cx(m3,m2); break;
                   case 7: x1=xsect_cx(m1,m2); y1=ysect_cx(m1,m2); x2=xsect_cx(m2,m3); y2=ysect_cx(m2,m3); break;
                   case 8: x1=xsect_cx(m2,m3); y1=ysect_cx(m2,m3); x2=xsect_cx(m3,m1); y2=ysect_cx(m3,m1); break;
                   case 9: x1=xsect_cx(m3,m1); y1=ysect_cx(m3,m1); x2=xsect_cx(m1,m2); y2=ysect_cx(m1,m2); break;
                   default: break;
                   }
                 /* Call user callback: move to start, draw to end */
                 {
                   int vis0 = 0, vis1 = 1;
                   float z = c[k];
                   plot(&vis0, &x1, &y1, &z);
                   plot(&vis1, &x2, &y2, &z);
                 }
               }
           }
       }
    }

#undef xsect_cx
#undef ysect_cx
}

/***************************************************************
 * cpgctab -- install the color table to be used by pgimag
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgctab(const float *l, const float *r, const float *g, \
 const float *b, int nc, float contra, float bright)
{
   giza_set_colour_table_float(l,r,g,b,nc,contra,bright);
}

/***************************************************************
 * cpgcurs -- read cursor position
 * Status: IMPLEMENTED
 ***************************************************************/
int cpgcurs(float *x, float *y, char *ch_scalar)
{
   return giza_get_key_press_float(x,y,ch_scalar);
}

/***************************************************************
 * cpgdraw -- draw a line from the current pen position to a point
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgdraw(float x, float y)
{
   giza_draw_float(x, y);
}

/***************************************************************
 * cpgebuf -- end batch of output (buffer)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgebuf(void)
{
   giza_end_buffer();
}

/***************************************************************
 * cpgend -- close all open graphics devices
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgend(void)
{
   giza_close_devices();
}

/***************************************************************
 * cpgenv -- set window and viewport and draw labeled frame
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgenv(float xmin, float xmax, float ymin, float ymax, \
 int just, int axis)
{
  giza_set_environment_float(xmin, xmax, ymin, ymax, just, axis);
}

/***************************************************************
 * cpgeras -- erase all graphics from current page
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgeras(void)
{
  giza_draw_background();
}

/***************************************************************
 * cpgerr1 -- horizontal or vertical error bar
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgerr1(int dir, float x, float y, float e, float t)
{
  float xi[1], yi[1], ei[1];
  xi[0] = x;
  yi[0] = y;
  ei[0] = e;
  cpgerrb(dir, 1, xi, yi, ei, t);
}

/***************************************************************
 * cpgerrb -- horizontal or vertical error bar
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgerrb(int dir, int n, const float *x, const float *y, \
 const float *e, float t)
{
  giza_error_bars_float(dir, n, x, y, e, t);
}

/***************************************************************
 * cpgerrx -- horizontal error bar
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgerrx(int n, const float *x1, const float *x2, \
 const float *y, float t)
{
  giza_error_bars_hori_float(n, x1, x2, y, t);
}

/***************************************************************
 * cpgerry -- vertical error bar
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgerry(int n, const float *x, const float *y1, \
 const float *y2, float t)
{
  giza_error_bars_vert_float(n, x, y1, y2, t);
}

/***************************************************************
 * cpgetxt -- erase text from graphics display
 * Status: NOT IMPLEMENTED
 ***************************************************************/
void cpgetxt(void)
{

}

/***************************************************************
 * cpggray -- gray-scale map of a 2D data array
 * Status: IMPLEMENTED
 ***************************************************************/
void cpggray(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float fg, float bg, const float *tr)
{
  float affine[6];
  affine[0] = tr[1];
  affine[1] = tr[4];
  affine[2] = tr[2];
  affine[3] = tr[5];
  affine[4] = tr[0] + tr[1]*(i1 - 0.5f) + tr[2]*(j1 - 0.5f);
  affine[5] = tr[3] + tr[4]*(i1 - 0.5f) + tr[5]*(j1 - 0.5f);
  giza_render_gray_shade_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,fg,bg, \
                         GIZA_EXTEND_NONE,GIZA_FILTER_NEAREST,affine);
}

/***************************************************************
 * cpghi2d -- cross-sections through a 2D data array
 * Status: IMPLEMENTED
 *
 * Draws cross-sections through a 2D array as a waterfall plot
 * with hidden-line removal using a horizon algorithm.
 *
 * data(nxv,nyv) - the data array
 * ix1..ix2      - range of x indices to plot (1-based)
 * iy1..iy2      - range of y (row) indices to plot (1-based)
 * x(ix1:ix2)    - x-coordinates for columns
 * ioff          - horizontal offset in x-array units per row
 * bias          - vertical offset per row
 * center        - if true, center rows on the x range
 * ylims(ix1:ix2)- work array (used as upper horizon)
 ***************************************************************/
void cpghi2d(const float *data, int nxv, int nyv, int ix1, \
 int ix2, int iy1, int iy2, const float *x, int ioff, float bias, \
 Logical center, float *ylims)
{
  int nx = ix2 - ix1 + 1;
  if (nx < 1 || iy1 > iy2) return;

  /* Initialize the horizon (ylims) to a very low value */
  int i, iy;
  for (i = 0; i < nx; i++) {
    ylims[i] = -1.0e30f;
  }

  /* Process rows from iy1 to iy2 (1-based pgplot indices) */
  for (iy = iy1; iy <= iy2; iy++) {
    float yoff = bias * (float)(iy - iy1);
    float xoff = 0.0f;

    if (center) {
      /* Center offset: shift x by half the total ioff range */
      xoff = (float)ioff * (float)(iy - iy1) * 0.5f;
    } else {
      xoff = (float)ioff * (float)(iy - iy1);
    }

    /* Draw cross-section for this row, clipping against horizon */
    int started = 0;
    for (i = ix1; i <= ix2; i++) {
      /* pgplot uses 1-based Fortran indexing: data(i,iy) = data[(iy-1)*nxv + (i-1)] */
      float val = data[(iy - 1) * nxv + (i - 1)] + yoff;
      float xi = x[i - 1] + xoff;
      int hi = i - ix1; /* index into ylims/horizon */

      if (val > ylims[hi]) {
        /* Point is above horizon — visible */
        if (!started) {
          cpgmove(xi, val);
          started = 1;
        } else {
          cpgdraw(xi, val);
        }
        ylims[hi] = val;
      } else {
        /* Below horizon — hidden */
        started = 0;
      }
    }
  }
}

/***************************************************************
 * cpghist -- histogram of unbinned data
 * Status: IMPLEMENTED
 ***************************************************************/
void cpghist(int n, const float *data, float datmin, float datmax, \
 int nbin, int pgflag)
{
  giza_histogram_float(n,data,datmin,datmax,nbin,pgflag);
}

/***************************************************************
 * cpgiden -- write username, date, and time at bottom of plot
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgiden(void)
{
  giza_print_id();
}

/***************************************************************
 * cpgimag -- color image from a 2D data array
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgimag(const float *a, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float a1, float a2, const float *tr)
{
  float affine[6];
  affine[0] = tr[1];
  affine[1] = tr[4];
  affine[2] = tr[2];
  affine[3] = tr[5];
  affine[4] = tr[0] + tr[1]*(i1 - 0.5f) + tr[2]*(j1 - 0.5f);
  affine[5] = tr[3] + tr[4]*(i1 - 0.5f) + tr[5]*(j1 - 0.5f);
  giza_render_float(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,a1,a2, \
                    GIZA_EXTEND_NONE,GIZA_FILTER_NEAREST,affine);
}

/***************************************************************
 * cpglab -- write labels for x-axis, y-axis, and top of plot
 * Status: IMPLEMENTED
 ***************************************************************/
void cpglab(const char *xlbl, const char *ylbl, const char *toplbl)
{
  giza_label(xlbl, ylbl, toplbl);
}

/***************************************************************
 * cpglcur -- draw a line using the cursor
 * Status: IMPLEMENTED
 ***************************************************************/
void cpglcur(int maxpt, int *npt, float *x, float *y)
{
  giza_mark_line_float(maxpt, npt, x, y);
}

/***************************************************************
 * cpgldev -- list available device types on standard output
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgldev(void)
{
  giza_print_device_list();
}

/***************************************************************
 * cpglen -- find length of a string in a variety of units
 * Status: IMPLEMENTED
 ***************************************************************/
void cpglen(int units, const char *string, float *xl, float *yl)
{
  giza_qtextlen_float(units_giza(units),string,xl,yl);
}

/***************************************************************
 * cpgline -- draw a polyline (curve defined by line-segments)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgline(int n, const float *xpts, const float *ypts)
{
  giza_line_float(n, xpts, ypts);
}

/***************************************************************
 * cpgmove -- move pen (change current pen position)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgmove(float x, float y)
{
  giza_move_float(x, y);
}

/***************************************************************
 * cpgmtxt -- write text at position relative to viewport
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgmtxt(const char *side, float disp, float coord, \
 float fjust, const char *text)
{
  giza_annotate_float(side, disp, coord, fjust, text);
}

/***************************************************************
 * cpgncur -- mark a set of points using the cursor
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgncur(int maxpt, int *npt, float *x, float *y, int symbol)
{
  giza_mark_points_ordered_float(maxpt, npt, x, y, symbol);
}

/***************************************************************
 * cpgnumb -- convert a number into a plottable character string
 * Status: IMPLEMENTED
 * Update: In order to prevent memory corruption by
 *         giza_format_number, assume that '*string_length', on
 *         entry, contains the maximum length of the string
 *         buffer allocated by the caller.
 *         PGPLOT c-binding does this, perl-PGPLOT5 does this
 *         and giza's F90 wrapper's been changed to do this
 ***************************************************************/
void cpgnumb(int mm, int pp, int form, char *string, \
 int *string_length)
{
  giza_format_number(mm, pp, form, string, *string_length);
  *string_length = strlen(string);
}

/***************************************************************
 * cpgolin -- mark a set of points using the cursor
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgolin(int maxpt, int *npt, float *x, float *y, int symbol)
{
  giza_mark_points_float(maxpt, npt, x, y, symbol);
}

/***************************************************************
 * cpgopen -- open a graphics device
 * Status: IMPLEMENTED
 ***************************************************************/
int cpgopen(const char *device)
{
  int pgopen = giza_open_device(device,"giza");
  if (pgopen > 0)
    {
      giza_set_colour_palette(GIZA_COLOUR_PALETTE_PGPLOT);
      giza_draw_background();
    }
  return pgopen;
}

/***************************************************************
 * cpgpage -- advance to new page
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpage(void)
{
  giza_change_page();
}

/***************************************************************
 * cpgpanl -- switch to a different panel on the view surface
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpanl(int nxc, int nyc)
{
  giza_set_panel(nxc, nyc);
}

/***************************************************************
 * cpgpap -- change the size of the view surface
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpap(float width, float aspect)
{

 giza_set_paper_size_float(GIZA_UNITS_INCHES,width,width*aspect);

}

/***************************************************************
 * cpgpixl -- draw pixels
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpixl(const int *ia, int idim, int jdim, int i1, int i2, \
 int j1, int j2, float x1, float x2, float y1, float y2)
{
  giza_draw_pixels_float(idim,jdim,ia,i1-1,i2-1,j1-1,j2-1,x1,x2,y1,y2,\
                         GIZA_EXTEND_NONE,GIZA_FILTER_NEAREST);
}

/***************************************************************
 * cpgpnts -- draw several graph markers, not all the same
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpnts(int n, const float *x, const float *y, \
 const int *symbol, int ns)
{
  int i;
  for (i = 0; i < n; i++)
    {
      int isym = (i < ns) ? symbol[i] : symbol[ns - 1];
      giza_single_point_float(x[i], y[i], isym);
    }
}

/***************************************************************
 * cpgpoly -- draw a polygon, using fill-area attributes
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpoly(int n, const float *xpts, const float *ypts)
{
  giza_polygon_float(n, xpts, ypts);
}

/***************************************************************
 * cpgpt -- draw several graph markers
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpt(int n, const float *xpts, const float *ypts, int symbol)
{
  giza_points_float(n, xpts, ypts, symbol);
}

/***************************************************************
 * cpgpt1 -- draw one graph marker
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgpt1(float xpt, float ypt, int symbol)
{
  giza_single_point_float(xpt, ypt, symbol);
}

/***************************************************************
 * cpgptxt -- write text at arbitrary position and angle
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgptxt(float x, float y, float angle, float fjust, \
 const char *text)
{
  giza_ptext_float(x, y, angle, fjust, text);
}

/***************************************************************
 * cpgqah -- inquire arrow-head style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqah(int *fs, float *angle, float *barb)
{
  giza_get_arrow_style_float(fs, angle, barb);
}

/***************************************************************
 * cpgqcf -- inquire character font
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqcf(int *font)
{
  /* pgfont set by call to cpgscf */
  *font = pgfont;
}

/***************************************************************
 * cpgqch -- inquire character height
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqch(float *size)
{
  giza_get_character_height_float(size);
}

/***************************************************************
 * cpgqci -- inquire color index
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqci(int *ci)
{
  giza_get_colour_index(ci);
}

/***************************************************************
 * cpgqcir -- inquire color index range
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqcir(int *icilo, int *icihi)
{
  giza_get_colour_index_range(icilo, icihi);
}

/***************************************************************
 * cpgqclp -- inquire clipping Status
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqclp(int *state)
{
  giza_get_clipping(state);
}

/***************************************************************
 * cpgqcol -- inquire color capability
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqcol(int *ci1, int *ci2)
{
  *ci1 = GIZA_COLOUR_INDEX_MIN;
  *ci2 = GIZA_COLOUR_INDEX_MAX;
}

/***************************************************************
 * cpgqcr -- inquire color representation
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqcr(int ci, float *cr, float *cg, float *cb)
{
  giza_get_colour_representation_float(ci,cr,cg,cb);
}

/***************************************************************
 * cpgqcs -- inquire character height in a variety of units
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqcs(int units, float *xch, float *ych)
{
  giza_get_character_size_float(units_giza(units),xch,ych);
}

/***************************************************************
 * cpgqdt -- inquire name of nth available device type
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqdt(int n, char *type, int *type_length, char *descr, \
 int *descr_length, int *inter)
{
  static const struct { const char *type; const char *descr; int inter; } devs[] = {
#ifdef _GIZA_HAS_XW
    {"/xw",   "X Window (interactive)", 1},
#endif
#ifdef _GIZA_HAS_OSXCOCOA
    {"/osx",  "macOS native window (interactive)", 1},
#endif
    {"/png",  "PNG file",               0},
    {"/pdf",  "PDF file",               0},
    {"/vpdf", "PDF file (portrait)",    0},
    {"/ps",   "PostScript file",        0},
    {"/vps",  "PostScript (portrait)",  0},
    {"/svg",  "SVG file",               0},
#ifdef _GIZA_HAS_EPS
    {"/eps",  "Encapsulated PS file",   0},
#endif
    {"/null", "Null device",            0},
  };
  static const int ndevs = sizeof(devs) / sizeof(devs[0]);

  /* pgplot uses 1-based indexing; n=0 returns ndevs */
  if (n < 1 || n > ndevs)
    {
      type[0] = '\0';
      *type_length = 0;
      descr[0] = '\0';
      *descr_length = 0;
      *inter = 0;
      return;
    }
  int idx = n - 1;
  /* Copy exactly strlen+1 bytes; avoids compiler optimizing a size-bounded
   * call into strncpy with trailing zero-padding that would clobber small
   * caller buffers. The pgplot cpgqdt API gives no buffer-size argument,
   * so the caller is responsible for supplying an adequately sized buffer;
   * our strings are short compile-time constants (max ~20 chars). */
  strcpy(type, devs[idx].type);
  *type_length = strlen(devs[idx].type);
  strcpy(descr, devs[idx].descr);
  *descr_length = strlen(devs[idx].descr);
  *inter = devs[idx].inter;
}

/***************************************************************
 * cpgqfs -- inquire fill-area style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqfs(int *fs)
{
  giza_get_fill(fs);
}

/***************************************************************
 * cpgqhs -- inquire hatching style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqhs(float *angle, float *sepn, float* phase)
{
  giza_get_hatching_style_float(angle,sepn,phase);
}

/***************************************************************
 * cpgqid -- inquire current device identifier
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqid(int *id)
{
  giza_get_device_id(id);
}

/***************************************************************
 * cpgqinf -- inquire pgplot general information
 * Status: NOT IMPLEMENTED
 ***************************************************************/
void cpgqinf(const char *item, char *value, int *value_length)
{
  if (!item || !value || !value_length || (value_length && !*value_length))
    {
      _giza_error("cgpqinf", "one or more NULL pointers item=%p value=%p value_length=%p", (void*)item, (void*)value, (void*)value_length);
      return;
    }
    if (!*value_length)
     {
        _giza_error("cgpqinf", "target string says it has length 0 querying '%s'", item);
        return;
     }
  const int last_char = *value_length - 1;
  /* check which item to query. do that case insensitive.
     this means that mixed case is also matched */
  if (strcasecmp(item, "VERSION")==0)
   {
      static char   giza_version_string[32] = {'\0',};
      /* Initialize only once */
      if (giza_version_string[0]=='\0' )
         snprintf(giza_version_string, sizeof(giza_version_string), "giza-%s", GIZA_VERSION_STRING);
      /* Copy at most *value_length-1 characters into value - note: strncpy(3)
         may leave value not-NUL terminated if *value_length < length of giza_version_string.
         Could have used strlcpy(3) but that is famously absent in glibc on Linux (need -lbsd
         to have it) so we tediously do it by hand */
      strncpy(value, giza_version_string, last_char);
   }
  else if (strcasecmp(item, "NOW")==0 )
   {
     /* In F90 calls date_and_time(...) which returns (surprisingly) date and time:
        (cf. https://gcc.gnu.org/onlinedocs/gfortran/DATE_005fAND_005fTIME.html)
        but PGPLOT reformats that to:
           "dd-mmm-yyyy hh:mm"
        so we mimic that behaviour */
     const time_t  now = time(NULL);

     /* Use strftime(3) to format current time correctly in one go */
     strftime(value, last_char, "%d-%b-%Y %H:%M", localtime(&now));
   }
  else
   {
     /* all other cases are handled by _giza_query_device() */
     giza_query_device(item, value, value_length);
   }

  /* terminate string and return length of result */
  value[last_char] = '\0';
  *value_length    = strlen(value);
}

/***************************************************************
 * cpgqitf -- inquire image transfer function
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqitf(int *itf)
{
       giza_get_image_transfer_function(itf);
}

/***************************************************************
 * cpgqls -- inquire line style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqls(int *ls)
{
  giza_get_line_style(ls);
}

/***************************************************************
 * cpgqlw -- inquire line width (as integer)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqlw(int *lw)
{
  double width;
  giza_get_line_width(&width);
  *lw = _giza_nint(width - 0.5);
}

/***************************************************************
 * cpgqndt -- inquire number of available device types
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqndt(int *n)
{
  /* Use cpgqdt to count: iterate until we get an empty type */
  char type[65];
  int tlen, dlen, inter;
  char descr[65];
  int count = 0;
  int i;
  for (i = 1; i < 100; i++)
    {
      cpgqdt(i, type, &tlen, descr, &dlen, &inter);
      if (tlen == 0) break;
      count++;
    }
  *n = count;
}

/***************************************************************
 * cpgqpos -- inquire current pen position
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqpos(float *x, float *y)
{
  giza_get_current_point_float(x, y);
}

/***************************************************************
 * cpgqtbg -- inquire text background color index
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqtbg(int *tbci)
{
  giza_get_text_background(tbci);
}

/***************************************************************
 * cpgqtxt -- find bounding box of text string
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqtxt(float x, float y, float angle, float fjust, \
 const char *text, float *xbox, float *ybox)
{
  giza_qtext_float(x, y, angle, fjust, text, xbox, ybox);
}

/***************************************************************
 * cpgqvp -- inquire viewport size and position
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqvp(int units, float *x1, float *x2, float *y1, float *y2)
{
  giza_get_viewport_float(units_giza(units), x1, x2, y1, y2);
}

/***************************************************************
 * cpgqvsz -- inquire size of view surface
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqvsz(int units, float *x1, float *x2, float *y1, float *y2)
{
 *x1 = 0.;
 *y1 = 0.;
 giza_get_paper_size_float(units_giza(units),x2,y2);
}

/***************************************************************
 * cpgqwin -- inquire window boundary coordinates
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgqwin(float *x1, float *x2, float *y1, float *y2)
{
  giza_get_window_float(x1,x2,y1,y2);
}

/***************************************************************
 * cpgrect -- draw a rectangle, using fill-area attributes
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgrect(float x1, float x2, float y1, float y2)
{
  giza_rectangle(x1,x2,y1,y2);
}

/***************************************************************
 * cpgrnd -- find the smallest `round' number greater than x
 * Status: IMPLEMENTED
 ***************************************************************/
float cpgrnd(float x, int *nsub)
{
  return giza_round_float(x, nsub);
}

/***************************************************************
 * cpgrnge -- choose axis limits
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgrnge(float x1, float x2, float *xlo, float *xhi)
{
  float dx = x2 - x1;
  *xlo = x1 - 0.1*dx;
  *xhi = x2 + 0.1*dx;
  if (*xlo < 0. && x1 >= 0.) *xlo = 0.;
  if (*xhi > 0. && x2 <= 0.) *xhi = 0.;
}

/***************************************************************
 * cpgsah -- set arrow-head style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsah(int fs, float angle, float barb)
{
  giza_set_arrow_style_float(fs,angle,barb);
}

/***************************************************************
 * cpgsave -- save pgplot attributes
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsave(void)
{
  giza_save();
}

/***************************************************************
 * cpgunsa -- restore pgplot attributes
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgunsa(void)
{
  giza_restore();
}

/***************************************************************
 * cpgscf -- set character font
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgscf(int font)
{
  switch(font)
  {
  case 4:
     giza_set_font("cursive");
     break;
  case 3:
     giza_set_font_italic("times");
     break;
  case 2:
     giza_set_font("times");
     break;
  default:
     giza_set_font("arial");
     break;
  }
  /*
   * set pgfont so that query calls to PGQCF are successful
   */
  pgfont = font;

}

/***************************************************************
 * cpgsch -- set character height
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsch(float size)
{
  giza_set_character_height_float(size);
}

/***************************************************************
 * cpgsci -- set color index
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsci(int ci)
{
  giza_set_colour_index(ci);
}

/***************************************************************
 * cpgscir -- set color index range
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgscir(int icilo, int icihi)
{
  giza_set_colour_index_range(icilo, icihi);
}

/***************************************************************
 * cpgsclp -- enable or disable clipping at edge of viewport
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsclp(int state)
{
  giza_set_clipping(state);
}

/***************************************************************
 * cpgscr -- set color representation
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgscr(int ci, float cr, float cg, float cb)
{
  giza_set_colour_representation_float(ci, cr, cg, cb);
}

/***************************************************************
 * cpgscrl -- scroll window
 * Status: IMPLEMENTED
 *
 * Scroll the window by dx in x and dy in y (world coordinates).
 * This adjusts the window range and redraws.
 ***************************************************************/
void cpgscrl(float dx, float dy)
{
  float x1, x2, y1, y2;
  cpgqwin(&x1, &x2, &y1, &y2);
  cpgswin(x1 + dx, x2 + dx, y1 + dy, y2 + dy);
}

/***************************************************************
 * cpgscrn -- set color representation by name
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgscrn(int ci, const char *name, int *ier)
{
  /* Hardcoded common color names (r, g, b in 0..1) */
  static const struct { const char *name; float r, g, b; } colors[] = {
    {"black",        0.0f, 0.0f, 0.0f},
    {"white",        1.0f, 1.0f, 1.0f},
    {"red",          1.0f, 0.0f, 0.0f},
    {"green",        0.0f, 1.0f, 0.0f},
    {"blue",         0.0f, 0.0f, 1.0f},
    {"cyan",         0.0f, 1.0f, 1.0f},
    {"magenta",      1.0f, 0.0f, 1.0f},
    {"yellow",       1.0f, 1.0f, 0.0f},
    {"orange",       1.0f, 0.65f, 0.0f},
    {"darkgray",     0.33f, 0.33f, 0.33f},
    {"darkgrey",     0.33f, 0.33f, 0.33f},
    {"lightgray",    0.75f, 0.75f, 0.75f},
    {"lightgrey",    0.75f, 0.75f, 0.75f},
    {"gray",         0.5f, 0.5f, 0.5f},
    {"grey",         0.5f, 0.5f, 0.5f},
    {"brown",        0.65f, 0.16f, 0.16f},
    {"pink",         1.0f, 0.75f, 0.80f},
    {"purple",       0.5f, 0.0f, 0.5f},
    {"darkgreen",    0.0f, 0.39f, 0.0f},
    {"skyblue",      0.53f, 0.81f, 0.92f},
    {NULL, 0, 0, 0}
  };

  /* Case-insensitive comparison */
  int i;
  for (i = 0; colors[i].name != NULL; i++)
    {
      if (strcasecmp(name, colors[i].name) == 0)
        {
          giza_set_colour_representation_float(ci, colors[i].r, colors[i].g, colors[i].b);
          *ier = 0;
          return;
        }
    }

  /* Try rgb.txt files */
  static const char *rgb_paths[] = {
    "/usr/share/X11/rgb.txt",
    "/opt/X11/share/X11/rgb.txt",
    "/etc/X11/rgb.txt",
    "/usr/lib/X11/rgb.txt",
    NULL
  };

  for (i = 0; rgb_paths[i] != NULL; i++)
    {
      FILE *fp = fopen(rgb_paths[i], "r");
      if (!fp) continue;
      char line[256];
      while (fgets(line, sizeof(line), fp))
        {
          int r, g, b;
          char cname[128];
          if (line[0] == '!' || line[0] == '#') continue;
          /* rgb.txt format: "R G B  name" */
          if (sscanf(line, "%d %d %d %127[^\n]", &r, &g, &b, cname) == 4)
            {
              /* Trim leading spaces from name */
              char *p = cname;
              while (*p == ' ' || *p == '\t') p++;
              if (strcasecmp(name, p) == 0)
                {
                  giza_set_colour_representation_float(ci, r / 255.0f, g / 255.0f, b / 255.0f);
                  *ier = 0;
                  fclose(fp);
                  return;
                }
            }
        }
      fclose(fp);
    }

  *ier = 1;
}

/***************************************************************
 * cpgsfs -- set fill-area style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsfs(int fs)
{
  giza_set_fill(fs);
}

/***************************************************************
 * cpgshls -- set color representation using hls system
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgshls(int ci, float ch, float cl, float cs)
{
  giza_set_colour_representation_hls_float(ci, ch, cl, cs);
}

/***************************************************************
 * cpgshs -- set hatching style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgshs(float angle, float sepn, float phase)
{
  giza_set_hatching_style_float(angle, sepn, phase);
}

/***************************************************************
 * cpgsitf -- set image transfer function
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsitf(int itf)
{
       giza_set_image_transfer_function(itf);
}

/***************************************************************
 * cpgslct -- select an open graphics device
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgslct(int id)
{
  giza_select_device(id);
}

/***************************************************************
 * cpgsls -- set line style
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsls(int ls)
{
  giza_set_line_style(ls);
}

/***************************************************************
 * cpgslw -- set line width
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgslw(int lw)
{
  giza_set_line_width(lw);
}

/***************************************************************
 * cpgstbg -- set text background color index
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgstbg(int tbci)
{
  giza_set_text_background(tbci);
}

/***************************************************************
 * cpgsubp -- subdivide view surface into panels
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsubp(int nxsub, int nysub)
{
  giza_subpanel(nxsub, nysub);
}

/***************************************************************
 * cpgsvp -- set viewport (normalized device coordinates)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgsvp(float xleft, float xright, float ybot, float ytop)
{
  giza_set_viewport_float(xleft,xright,ybot,ytop);
}

/***************************************************************
 * cpgswin -- set window
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgswin(float x1, float x2, float y1, float y2)
{
  giza_set_window_float(x1, x2, y1, y2);
}

/***************************************************************
 * cpgtbox -- draw frame and write (dd) hh mm ss.s labelling
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgtbox(const char *xopt, float xtick, int nxsub, \
 const char *yopt, float ytick, int nysub)
{
  giza_box_time_float(xopt, xtick, nxsub, yopt, ytick, nysub);
}

/***************************************************************
 * cpgtext -- write text (horizontal, left-justified)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgtext(float x, float y, const char *text)
{
  giza_text_float(x, y, text);
}

/***************************************************************
 * cpgtick -- draw a single tick mark on an axis
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgtick(float x1, float y1, float x2, float y2, float v, \
 float tikl, float tikr, float disp, float orient, const char *str)
{
   giza_tick_float(x1,y1,x2,y2,v,tikl,tikr,disp,orient,str);
}

/***************************************************************
 * cpgupdt -- update display
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgupdt(void)
{
  giza_flush_buffer();
}

/***************************************************************
 * cpgvect -- vector map of a 2D data array, with blanking
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgvect(const float *a, const float *b, int idim, int jdim, \
 int i1, int i2, int j1, int j2, float c, int nc, \
 const float *tr, float blank)
{
  float affine[6];
  convert_tr_to_affine(tr,affine);
  giza_vector_float(idim,jdim,a,b,i1-1,i2-1,j1-1,j2-1,c,nc,affine,blank);
}

/***************************************************************
 * cpgvsiz -- set viewport (inches)
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgvsiz(float xleft, float xright, float ybot, float ytop)
{
  giza_set_viewport_inches_float(xleft, xright, ybot, ytop);
}

/***************************************************************
 * cpgvstd -- set standard (default) viewport
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgvstd(void)
{
  giza_set_viewport_default();
}

/***************************************************************
 * cpgwedg -- annotate an image plot with a wedge
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgwedg(const char *side, float disp, float width, \
 float fg, float bg, const char *label)
{
  giza_colour_bar_float(side,disp,width,bg,fg,label);
}

/***************************************************************
 * cpgwnad -- set window and adjust viewport to same aspect ratio
 * Status: IMPLEMENTED
 ***************************************************************/
void cpgwnad(float x1, float x2, float y1, float y2)
{
  giza_set_window_equal_scale_float(x1, x2, y1, y2);
}
