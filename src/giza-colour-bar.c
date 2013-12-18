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
#include <giza.h>
#include <string.h>

/**
 * Drawing: giza_colour_bar
 *
 * Synopsis: Draws a colour bar (wedge) using the current colour ramp
 *
 * Input:
 *  -side    :- edge of viewport to draw colour bar relative to, either 'B' (bottom), 'T' (top), 'L' (left) or 'R' (right)
 *  -disp    :- displacement of the bar in character heights from the specified edge
 *  -width   :- width of the colour bar in character heights
 *  -valMin  :- The value in data that gets assigned the colour corresponding to zero on the
 *              colour ramp (The ramp is set by giza_set_colour_table)
 *  -valMax  :- The value in data that gets assigned the colour corresponding to one on the
 *              colour ramp
 *  -label   :- Text label to annotate colour bar with
 *
 * See Also: giza_render, giza_colour_bar_float, giza_set_colour_table
 */
void
giza_colour_bar (const char *side, double disp, double width, 
                 double valMin, double valMax, const char *label)
{
  /* query current window, viewport and character height settings */
  double xmin,xmax,ymin,ymax;
  double vptxmin,vptxmax,vptymin,vptymax;
  double xch,ych;
  giza_get_window(&xmin,&xmax,&ymin,&ymax);
  giza_get_viewport(GIZA_UNITS_NORMALIZED,&vptxmin,&vptxmax,&vptymin,&vptymax);
  giza_get_character_size(GIZA_UNITS_NORMALIZED,&xch,&ych);

  /* setup affine matrix for call to giza_render */
  double affine[6];
  affine[0] = 1.0;
  affine[1] = 0.0;
  affine[2] = 0.0;
  affine[3] = 1.0;
  affine[4] = 0.0;
  affine[5] = 0.0;

  /* fill an array with values from valMin to valMax */
  int npixwedg = 400;
  double sample[npixwedg];
  double vmin,vmax;
  if (valMax < valMin) 
    {
      vmin = valMax;
      vmax = valMin;
    }
  else
    {
      vmin = valMin;
      vmax = valMax;
    }
  double dx = (vmax - vmin)/((double) (npixwedg-1));
  
  int i;
  for (i = 0; i < npixwedg; i++)
     {
        sample[i] = vmin + i*dx;
     }

  double vptxmini = vptxmin;
  double vptxmaxi = vptxmax;
  double vptymini = vptymin;
  double vptymaxi = vptymax;
  
  int bottom = 0, top = 0, left = 0, right = 0;
  if (strchr(side,'B') || strchr(side,'b'))      { bottom = 1; }
  else if (strchr(side,'T') || strchr(side,'t')) { top = 1; }
  else if (strchr(side,'L') || strchr(side,'l')) { left = 1; }
  else if (strchr(side,'R') || strchr(side,'r')) { right = 1; }
  
  int greyscale = 0;
  if (strchr(side,'G') || strchr(side,'g')) { greyscale = 1; }
  
  if (bottom || top) 
    {
       if (bottom)
         { 
           vptymaxi = vptymini - disp*ych;
           vptymini = vptymaxi - width*xch;
         }
       else
         {
           vptymini = vptymaxi + disp*ych;
           vptymaxi = vptymini + width*xch;         
         }

       giza_set_viewport(vptxmini,vptxmaxi,vptymini,vptymaxi);
       giza_set_window(1.0,(double) npixwedg, 0.0, 1.0);
       if (greyscale)
         {
           giza_render_gray(npixwedg,1,sample,0,npixwedg-1,0,0,valMin,valMax,
                       GIZA_EXTEND_PAD,affine);
         }
       else
         {
           giza_render(npixwedg,1,sample,0,npixwedg-1,0,0,valMin,valMax,
                       GIZA_EXTEND_PAD,affine);         
         }

       giza_set_window(vmin,vmax,0.0,1.0);

       if (bottom)
         {
           giza_box("BCNST",0.0,0,"BC",0.0,0);
         }
       else
         {
           giza_box("BCMST",0.0,0,"BC",0.0,0);         
         }
    }
  else if (left || right)
    {
       if (left)
         { 
           vptxmaxi = vptxmini - disp*xch;
           vptxmini = vptxmaxi - width*xch;
         }
       else
         {
           vptxmini = vptxmaxi + disp*xch;
           vptxmaxi = vptxmini + width*xch;
         }

       giza_set_viewport(vptxmini,vptxmaxi,vptymini,vptymaxi);
       giza_set_window(0.0, 1.0, 1.0,(double) npixwedg);
       if (greyscale)
         {
           giza_render_gray(1,npixwedg,sample,0,0,0,npixwedg-1,valMin,valMax,
                       GIZA_EXTEND_PAD,affine);
         }
       else
         {
           giza_render(1,npixwedg,sample,0,0,0,npixwedg-1,valMin,valMax,
                       GIZA_EXTEND_PAD,affine);         
         }

       giza_set_window(0.0,1.0,vmin,vmax);

       if (left)
         {
           giza_box("BC",0.0,0,"BCNST",0.0,0);
         }
       else
         {
           giza_box("BC",0.0,0,"BCMST",0.0,0);         
         }
    }

  giza_annotate(side,2.8,1.0,1.0,label);

  /* reset window and viewport */
  giza_set_viewport(vptxmin,vptxmax,vptymin,vptymax);
  giza_set_window(xmin,xmax,ymin,ymax);

  giza_flush_device ();
}

/**
 * Drawing: giza_colour_bar_float
 *
 * Synopsis: Same functionality as giza_colour_bar, but takes floats
 *
 * See Also: giza_colour_bar
 */
void
giza_colour_bar_float (const char *side, float disp, float width, 
                       float valMin, float valMax, const char *label)
{
  giza_colour_bar (side, (double) disp, (double) width,
                   (double) valMin, (double) valMax, label);
}
