#include <giza.h>
#include "math.h"
static double sinx(double *x);

int main() {
 int ierr, i;
 ierr = giza_open_device("/png","test");
 for (i=1;i<1100;i++) {
     giza_set_environment(0.,1.,-1.,1.,0,0);
     giza_label("x","y","title");
     giza_function_x(sinx,100,0.,1.,1);
 }
 giza_close_device();

 return ierr;
}

double sinx(double *x) {
   return sin(6.28*(*x));
}
