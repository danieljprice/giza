#include <giza.h>

int main () 
{
  int ierr;
  ierr = giza_open_device("?","test");
  giza_stop_prompting();
  giza_close_device();

  ierr = giza_open_device("/xw","test");
  giza_stop_prompting();
  giza_close_device();

}
