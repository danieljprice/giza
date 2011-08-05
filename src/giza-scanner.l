/* A re-entrant scanner to recursively print text */

/* Specify that it is re-entrant and doesn't use ywrap */
%option reentrant noyywrap

/* Include the header and define tokens */

%{
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
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */
#include "giza-private.h"
#include "giza-transforms-private.h"
#include "giza-drivers-private.h"
#include "giza-text-private.h"
#include "giza.h"
#include <string.h>
#include <math.h>

#define GIZA_TOKEN_ERROR  0
#define GIZA_TOKEN_END    1
#define GIZA_TOKEN_SUPER  2
#define GIZA_TOKEN_SUB    3
#define GIZA_TOKEN_OB     4
#define GIZA_TOKEN_CB     5
#define GIZA_TOKEN_TIMES  6
#define GIZA_TOKEN_OTHER  7
#define GIZA_TOKEN_RAISE  8
#define GIZA_TOKEN_LOWER  9
#define GIZA_TOKEN_INTEGRAL 10
#define GIZA_TOKEN_NABLA 11

#define GIZA_TOKEN_FONT_NORMAL 12
#define GIZA_TOKEN_FONT_ROMAN 13
#define GIZA_TOKEN_FONT_ITALIC 14
#define GIZA_TOKEN_FONT_SCRIPT 15

#define GIZA_TOKEN_ALPHA   21
#define GIZA_TOKEN_BETA    22
#define GIZA_TOKEN_GAMMA   23
#define GIZA_TOKEN_DELTA   24
#define GIZA_TOKEN_EPSILON 25
#define GIZA_TOKEN_ZETA    26
#define GIZA_TOKEN_ETA     27
#define GIZA_TOKEN_THETA   28
#define GIZA_TOKEN_IOTA    29
#define GIZA_TOKEN_KAPPA   30
#define GIZA_TOKEN_LAMBDA  31
#define GIZA_TOKEN_MU      32
#define GIZA_TOKEN_NU      33
#define GIZA_TOKEN_XI      34
#define GIZA_TOKEN_OMICRON 35
#define GIZA_TOKEN_PI      36
#define GIZA_TOKEN_RHO     37
#define GIZA_TOKEN_SIGMA   38
#define GIZA_TOKEN_TAU     39
#define GIZA_TOKEN_UPSILON 40
#define GIZA_TOKEN_PHI     41
#define GIZA_TOKEN_CHI     42
#define GIZA_TOKEN_PSI     43
#define GIZA_TOKEN_OMEGA   44

#define GIZA_TOKEN_BIGALPHA   45
#define GIZA_TOKEN_BIGBETA    46
#define GIZA_TOKEN_BIGGAMMA   47
#define GIZA_TOKEN_BIGDELTA   48
#define GIZA_TOKEN_BIGEPSILON 49
#define GIZA_TOKEN_BIGZETA    50
#define GIZA_TOKEN_BIGETA     51
#define GIZA_TOKEN_BIGTHETA   52
#define GIZA_TOKEN_BIGIOTA    53
#define GIZA_TOKEN_BIGKAPPA   54
#define GIZA_TOKEN_BIGLAMBDA  55
#define GIZA_TOKEN_BIGMU      56
#define GIZA_TOKEN_BIGNU      57
#define GIZA_TOKEN_BIGXI      58
#define GIZA_TOKEN_BIGOMICRON 59
#define GIZA_TOKEN_BIGPI      60
#define GIZA_TOKEN_BIGRHO     61
#define GIZA_TOKEN_BIGSIGMA   62
#define GIZA_TOKEN_BIGTAU     63
#define GIZA_TOKEN_BIGUPSILON 64
#define GIZA_TOKEN_BIGPHI     65
#define GIZA_TOKEN_BIGCHI     66
#define GIZA_TOKEN_BIGPSI     67
#define GIZA_TOKEN_BIGOMEGA   68

#define GIZA_TOKEN_ODOT 69

#define GIZA_SCALE_SUPERS 0.65
#define GIZA_RAISE_SUPERS 0.4
#define GIZA_SCALE_SUBS   0.65
#define GIZA_LOWER_SUBS   0.2

void _giza_add_token_to_string (char *string, yyscan_t scanner, int token);
int _giza_get_chunk (char *text);

%}

SUPER    \136
SUB      \137
RAISE    "\\u"
LOWER    "\\d"
OB       \173 
CB       \175
TIMES    "\\times"|"\\x"
INTEGRAL "\\int"|"\\(2268)"
NABLA    "\\nabla"|"\\(2266)"

FONT_NORMAL "\\fn"
FONT_ROMAN "\\fr"|"\\rm"
FONT_ITALIC "\\fi"|"\\it"
FONT_SCRIPT "\\fs"|"\\cal"

ALPHA    "\\ga"|"\\alpha"
BETA     "\\gb"|"\\beta"
GAMMA    "\\gg"|"\\gamma"
DELTA    "\\gd"|"\\delta"
EPSILON  "\\ge"|"\\epsilon"
ZETA     "\\gz"|"\\zeta"
ETA      "\\gy"|"\\eta"
THETA    "\\gh"|"\\theta"
IOTA     "\\gi"|"\\iota"
KAPPA    "\\gk"|"\\kappa"
LAMBDA   "\\gl"|"\\lambda"
MU       "\\gm"|"\\mu"
NU       "\\gn"|"\\nu"
XI       "\\gc"|"\\xi"
OMICRON  "\\go"|"\\omicron"
PI       "\\gp"|"\\pi"
RHO      "\\gr"|"\\rho"
SIGMA    "\\gs"|"\\sigma"
TAU      "\\gt"|"\\tau"
UPSILON  "\\gu"|"\\upsilon"
PHI      "\\gf"|"\\phi"
CHI      "\\gx"|"\\chi"
PSI      "\\gq"|"\\psi"
OMEGA    "\\gw"|"\\omega"

BIGALPHA   "\\gA"|"\\Alpha"
BIGBETA    "\\gB"|"\\Beta"
BIGGAMMA   "\\gG"|"\\Gamma"
BIGDELTA   "\\gD"|"\\Delta"
BIGEPSILON "\\gE"|"\\Epsilon"
BIGZETA    "\\gZ"|"\\Zeta"
BIGETA     "\\gY"|"\\Eta"
BIGTHETA   "\\gH"|"\\Theta"
BIGIOTA    "\\gI"|"\\Iota"
BIGKAPPA   "\\gK"|"\\Kappa"
BIGLAMBDA  "\\gL"|"\\Lambda"
BIGMU      "\\gM"|"\\Mu"
BIGNU      "\\gN"|"\\Nu"
BIGXI      "\\gC"|"\\Xi"
BIGOMICRON "\\gO"|"\\Omicron"
BIGPI      "\\gP"|"\\Pi"
BIGRHO     "\\gR"|"\\Rho"
BIGSIGMA   "\\gS"|"\\Sigma"
BIGTAU     "\\gT"|"\\Tau"
BIGUPSILON "\\gU"|"\\Upsilon"
BIGPHI     "\\gF"|"\\Phi"
BIGCHI     "\\gX"|"\\Chi"
BIGPSI     "\\gQ"|"\\Psi"
BIGOMEGA   "\\gW"|"\\Omega"
ODOT       "\\odot"|"\\Sun"

OTHER  [\0-\135]|[\138-\172]|[\174]|[\176-\x7F]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]|[\xE1-\xEF][\x80-\xBF][\x80-\xBF]
PRINT  {FONT_NORMAL}|{FONT_ROMAN}|{FONT_ITALIC}|{FONT_SCRIPT}|{TIMES}|{ODOT}|{INTEGRAL}|{NABLA}|{ALPHA}|{BETA}|{GAMMA}|{DELTA}|{EPSILON}|{ZETA}|{ETA}|{THETA}|{IOTA}|{KAPPA}|{LAMBDA}|{MU}|{NU}|{XI}|{OMICRON}|{PI}|{RHO}|{SIGMA}|{TAU}|{UPSILON}|{PHI}|{CHI}|{PSI}|{OMEGA}|{BIGALPHA}|{BIGBETA}|{BIGGAMMA}|{BIGDELTA}|{BIGEPSILON}|{BIGZETA}|{BIGETA}|{BIGTHETA}|{BIGIOTA}|{BIGKAPPA}|{BIGLAMBDA}|{BIGMU}|{BIGNU}|{BIGXI}|{BIGOMICRON}|{BIGPI}|{BIGRHO}|{BIGSIGMA}|{BIGTAU}|{BIGUPSILON}|{BIGPHI}|{BIGCHI}|{BIGPSI}|{BIGOMEGA}|{OTHER}
ANY    [\0-\x7F]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]|[\xE1-\xEF][\x80-\xBF][\x80-\xBF]

%%

{SUPER} { return GIZA_TOKEN_SUPER; }

{SUB}   { return GIZA_TOKEN_SUB; }

{RAISE} { return GIZA_TOKEN_RAISE; }

{LOWER}   { return GIZA_TOKEN_LOWER; }

{OB}    { return GIZA_TOKEN_OB; }

{CB}    { return GIZA_TOKEN_CB; }

{OTHER} { return GIZA_TOKEN_OTHER; }

{FONT_NORMAL} { return GIZA_TOKEN_FONT_NORMAL; }
{FONT_ROMAN} { return GIZA_TOKEN_FONT_ROMAN; }
{FONT_ITALIC} { return GIZA_TOKEN_FONT_ITALIC; }
{FONT_SCRIPT} { return GIZA_TOKEN_FONT_SCRIPT; }

{TIMES} { return GIZA_TOKEN_TIMES; }
{INTEGRAL} { return GIZA_TOKEN_INTEGRAL; }
{NABLA} { return GIZA_TOKEN_NABLA; }
{ODOT} { return GIZA_TOKEN_ODOT; }

{ALPHA}     { return GIZA_TOKEN_ALPHA; }
{BETA}      { return GIZA_TOKEN_BETA; }
{GAMMA}     { return GIZA_TOKEN_GAMMA; }
{DELTA}     { return GIZA_TOKEN_DELTA; }
{EPSILON}   { return GIZA_TOKEN_EPSILON; }
{ZETA}      { return GIZA_TOKEN_ZETA; }
{ETA}       { return GIZA_TOKEN_ETA; }
{THETA}     { return GIZA_TOKEN_THETA; }
{IOTA}      { return GIZA_TOKEN_IOTA; }
{KAPPA}     { return GIZA_TOKEN_KAPPA; }
{LAMBDA}    { return GIZA_TOKEN_LAMBDA; }
{MU}        { return GIZA_TOKEN_MU; }
{NU}        { return GIZA_TOKEN_NU; }
{XI}        { return GIZA_TOKEN_XI; }
{OMICRON}   { return GIZA_TOKEN_OMICRON; }
{PI}        { return GIZA_TOKEN_PI; }
{RHO}       { return GIZA_TOKEN_RHO; }
{SIGMA}     { return GIZA_TOKEN_SIGMA; }
{TAU}       { return GIZA_TOKEN_TAU; }
{UPSILON}   { return GIZA_TOKEN_UPSILON; }
{PHI}       { return GIZA_TOKEN_PHI; }
{CHI}       { return GIZA_TOKEN_CHI; }
{PSI}       { return GIZA_TOKEN_PSI; }
{OMEGA}     { return GIZA_TOKEN_OMEGA; }

{BIGALPHA}   { return GIZA_TOKEN_BIGALPHA; }
{BIGBETA}    { return GIZA_TOKEN_BIGBETA; }
{BIGGAMMA}   { return GIZA_TOKEN_BIGGAMMA; }
{BIGDELTA}   { return GIZA_TOKEN_BIGDELTA; }
{BIGEPSILON} { return GIZA_TOKEN_BIGEPSILON; }
{BIGZETA}    { return GIZA_TOKEN_BIGZETA; }
{BIGETA}     { return GIZA_TOKEN_BIGETA; }
{BIGTHETA}   { return GIZA_TOKEN_BIGTHETA; }
{BIGIOTA}    { return GIZA_TOKEN_BIGIOTA; }
{BIGKAPPA}   { return GIZA_TOKEN_BIGKAPPA; }
{BIGLAMBDA}  { return GIZA_TOKEN_BIGLAMBDA; }
{BIGMU}      { return GIZA_TOKEN_BIGMU; }
{BIGNU}      { return GIZA_TOKEN_BIGNU; }
{BIGXI}      { return GIZA_TOKEN_BIGXI; }
{BIGOMICRON} { return GIZA_TOKEN_BIGOMICRON; }
{BIGPI}      { return GIZA_TOKEN_BIGPI; }
{BIGRHO}     { return GIZA_TOKEN_BIGRHO; }
{BIGSIGMA}   { return GIZA_TOKEN_BIGSIGMA; }
{BIGTAU}     { return GIZA_TOKEN_BIGTAU; }
{BIGUPSILON} { return GIZA_TOKEN_BIGUPSILON; }
{BIGPHI}     { return GIZA_TOKEN_BIGPHI; }
{BIGCHI}     { return GIZA_TOKEN_BIGCHI; }
{BIGPSI}     { return GIZA_TOKEN_BIGPSI; }
{BIGOMEGA}   { return GIZA_TOKEN_BIGOMEGA; }


<<EOF>> { return GIZA_TOKEN_END; }

{ANY} { return GIZA_TOKEN_ERROR; }

%%

/**
 * Parses brackets in text, returns the length of the string contained in outermost brackets
 */
int
_giza_get_chunk (char *text)
{
  int openb = 0;
  int length = 0;
  int token, stop = 0;

  /* initialise the scanner */
  yyscan_t scanner;
  yylex_init (&scanner);
  yy_scan_string (text, scanner);
 
  while(!stop)
    {
      token = yylex(scanner);
      length += yyget_leng (scanner);

      switch (token)
        {
	  case GIZA_TOKEN_OB:
	    ++openb;
	    break;
	  case GIZA_TOKEN_CB:
	    --openb;
	    break;
          case GIZA_TOKEN_END:
            stop = 1;
            break;
	  default:
	    break;
        }

	if (openb < 1) 
          {
            yylex_destroy (scanner);
            return length;
          }
    }
   yylex_destroy (scanner);
   return length; /* DJP: assumes whole string if unclosed open bracket */
}

/**
 * Performs action on each chunk of text. If calculating size height and width get set to height and width of the text.
 */
void
_giza_parse_string (char *text, double *width, double *height, void (*action)(const char *, double *, double *))
{
  yyscan_t scanner;
  int stop = 0, token, chunkLength, oldTrans;
  int posInText = 0;
  int insuper = 0;
  int insub   = 0;
  int lenstr  = strlen(text);
  char processed[lenstr + 1];
  processed[0] = '\0';

  oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  /* initialise the scanner */
  yylex_init (&scanner);
  yy_scan_string (text, scanner);
 
  while(!stop)
    {
      token = yylex(scanner);
      posInText += yyget_leng (scanner);;
      /*printf("in parse string, token = %i pos = %i processed=\"%s\"\n",token,posInText,processed); */

      switch(token)
        {
        case GIZA_TOKEN_OB:
        case GIZA_TOKEN_CB: /* ignore closing bracket! */
          break;
        case GIZA_TOKEN_SUPER:
          /* Perform action on the text so far parsed */
          action (processed, width, height);
          processed[0] = '\0';

          /* if end of string, do nothing */
          if (posInText >= lenstr)
            { 
              stop = 1;
              break;
            }

          /* Parse the brackets, i.e. get the chunk of text that needs to be superscript */
          chunkLength = _giza_get_chunk (text + posInText);
          strncpy (processed, text + posInText, chunkLength);
          processed[chunkLength] = '\0';
          
          /* Perform the action on this chunk */
          _giza_start_super ();
          _giza_parse_string (processed, width, height, action);
          _giza_stop_super ();

          /* restart the scanner at the new position! */
          yylex_destroy (scanner);
          yylex_init (&scanner);
          yy_scan_string (text + posInText + chunkLength, scanner);

          posInText += chunkLength;

          processed[0] = '\0';
          break;
        case GIZA_TOKEN_SUB:
          /* Perform action on text so far parsed */
          action (processed, width, height);
          processed[0] = '\0';

          /* if end of string, do nothing */
          if (posInText >= lenstr)
            { 
              stop = 1;
              break;
            }

          /* Parse the brackets */
          chunkLength = _giza_get_chunk (text + posInText);
          strncpy (processed, text + posInText, chunkLength);
          processed[chunkLength] = '\0';

          /* perform action on this chunk */
          _giza_start_sub ();
          _giza_parse_string (processed, width, height, action);
          _giza_stop_sub ();

          /* restart the scanner at the new position! */
          yylex_destroy (scanner);
          yylex_init (&scanner);
          yy_scan_string (text + posInText + chunkLength, scanner);

          posInText += chunkLength;

          processed[0] = '\0';
          break;
        case GIZA_TOKEN_RAISE:
          /* Perform action on the text so far parsed */
          action (processed, width, height);
          processed[0] = '\0';
          if (insub > 0)
            {
             _giza_stop_sub();
             insub -= 1;
            }
          else
            {
             _giza_start_super();
             insuper += 1;
            }
          break;
        case GIZA_TOKEN_LOWER:
          /* Perform action on the text so far parsed */
          action (processed, width, height);
          processed[0] = '\0';
          if (insuper > 0)
            {
             _giza_stop_super();
             insuper -= 1;
            }
          else
            {
             _giza_start_sub();
             insub += 1;
            }
          break;
        case GIZA_TOKEN_FONT_NORMAL:
        case GIZA_TOKEN_FONT_ROMAN:
        case GIZA_TOKEN_FONT_ITALIC:
        case GIZA_TOKEN_FONT_SCRIPT:
          /* Perform action on the text so far parsed */
          action (processed, width, height);
          processed[0] = '\0';

          /* if end of string, do nothing */
          if (posInText >= lenstr)
            { 
              stop = 1;
              break;
            }

          /* Parse the brackets */
          chunkLength = _giza_get_chunk (text + posInText);
          strncpy (processed, text + posInText, chunkLength);
          processed[chunkLength] = '\0';

          /* perform action on this chunk */
          switch (token)
            {
            case GIZA_TOKEN_FONT_SCRIPT:
              _giza_switch_font (4);
              break;
            case GIZA_TOKEN_FONT_ITALIC:
              _giza_switch_font (3);
              break;
            case GIZA_TOKEN_FONT_ROMAN:
              _giza_switch_font (2);
              break;
            case GIZA_TOKEN_FONT_NORMAL:
              _giza_switch_font (1);
              break;
            }
          _giza_parse_string (processed, width, height, action);

          /* restart the scanner at the new position! */
          yylex_destroy (scanner);
          yylex_init (&scanner);
          yy_scan_string (text + posInText + chunkLength, scanner);

          posInText += chunkLength;

          processed[0] = '\0';
          break;

        case GIZA_TOKEN_END:
          stop = 1;
          break;
        default:
          _giza_add_token_to_string (processed, scanner, token);
          break;
        }
    }
  yylex_destroy (scanner);

  /* Perform action on the last section of text */
  action (processed, width, height);

  _giza_set_trans (oldTrans);
}

/**
 * Adds the utf-8 character representation of token to string.
 */
void
_giza_add_token_to_string (char *string, yyscan_t scanner, int token)
{
  switch (token)
    {
     case GIZA_TOKEN_OTHER: /* print the ascii or unicode char */
       strcat (string, yyget_text (scanner));
       break;
     case GIZA_TOKEN_TIMES:
       strcat (string, "×");
       break;
     case GIZA_TOKEN_ALPHA: 
       strcat (string, "α");
       break;
     case GIZA_TOKEN_BETA: 
       strcat (string, "β");
       break;
     case GIZA_TOKEN_GAMMA: 
       strcat (string, "γ");
       break;
     case GIZA_TOKEN_DELTA: 
       strcat (string, "δ");
       break;
     case GIZA_TOKEN_EPSILON: 
       strcat (string, "ε");
       break;
     case GIZA_TOKEN_ZETA: 
       strcat (string, "ζ");
       break;
     case GIZA_TOKEN_ETA: 
       strcat (string, "η");
       break;
     case GIZA_TOKEN_THETA: 
       strcat (string, "θ");
       break;
     case GIZA_TOKEN_IOTA: 
       strcat (string, "ι");
       break;
     case GIZA_TOKEN_KAPPA: 
       strcat (string, "κ");
       break;
     case GIZA_TOKEN_LAMBDA: 
       strcat (string, "λ");
       break;
     case GIZA_TOKEN_MU: 
       strcat (string, "μ");
       break;
     case GIZA_TOKEN_NU: 
       strcat (string, "ν");
       break;
     case GIZA_TOKEN_XI: 
       strcat (string, "ξ");
       break;
     case GIZA_TOKEN_OMICRON: 
       strcat (string, "ο");
       break;
     case GIZA_TOKEN_PI: 
       strcat (string, "π");
       break;
     case GIZA_TOKEN_RHO: 
       strcat (string, "ρ");
       break;
     case GIZA_TOKEN_SIGMA: 
       strcat (string, "σ");
       break;
     case GIZA_TOKEN_TAU: 
       strcat (string, "τ");
       break;
     case GIZA_TOKEN_UPSILON: 
       strcat (string, "υ");
       break;
     case GIZA_TOKEN_PHI: 
       strcat (string, "φ");
       break;
     case GIZA_TOKEN_CHI: 
       strcat (string, "χ");
       break;
     case GIZA_TOKEN_PSI: 
       strcat (string, "ψ");
       break;
     case GIZA_TOKEN_OMEGA: 
       strcat (string, "ω");
       break;
     case GIZA_TOKEN_BIGALPHA: 
       strcat (string, "Α");
       break;
     case GIZA_TOKEN_BIGBETA: 
       strcat (string, "Β");
       break;
     case GIZA_TOKEN_BIGGAMMA: 
       strcat (string, "Γ");
       break;
     case GIZA_TOKEN_BIGDELTA: 
       strcat (string, "Δ");
       break;
     case GIZA_TOKEN_BIGEPSILON: 
       strcat (string, "Ε");
       break;
     case GIZA_TOKEN_BIGZETA: 
       strcat (string, "Ζ");
       break;
     case GIZA_TOKEN_BIGETA: 
       strcat (string, "Η");
       break;
     case GIZA_TOKEN_BIGTHETA: 
       strcat (string, "Θ");
       break;
     case GIZA_TOKEN_BIGIOTA: 
       strcat (string, "Ι");
       break;
     case GIZA_TOKEN_BIGKAPPA: 
       strcat (string, "Κ");
       break;
     case GIZA_TOKEN_BIGLAMBDA: 
       strcat (string, "Λ");
       break;
     case GIZA_TOKEN_BIGMU: 
       strcat (string, "Μ");
       break;
     case GIZA_TOKEN_BIGNU: 
       strcat (string, "Ν");
       break;
     case GIZA_TOKEN_BIGXI: 
       strcat (string, "Ξ");
       break;
     case GIZA_TOKEN_BIGOMICRON: 
       strcat (string, "Ο");
       break;
     case GIZA_TOKEN_BIGPI: 
       strcat (string, "Π");
       break;
     case GIZA_TOKEN_BIGRHO: 
       strcat (string, "Ρ");
       break;
     case GIZA_TOKEN_BIGSIGMA: 
       strcat (string, "Σ");
       break;
     case GIZA_TOKEN_BIGTAU: 
       strcat (string, "Τ");
       break;
     case GIZA_TOKEN_BIGUPSILON: 
       strcat (string, "Υ");
       break;
     case GIZA_TOKEN_BIGPHI: 
       strcat (string, "Φ");
       break;
     case GIZA_TOKEN_BIGCHI: 
       strcat (string, "Χ");
       break;
     case GIZA_TOKEN_BIGPSI: 
       strcat (string, "Ψ");
       break;
     case GIZA_TOKEN_BIGOMEGA: 
       strcat (string, "Ω");
       break;
     case GIZA_TOKEN_INTEGRAL:
       strcat (string, "∫");
       break;
     case GIZA_TOKEN_NABLA:
       strcat (string, "∇");
       break;
     case GIZA_TOKEN_ODOT:
       strcat (string, "⊙");
       break;
     default:
       break;
    }
}
