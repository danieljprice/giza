/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
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
 *      Marjolein Verkouter <verkouter@jive.eu>
 */
#include "giza-itf.h"
#include "giza-io-private.h"
#include "giza-private.h"
#include <giza.h>
#include <math.h>


double _giza_itf_linear(const double pixelvalue, const double vmin, const double vmax);
double _giza_itf_log(const double pixelvalue, const double vmin, const double vmax);
double _giza_itf_sqrt(const double pixelvalue, const double vmin, const double vmax);

float  _giza_itf_linear_f(const float pixelvalue, const float vmin, const float vmax);
float  _giza_itf_log_f(const float pixelvalue, const float vmin, const float vmax);
float  _giza_itf_sqrt_f(const float pixelvalue, const float vmin, const float vmax);

int    _giza_itf_idx_linear(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax);
int    _giza_itf_idx_log(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax);
int    _giza_itf_idx_sqrt(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax);

int     _giza_itf_idx_linear_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax);
int     _giza_itf_idx_log_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax);
int     _giza_itf_idx_sqrt_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax);

/* We keep an array of three image transfer functions, use set_image_transfer_function to set
	0: linear
	1: log
	2: sqrt 
*/
giza_itf_type       giza_itf[3]       = {_giza_itf_linear,       _giza_itf_log,       _giza_itf_sqrt};
giza_itf_type_f     giza_itf_f[3]     = {_giza_itf_linear_f,     _giza_itf_log_f,     _giza_itf_sqrt_f};
giza_itf_idx_type   giza_itf_idx[3]   = {_giza_itf_idx_linear,   _giza_itf_idx_log,   _giza_itf_idx_sqrt};
giza_itf_idx_type_f giza_itf_idx_f[3] = {_giza_itf_idx_linear_f, _giza_itf_idx_log_f, _giza_itf_idx_sqrt_f};


void giza_set_image_transfer_function(int itf) {
  if (!_giza_check_device_ready ("giza_set_image_transfer_function"))
    return;
  if (itf < 0 || itf > 2)
    {
      _giza_warning ("giza_set_image_transfer_function",
		    "Invalid image transfer function, not set");
      return;
    }
  Dev[id].itf = itf;
}

void giza_get_image_transfer_function(int* itfp) {
  if (!_giza_check_device_ready ("giza_get_image_transfer_function"))
    return;
  *itfp = Dev[id].itf;
}


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

/* The linear transfer functions */

/* returns position of pixelvalue as fraction of the distance between vmin, vmax */
double _giza_itf_linear(const double pixelvalue, const double vmin, const double vmax) {
    return (vmax > vmin) ? (MIN(vmax, MAX(vmin, pixelvalue))-vmin)/(vmax-vmin) : (MIN(vmin, MAX(vmax, pixelvalue))-vmax)/(vmin-vmax);
}

/* maps pixelvalue's fractional position between vmin, vmax to corresponding index between cimin, cimax */
int  _giza_itf_idx_linear(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax) {
    const double fractional_pos = _giza_itf_linear(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int) round(((cimin < cimax) ? (cimax-cimin) : (cimin - cimax)) * fractional_pos);
}

/* id. for floats */
float _giza_itf_linear_f(const float pixelvalue, const float vmin, const float vmax) {
    return (vmax > vmin) ? (MIN(vmax, MAX(vmin, pixelvalue))-vmin)/(vmax-vmin) : (MIN(vmin, MAX(vmax, pixelvalue))-vmax)/(vmin-vmax);
}

int  _giza_itf_idx_linear_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax) {
    const float fractional_pos = _giza_itf_linear_f(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int)roundf(((cimin < cimax) ? (cimax-cimin) : (cimin - cimax)) * fractional_pos);
}

/* The log transfer functions */
const double sfac    = 65000.0;

/* returns position of pixelvalue as fraction of the distance between vmin, vmax */
double _giza_itf_log(const double pixelvalue, const double vmin, const double vmax) {
    double sfacl = log(1.0+sfac);
    return log(1.0+sfac*fabs(((vmax > vmin) ? (MIN(vmax, MAX(vmin, pixelvalue))-vmin) : (MIN(vmin, MAX(vmax, pixelvalue))-vmax))/(vmax-vmin)))/sfacl;
}

/* maps pixelvalue's fractional position between vmin, vmax to corresponding index between cimin, cimax */
int  _giza_itf_idx_log(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax) {
    const double fractional_pos = _giza_itf_log(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int)round(((cimax > cimin) ? (cimax-cimin) : (cimin-cimax)) * fractional_pos);
}

/* id. for floats */
const double sfac_f  = 65000.0f;

float _giza_itf_log_f(const float pixelvalue, const float vmin, const float vmax) {
    float sfacl_f = logf(1.0f+sfac);
    return log(1.0f+sfac_f*fabsf(((vmax > vmin) ? (MIN(vmax, MAX(vmin, pixelvalue))-vmin) : (MIN(vmin, MAX(vmax, pixelvalue))-vmax))/(vmax-vmin)))/sfacl_f;
}

int  _giza_itf_idx_log_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax) {
    const float fractional_pos = _giza_itf_log_f(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int)roundf(((cimax > cimin) ? (cimax-cimin) : (cimin-cimax)) * fractional_pos);
}

/* and the sqrt versions */

/* returns position of pixelvalue as fraction of the distance between vmin, vmax */
double _giza_itf_sqrt(const double pixelvalue, const double vmin, const double vmax) {
    return sqrt( _giza_itf_linear(pixelvalue, vmin, vmax) );
}

/* maps pixelvalue's fractional position between vmin, vmax to corresponding index between cimin, cimax */
int  _giza_itf_idx_sqrt(const double pixelvalue, const double vmin, const double vmax, const int cimin, const int cimax) {
    const double fractional_pos = _giza_itf_sqrt(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int)round(((cimax > cimin) ? (cimax-cimin) : (cimin-cimax)) * fractional_pos);
}

/* id. for floats */
float _giza_itf_sqrt_f(const float pixelvalue, const float vmin, const float vmax) {
    return sqrtf( _giza_itf_linear_f(pixelvalue, vmin, vmax) );
}

int  _giza_itf_idx_sqrt_f(const float pixelvalue, const float vmin, const float vmax, const int cimin, const int cimax) {
    const float fractional_pos = _giza_itf_sqrt_f(pixelvalue, vmin, vmax);
    return MIN(cimin, cimax) + (int)roundf(((cimax > cimin) ? (cimax-cimin) : (cimin-cimax)) * fractional_pos);
}
