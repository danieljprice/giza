/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2013 Daniel Price
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

#include <giza.h>

/**
 * Settings: giza_version
 *
 * Synopsis: Returns the giza version.
 * Note that version information can also be
 * obtained using the header variables GIZA_VERSION_STRING,
 * GIZA_VERSION_MAJOR, GIZA_VERSION_MINOR and GIZA_VERSION_MICRO.
 * These are available in the Fortran interface also.
 *
 * Output:
 *  -major :- major version number
 *  -minor :- minor version number
 *  -micro :- micro version number
 *
 */
void
giza_version (int *major, int *minor, int *micro)
{
  *major = GIZA_VERSION_MAJOR;
  *minor = GIZA_VERSION_MINOR;
  *micro = GIZA_VERSION_MICRO;
}
