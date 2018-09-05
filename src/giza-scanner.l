/* A re-entrant scanner to recursively print text */

/* Specify that it is re-entrant and doesn't use ywrap */
%option reentrant noyywrap

/* Include the header and define tokens */

%{
/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2014 Daniel Price
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
#include "giza-transforms-private.h"
#include "giza-drivers-private.h"
#include "giza-text-private.h"
#include "giza-hershey-to-utf.h"
#include "giza.h"
#include <string.h>
#include <math.h>
#include <ctype.h>


#define GIZA_TOKEN_END         1
#define GIZA_TOKEN_SUPER       2
#define GIZA_TOKEN_SUB         3
#define GIZA_TOKEN_OB          4
#define GIZA_TOKEN_CB          5
#define GIZA_TOKEN_RAISE       8
#define GIZA_TOKEN_LOWER       9


#define GIZA_TOKEN_FONT        110
#define GIZA_TOKEN_HERSHEY     111
#define GIZA_TOKEN_MARKER      112
#define GIZA_TOKEN_GREEK       113

#define GIZA_TOKEN_BACKSPACE   120
#define GIZA_TOKEN_REMAP       121

#define GIZA_TOKEN_TEXT        130

/* the token remapper can only be included after the tokens have been defined */
#include "giza-token-remapper.h"

/* Map single character to greek letter according to PGPLOT grsyds.f */
/* ABGDEZYHIKLMNCOPRSTUFXQWabgdezyhiklmncoprstufxqw */
static char const* const giza_greek_alphabet      = "ABGDEZYHIKLMNCOPRSTUFXQWabgdezyhiklmncoprstufxqw";
static char const* const giza_greek_to_utf_data[] = {
    /* Upper case */
    "\u0391", "\u0392", "\u0393", "\u0394", "\u0395", "\u0396", "\u0397", "\u0398",
    "\u0399", "\u039a", "\u039b", "\u039c", "\u039d", "\u039e", "\u039f", "\u03a0",
    "\u03a1", "\u03a3", "\u03a4", "\u03a5", "\u03a6", "\u03a7", "\u03a8", "\u03a9",
    /* and lower case */
    "\u03b1", "\u03b2", "\u03b3", "\u03b4", "\u03b5", "\u03b6", "\u03b7", "\u03b8",
    "\u03b9", "\u03ba", "\u03bb", "\u03bc", "\u03bd", "\u03be", "\u03bf", "\u03c0",
    "\u03c1", "\u03c3", "\u03c4", "\u03c5", "\u03c6", "\u03c7", "\u03c8", "\u03c9"
};

/* Assume the caller has verified that letter is a /valid/ greek letter! */
static char const*                       _giza_greek_to_utf(char letter);
/* May return NULL if number is not a known Hershey symbol or the string in
  <special> is not found in the remapped token lookup table */
static giza_hershey_to_utf_entry const*  _giza_lookup_hershey(int number);
static giza_remapped_token_entry const*  _giza_remap_token(char const* special);

%}


%x FONT HERSHEY MARKER GREEK ESCAPE

%%

<INITIAL>"\\"[uU]     { return GIZA_TOKEN_RAISE; }
<INITIAL>"\\"[dD]     { return GIZA_TOKEN_LOWER; }
<INITIAL>"\\"[bB]     { return GIZA_TOKEN_BACKSPACE; }

<INITIAL>"\\"[fF]     { BEGIN(FONT); }
<FONT>[nrisbNRISB]    { BEGIN(INITIAL); return GIZA_TOKEN_FONT; };
<FONT>.               { BEGIN(INITIAL); _giza_warning("giza_parse_string", "invalid font specifier '%c'", *yytext); return GIZA_TOKEN_TEXT; }

<INITIAL>"\\("        { BEGIN(HERSHEY); }
<HERSHEY>[0-9]{4}")"  { if( _giza_lookup_hershey(atoi(yytext)) ) { BEGIN(INITIAL); return GIZA_TOKEN_HERSHEY; } REJECT; }
<HERSHEY>[^)]*        { BEGIN(INITIAL); _giza_warning("giza_parse_string", "invalid Hershey code '%s'", yytext); return GIZA_TOKEN_TEXT; }

<INITIAL>"\\"[mM]     { BEGIN(MARKER); }
<MARKER>[0-9]{1,2}    { const int marker = atoi(yytext); if( marker>=1 && marker<=31 ) { BEGIN(INITIAL); return GIZA_TOKEN_MARKER; } REJECT; }
<MARKER>.             { BEGIN(INITIAL); _giza_warning("giza_parse_string", "invalid marker '%s'", yytext); return GIZA_TOKEN_TEXT; }

<INITIAL>"\\"[gG]        { BEGIN(GREEK); }
<GREEK>[ABGDEZYHIKLMNCOPRSTUFXQWabgdezyhiklmncoprstufxqw] { BEGIN(INITIAL); return GIZA_TOKEN_GREEK; }
<GREEK>.              { BEGIN(INITIAL); _giza_warning("giza_parse_string", "invalid greek letter '%c'", *yytext); return GIZA_TOKEN_TEXT; }

<INITIAL>"\\"         { BEGIN(ESCAPE); }
<ESCAPE>[a-zA-Z]{2,}  { if( _giza_remap_token(yytext) ) { BEGIN(INITIAL); return GIZA_TOKEN_REMAP; } REJECT; }
<ESCAPE>.             { BEGIN(INITIAL); return _giza_remap_token(yytext) ? GIZA_TOKEN_REMAP : GIZA_TOKEN_TEXT; /* single character escape sequence or ordinary escaped character meaning */ }

<INITIAL>"^"          { return GIZA_TOKEN_SUPER; }
<INITIAL>"_"          { return GIZA_TOKEN_SUB; }
<INITIAL>"{"          { return GIZA_TOKEN_OB; }
<INITIAL>"}"          { return GIZA_TOKEN_CB; }

<INITIAL>.            { return GIZA_TOKEN_TEXT; }

<<EOF>>               { return GIZA_TOKEN_END; }

%%


/* Functions that look up character sequences and return utf strings; 
   giza is built upon cairo, which in turn handles utf Just Fine (tm) */
giza_hershey_to_utf_entry const* _giza_lookup_hershey(int number) {
    giza_hershey_to_utf_entry const* rv = giza_hershey_to_utf;
    while( rv->hershey!=-1 && rv->hershey!=number )
        rv++;
    return (rv->hershey==-1 ? NULL : rv);
}

giza_remapped_token_entry const* _giza_remap_token(char const* special) {
    giza_remapped_tokens_entry const* rv = giza_remappable_tokens;

    while( rv->text && strcmp(rv->text, special) )
        rv++;
    return (rv->text==NULL ? NULL : &rv->replacement);
}

char const* giza_greek_to_utf(char letter) {
    return giza_greek_to_utf_data[ strchr(giza_greek_alphabet, letter) - giza_greek_alphabet ];
}


#define MAX_NFONT 32
typedef struct _font_change_type {
    char    font[GIZA_FONT_LEN];
    int     ob_level;       /* open bracket level at start of font change*/
    int     wait_for_token; /* 0 = wait for first token to be seen, < 0 stop font change immediately, == GIZA_TOKEN_CB wait until ob_count drops below .ob_level */
} font_change_type;

typedef struct _glyph_pos_type {
    double x, y;
} glyph_pos_type;

typedef struct _ob_state_type {
    int   ob_level;       /* open bracket level at start of state */
    int   wait_for_token; /* 0 = wait for first token to be seen, < 0 stop super/sub script immediately, == GIZA_TOKEN_CB wait until global ob_count drops below .ob_level */
} ob_state_type;

/**
 * Performs action on each chunk of text. If calculating size height and width get set to height and width of the text.
 */
    void
_giza_parse_string (const char *text, double *width, double *height, void (*action)(const char *, double *, double *))
{
  if( text==NULL ) {
      _giza_error("giza_parse_string", "NULL string");
      return;
  }

  const int            lenstr = strlen(text);
  if( lenstr==0 )
      return;

  yyscan_t             scanner;
  int                  stop      = 0, token, oldTrans;
  int                  insuper   = 0, insub = 0;
  int                  ob        = 0; /* count open/close bracket */
  int                  bckSpace  = 0;
  int                  nGlyph    = 0;
  double               sina = sin(Dev[id].fontAngle);
  double               cosa = cos(Dev[id].fontAngle);
  char const*          token_txt;
  ob_state_type        ob_super[MAX_NFONT], *cur_super = &ob_super[-1];/* keep track of superscript state */
  ob_state_type        ob_sub[MAX_NFONT],   *cur_sub   = &ob_sub[-1]; /* keep track of subscript state */
  glyph_pos_type       positions[lenstr];  /* keep track of each glyph's x,y position (for backing up characters) */
  font_change_type     oldfont[MAX_NFONT],  *cur_font = &oldfont[-1]; /* keep track of font change state */

  oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  /* initialise the scanner */
  yylex_init (&scanner);
  yy_scan_string (text, scanner);

  while(!stop)
  {
      /* we can always do this - repeatedly overwriting the current top-of-symbol-stack's position
         until the code below actually draws a symbol and increments nGlyph */
      cairo_get_current_point(Dev[id].context, &positions[nGlyph].x, &positions[nGlyph].y);

      /* Get current token from the scanner */
      token      = yylex(scanner);
      token_txt  = yyget_text(scanner);

      /* If we found a special character sequence (e.g. "\Sun") this may be remapped to an actual symbol */
      if( token==GIZA_TOKEN_REMAP ) {
          giza_remapped_token_entry const* ptr = _giza_remap_token(token_txt);
          token     = ptr->token;
          token_txt = ptr->text;
      }
      /* Check if we need to do font, super- or subscript first-token-after-change checking:
         if the first token after a change is not open bracket, the change
         lasts only for the current glyph (if any)  [single character super-/subscript],
         for font change we only keep a stack of nested font changes, not
         'globals', which only last until the closing bracket of the current top-of-stack (if any) */
      if( (cur_super>=ob_super) && cur_super->wait_for_token==0 )
          cur_super->wait_for_token = (token == GIZA_TOKEN_OB ? GIZA_TOKEN_CB : -1);
      if( cur_sub>=ob_sub   && cur_sub->wait_for_token==0 )
          cur_sub->wait_for_token = (token == GIZA_TOKEN_OB ? GIZA_TOKEN_CB : -1);

      if( cur_font>=oldfont && cur_font->wait_for_token==0 ) {
          cur_font->wait_for_token = (token == GIZA_TOKEN_OB ? GIZA_TOKEN_CB : -1);
          /* check if we need to 'forget' this font change state; if it was 'global' */
          if( cur_font->wait_for_token==-1 /*it was a 'global font change'*/ &&
              cur_font>oldfont /* there exists at least a previous state */ &&
              (cur_font-1)->wait_for_token==GIZA_TOKEN_CB /* and that one is waiting for close bracket*/ )
                /* basically forget current font state */
                cur_font--;
      }

      switch(token)
      {
          case GIZA_TOKEN_OB:
              ob++;
              break;
          case GIZA_TOKEN_CB:
              ob--;
              /* font change remains in effect until end of string or until close bracket, the latter only if the first token after
                 font change was open bracket  */
              if( (cur_font>=oldfont) && cur_font->wait_for_token==GIZA_TOKEN_CB && ob<=cur_font->ob_level ) {
                  /* OK go back to old font */
                  giza_set_font(cur_font->font);
                  cur_font--;
              }
              /* Actual processing of closing bracket for super/sub is /after/ the switch statement
                 because then it can be shared with single-token super/sub handling */
              break;

          /* Handle a Hershey symbol by replacing with its utf version (if any)
             It's already verified by the lexer that the token is a valid Hershey number */
          case GIZA_TOKEN_HERSHEY:
              {
                  action( _giza_lookup_hershey(atoi(token_txt))->utf, width, height );
                  /* added new character to end of string so 'top of stack'
                     for backing up is now restored to new end of string */
                  bckSpace = nGlyph;
              }
              break;

          /* Start of super/sub script. Mark the current level as waiting to check the next token;
             if it's not open bracket, we must drop the superscript immediately after the first token */
          case GIZA_TOKEN_SUPER:
              if( cur_super<&ob_super[MAX_NFONT-1] ) {
                  cur_super++;
                  cur_super->ob_level       = ob;
                  cur_super->wait_for_token = 0;
                  _giza_start_super ();
              } else {
                  _giza_error("giza_parse_string", "Too many levels of super-script");
              }
              break;
          case GIZA_TOKEN_SUB:
              if( cur_sub<&ob_sub[MAX_NFONT-1] ) {
                  cur_sub++;
                  cur_sub->ob_level       = ob;
                  cur_sub->wait_for_token = 0;
                  _giza_start_sub ();
              } else {
                  _giza_error("giza_parse_string", "Too many levels of sub-script");
              }
              break;

          /* raising/lowering works relative to current state */
          case GIZA_TOKEN_RAISE:
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

          /* move to previous character position. If backed up before start of string
             ignore silently. 
             Note: in order to support having super + sub scripts appended
                   to the same character you'll have to back up by *two*
                   'characters': first backup up over the superscript, then
                                 backing up to the baseline of the font
                                 before the superscript was entered.
                   It is as if ^, _, \u, \d act as glyphs too only they're
                   not inked.
             The upshot is that x^2\b\b_i renders nicely as proper x-i-squared */
          case GIZA_TOKEN_BACKSPACE:
              bckSpace -= 1;
              if( bckSpace>=0 )
                  cairo_move_to(Dev[id].context, positions[bckSpace].x, positions[bckSpace].y);
              break;

          /* Handle change of font! The lexer already verified it's a /valid/ font specifier */
          case GIZA_TOKEN_FONT:
              if ( cur_font<&oldfont[MAX_NFONT-1] )
              {
                  /* new font change state */
                  cur_font++;
                  cur_font->ob_level       = ob;
                  cur_font->wait_for_token = 0;
                  /* save font */
                  giza_get_font(cur_font->font, GIZA_FONT_LEN);
              } else {
                  _giza_error("giza_parse_string", "Too many nested levels of font changes");
                  break;
              }
              switch ( tolower(*token_txt) )
              {
                  case 'b': /* bold */
                      _giza_switch_font (5);
                      break;
                  case 's': /* script */
                      _giza_switch_font (4);
                      break;
                  case 'i': /* italic */
                      _giza_switch_font (3);
                      break;
                  case 'r': /* roman */
                      _giza_switch_font (2);
                      break;
                  case 'n': /* normal */
                      _giza_switch_font (1);
                      break;
              }
              break;

          /* Draw PGPLOT marker \mnn The lexer already verified it's a /valid/ marker number,
             i.e. 1 <= nn <= 31 */
          case GIZA_TOKEN_MARKER:
              {
                  /* get current marker height */
                  double markerHeight;
                  _giza_get_markerheight(&markerHeight);

                  /* extract the symbol number following the \m */
                  int const number = atoi( token_txt );

                  /* either draw the symbol or increment the width */
                  if (*width < 0.) {
                      double dx = 0.75*markerHeight;
                      double dy = -0.5*markerHeight;
                      _giza_draw_symbol_device(positions[nGlyph].x + dx*cosa + dy*sina,
                                               positions[nGlyph].y + dx*sina + dy*cosa,
                                               number);
                      cairo_move_to(Dev[id].context, positions[nGlyph].x + 2.*dx*cosa, positions[nGlyph].y + 2.*dx*sina); /* restore pen position */
                  } else {
                      /* return width if action is _giza_action_get_size */
                      *width = *width + 1.5*markerHeight;
                  }
              }
              break;

          /* Replace the greek letter by its unicode representation.
             The lexer already verified it's a /valid/ greek letter.
             After that processing is like normal text */
          case GIZA_TOKEN_GREEK:
              token_txt = giza_greek_to_utf( *token_txt );
          case GIZA_TOKEN_TEXT:
              {
                  /* added new character to end of string so 'top of stack'
                     for backing up is now restored to new end of string */
                  bckSpace = nGlyph;
                  action(token_txt, width, height);
              }
              break;

          /* end of string */
          case GIZA_TOKEN_END:
              stop = 1;
              break;
          /* Ow dear, we got something we didn't expect! */
          default:
              _giza_error("giza_parse_string", "unhandled token %d (%s)", token, token_txt);
              stop = 1;
              break;
      }
      /* After having dealt with a token check if we need to drop super/sub/font immediately */
      /* did the closing bracket mark end-of-superscript?
         wait_for_token == CLOSE_BRACKET? then wait for level to fall below that, if == -1, drop immediately */
      if( (cur_super>=ob_super) && ((cur_super->wait_for_token==GIZA_TOKEN_CB && ob<=cur_super->ob_level) || cur_super->wait_for_token==-1) ) {
          _giza_stop_super();
          cur_super--;
      }
      /* did the closing bracket mark end-of-subscript? */
      if( (cur_sub>=ob_sub) && ((cur_sub->wait_for_token==GIZA_TOKEN_CB && ob<=cur_sub->ob_level) || cur_sub->wait_for_token==-1) ) {
          _giza_stop_sub();
          cur_sub--;
      }
      /* Move on to next 'glyph' position */
      nGlyph++;
  }
  yylex_destroy (scanner);

  _giza_set_trans (oldTrans);

  /* In case we broke from the loop but there's font change still in effect
     switch back to the font we started with */
  if( cur_font>=oldfont )
      giza_set_font( oldfont[0].font );
}

