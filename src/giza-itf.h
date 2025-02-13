/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the GPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 *      Marjolein Verkouter <verkouter@jive.eu>
 */
#ifndef GIZA_ITF_H
#define GIZA_ITF_H

/* support image transfer functions */

/*
 * Lifted from PGPLOT grimg2.f on how to handle the image transfer function:
	
	SFAC  = 65000.0
	SFACL = LOG(1.0+SFAC)
	...
        IF (MODE.EQ.0) THEN
        	IV = NINT((MININD*(A2-AV) + MAXIND*(AV-A1))/(A2-A1))
        ELSE IF (MODE.EQ.1) THEN
        	IV = MININD + NINT((MAXIND-MININD)*
               			   LOG(1.0+SFAC*ABS((AV-A1)/(A2-A1)))/SFACL)
        ELSE IF (MODE.EQ.2) THEN
        	IV = MININD + NINT((MAXIND-MININD)*
                                   SQRT(ABS((AV-A1)/(A2-A1))))
        ELSE
        	IV = MININD
*/

typedef double (*giza_itf_type)(const double pixelvalue, const double vmin, const double vmax);
typedef float  (*giza_itf_type_f)(const float pixelvalue, const float vmin, const float vmax);
typedef int    (*giza_itf_idx_type)(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax);
typedef int    (*giza_itf_idx_type_f)(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax);

/* We keep an array of three image transfer functions, use set_image_transfer_function to set
	0: linear
	1: log
	2: sqrt 
*/
extern giza_itf_type       giza_itf[3];
extern giza_itf_type_f     giza_itf_f[3];
extern giza_itf_idx_type   giza_itf_idx[3];
extern giza_itf_idx_type_f giza_itf_idx_f[3];

#endif
