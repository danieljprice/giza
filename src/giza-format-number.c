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

#include "giza-io-private.h"
#include "giza-private.h"
#include <giza.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*static void _giza_tidy_expo (char *from, int n, char *to); */

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

  /* return "0" */
  if (mantissa == 0)
    {
      strcpy (string, "0");
      return;
    }

  /* Check if mantissa is negative, and make it positive */
  minus = mantissa < 0;
  mantissa = abs (mantissa);

  /* Convert the mantissa to a string, and count the number of digits */
  sprintf (tmp, "%i", mantissa);
  numDigits = strlen (tmp);

  /* Remove zeros on the right increasing power as needed */
  while (tmp[numDigits - 1] == '0')
    {
      --numDigits;
      tmp[numDigits] = '\0';
      ++power;
    }
  /* Work out how many digits before the decimal */
  digitsBeforePoint = numDigits + MIN (power, 0);

  /* Integers of four or less digits (or forced integers) */
  if ((power >= 0) && ((form == GIZA_NUMBER_FORMAT_AUTO && (power + numDigits) <= 4) || (form == GIZA_NUMBER_FORMAT_DEC && power + numDigits <= 10)))
    {
      for (; power > 0; --power)
        {
	  tmp[numDigits] = '0';
	  ++numDigits;
	}
    }
  /* Decimal without a power */
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
  /* Otherwise the point goes after first digit and adjust power */
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
  /* Put in a null character */
  tmp[numDigits] = '\0';

  /* Add in the exponent */
  if (power != 0)
    {
      sprintf (&tmp[numDigits], "\\times10^{%i}", power);
    }

  /* Add leading minus sign and copy the result to output */
  string[0] = '\0';
  if (minus)
    {
      string[0] = '-';
      string[1] = '\0';
    }
  strcat (string, tmp);
}

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
/*static void */
/*_giza_tidy_expo (char *from, int n, char *to) */
/*{ */
/*  int i = 0; */
//  /* tidy the bit before 'E' */
//  /* remove sign if it is + */
/*  if (from[0] == '-') */
/*    { */
/*      to[i] = '-'; */
/*      i++; */
/*    } */
/* */
//  /* copy the digit before the decimal */
/*  to[i] = from[1]; */
/*  i++; */
/* */
//  /* remove trailing zeros */
//  /* find how many non-zero digits to copy */
/*  int j, stop = 0; */
/*  for (j = n + 1; !stop; j--) */
/*    { */
/*      if (from[j] != '0') */
/*	stop = 1; */
/*    } */
/*  j++; */
/* */
//  /* if there are digits after the point add the point and the digits */
/*  if (from[j] != '.') */
/*    { */
/*      to[i] = '.'; */
/*      i++; */
/*      int k = 3; */
/*      for (; i < j; i++) */
/*	{ */
/*	  to[i] = from[k]; */
/*	  k++; */
/*	} */
/*    } */
/* */
/* */
//  /* add * 10 ^ */
/*  to[i] = '\\'; */
/*  i++; */
/*  to[i] = 't'; */
/*  i++; */
/*  to[i] = 'i'; */
/*  i++; */
/*  to[i] = 'm'; */
/*  i++; */
/*  to[i] = 'e'; */
/*  i++; */
/*  to[i] = 's'; */
/*  i++; */
/*  to[i] = '1'; */
/*  i++; */
/*  to[i] = '0'; */
/*  i++; */
/*  to[i] = '^'; */
/*  i++; */
/*  to[i] = '{'; */
/*  i++; */
/* */
//  /* tidy the bit after 'E' */
//  /* remove sign if + */
/*  if (from[n + 3] == '-') */
/*    { */
/*      to[i] = '-'; */
/*      i++; */
/*    } */
/* */
//  /* case where power has 3 digits */
/*  if (from[n + 6] != '\0') */
/*    { */
/*      to[i] = from[n + 4]; */
/*      i++; */
/*      to[i] = from[n + 5]; */
/*      i++; */
/*      to[i] = from[n + 6]; */
/*      i++; */
/*    } */
//  /* if two digits */
/*  else */
/*    { */
//      /* if extra 0 in power */
/*      if (from[n + 4] != '0') */
/*	{ */
/*	  to[i] = from[n + 4]; */
/*	  i++; */
/*	} */
/*      to[i] = from[n + 5]; */
/*      i++; */
/*    } */
/*  to[i] = '}'; */
/*  i++; */
/*  to[i] = '\0'; */
/*} */
