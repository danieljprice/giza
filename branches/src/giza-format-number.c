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
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
 */

#include "giza-shared-cpp.h"
#include "giza-io-private.h"
#include "giza-private.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//static void _giza_tidy_expo (char *from, int n, char *to);

/**
 * Utility: giza_format_number
 *
 * Synopsis: Formats a floating point number to a string. str should be of length 16 + number of digits.
 *
 * input:
 *  -mantissa :- The mantissa of the number
 *  -power    :- The power of the number
 *  -form     :- Determine which form to use, see below 
 *  -string   :- Gets set to the string
 *
 * Forms:
 *  -GIZA_NUMBER_FORMAT_AUTO :- giza decides
 *  -GIZA_NUMBER_FORMAT_DEC  :- don't use an exponent, unless there are more than ten signifigant figures
 *  -GIZA_NUMBER_FORMAT_EXP  :- Use an exponent
 */
void
giza_format_number (int mantissa, int power, int form, char *string)
{
  int minus, numDigits, digitsBeforePoint;
  char tmp[100];

  // return "0"
  if (mantissa == 0)
    {
      strcpy (string, "0");
      return;
    }

  // Check if mantissa is negative, and make it positive
  minus = mantissa < 0;
  mantissa = abs (mantissa);

  // Convert the mantissa to a string, and count the number of digits
  sprintf (tmp, "%i", mantissa);
  numDigits = strlen (tmp);

  // Remove zeros on the right increasing power as needed
  while (tmp[numDigits - 1] == '0')
    {
      --numDigits;
      tmp[numDigits] = '\0';
      ++power;
    }
  // Work out how many digits before the decimal
  digitsBeforePoint = numDigits + MIN (power, 0);

  // Integers of four or less digits (or forced integers)
  if ((power >= 0) && ((form == GIZA_NUMBER_FORMAT_AUTO && (power + numDigits) <= 4) || (form == GIZA_NUMBER_FORMAT_DEC && power + numDigits <= 10)))
    {
      for (; power > 0; --power)
        {
	  tmp[numDigits] = '0';
	  ++numDigits;
	}
    }
  // Decimal without a power
  else if (form != GIZA_NUMBER_FORMAT_EXP && digitsBeforePoint >= 1 && digitsBeforePoint <= 4 && digitsBeforePoint < numDigits)
    {
      int i;
      for (i = numDigits; i >= digitsBeforePoint; --i)
        {
	  tmp[i+1] = tmp[i];
	}
      tmp[i+1] = '.';
      ++numDigits;
      power = 0;
    }
  // Otherwise the point goes after first digit and adjust power
  else
    {
      power = power + numDigits - 1;
      if (form != GIZA_NUMBER_FORMAT_EXP && power == -1)
        {
	  int i;
	  for (i = numDigits; i >= 0; --i)
	    {
	      tmp[i+1] = tmp[i];
	    }
	  tmp[0] = '0';
	  power = 0;
	  ++numDigits;
	}
      else if (form != GIZA_NUMBER_FORMAT_EXP && power == -2)
        {
	  int i;
	  for (i = numDigits; i >= 0; --i)
	    {
	      tmp[i+2] = tmp[i];
	    }
	  tmp[0] = '0';
	  tmp[1] = '0';
	  power = 0;
	  numDigits += 2;
	}
      if (numDigits > 1)
        {
	  int i;
	  for (i = numDigits; i > 0; --i)
	    {
	      tmp[i+1] = tmp[i];
	    }
	  tmp[1] = '.';
	  ++numDigits;
	}
    }
  // Put in a null character
  tmp[numDigits] = '\0';

  // Add in the exponent
  if (power != 0)
    {
      sprintf (&tmp[numDigits], "\\times10^{%i}", power);
    }

  // Add leading minus sign and copy the result to output
  string[0] = '\0';
  if (minus)
    {
      string[0] = '-';
      string[1] = '\0';
    }
  strcat (string, tmp);
}

/*
 * Utility: giza_format_number
 *
 * Synopsis: Formats a floating point number to a string. str should be of length 16 + n.
 *
 * input:
 *  -x :- The number to be formatted
 *  -n :- The number of significant figures
 *  -str :- Gets set to the string
 */
/*
void
giza_format_number (double x, int n, char *str)
{
  if (n < 1)
    {
      //_giza_warning ("giza_format_number",
      //               "Invalid number of significant figures, setting n = 2");
      n = 2;
    }

  // return "0"
  if (fabs (x) < GIZA_ZERO_DOUBLE)
    strcpy (str, "0");
  // if the number is less than 0.01
  else if (fabs (x) < 1e-2)
    {
      // if it is a multiple of 0.001 print as decimal
      if (fabs (x) >= 0.001 && fmod (x, 0.001) < 0.0001)
	{
	  sprintf (str, "%.3f", x);
	}
      // otherwise print as exponential
      else
	{
	  char tmp[20+n]; // make sure string is big enough for sig figures
	  sprintf (tmp, "%+.*E", n - 1, x);
          //printf("exponential = %s %i \n",tmp,n);
	  _giza_tidy_expo (tmp, n, str);
	}
    }
  // if the number is less than 100000 print it as a decimal
  else if (fabs (x) < 1e5)
    {
      //printf("decimal = %f \n",x);
      int n2 = n - ((int) log10 (fabs(x))) - 1;
      sprintf (str, "%.*f", n2, x);
      int i;
      for (i = strlen (str) - 1; str[i] == '0'; i--)
	{
	  str[i] = '\0';
	}
      if (str[i] == '.')
	str[i] = '\0';
    }
  // print expo
  else
    {
      char tmp[20+n];
      sprintf (tmp, "%+.*E", n - 1, x);
      _giza_tidy_expo (tmp, n, str);
    }
}
*/
/**
 * Formats a floating point number to a string, in a fasion for log ticks
 */
/*
void
giza_format_number_log (double power, char *str)
{
  if (fabs (power) < GIZA_ZERO_DOUBLE)
    {
       strcpy (str, "1");
       return;
    }

  if (fabs (power1.) )
    {
      char tmp[100];
      sprintf (tmp, "%s%1f%s", "10^{", power, "}");
      strcpy (str, "tmp");
      return;
    }
}
*/

/**
 * 'Cleans up' the exponential part of a string representation of a float.
 * Removes unnecessary '+' and '0' and converts E to \times10^{. 
 */
//static void
//_giza_tidy_expo (char *from, int n, char *to)
//{
//  int i = 0;
//  // tidy the bit before 'E'
//  // remove sign if it is +
//  if (from[0] == '-')
//    {
//      to[i] = '-';
//      i++;
//    }
//
//  // copy the digit before the decimal
//  to[i] = from[1];
//  i++;
//
//  // remove trailing zeros
//  // find how many non-zero digits to copy
//  int j, stop = 0;
//  for (j = n + 1; !stop; j--)
//    {
//      if (from[j] != '0')
//	stop = 1;
//    }
//  j++;
//
//  // if there are digits after the point add the point and the digits
//  if (from[j] != '.')
//    {
//      to[i] = '.';
//      i++;
//      int k = 3;
//      for (; i < j; i++)
//	{
//	  to[i] = from[k];
//	  k++;
//	}
//    }
//
//
//  // add * 10 ^
//  to[i] = '\\';
//  i++;
//  to[i] = 't';
//  i++;
//  to[i] = 'i';
//  i++;
//  to[i] = 'm';
//  i++;
//  to[i] = 'e';
//  i++;
//  to[i] = 's';
//  i++;
//  to[i] = '1';
//  i++;
//  to[i] = '0';
//  i++;
//  to[i] = '^';
//  i++;
//  to[i] = '{';
//  i++;
//
//  // tidy the bit after 'E'
//  // remove sign if +
//  if (from[n + 3] == '-')
//    {
//      to[i] = '-';
//      i++;
//    }
//
//  // case where power has 3 digits
//  if (from[n + 6] != '\0')
//    {
//      to[i] = from[n + 4];
//      i++;
//      to[i] = from[n + 5];
//      i++;
//      to[i] = from[n + 6];
//      i++;
//    }
//  // if two digits
//  else
//    {
//      // if extra 0 in power
//      if (from[n + 4] != '0')
//	{
//	  to[i] = from[n + 4];
//	  i++;
//	}
//      to[i] = from[n + 5];
//      i++;
//    }
//  to[i] = '}';
//  i++;
//  to[i] = '\0';
//}