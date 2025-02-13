/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
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
 */
#include <giza.h>
#include "math.h"
#include <unistd.h>
#include <stdio.h>
static double sinx(double *x);
int check_pdf_file(void);

int main() {
 int id, i;
 id = giza_open_device("/pdf","test");
 for (i=1;i<=10;i++) {
     giza_set_environment(0.,1.,-1.,1.,0,0);
     giza_label("x","y","title");
     giza_function_x(sinx,100,0.,1.,1);
 }
 giza_close_device();

 /* open_device should return a positive integer */
 if (id <= 0 || !check_pdf_file()) {
   return 1;
 } else {
   return 0;
 }
}

double sinx(double *x) {
   return sin(6.28*(*x));
}

/* check if the corresponding files exist */
int check_pdf_file() {
    char filename[20];
    snprintf(filename, sizeof(filename), "test.pdf");
    /* Check if the file exists */
    if (access(filename, F_OK) == -1) {
       fprintf(stderr, "Error: Required file %s does not exist.\n", filename);
       return 0; /* Return 0 if file is missing */
    } else {
       fprintf(stdout, "File %s exists -> OK\n", filename);
    }
    return 1; /* Return 1 if file exists */
}
