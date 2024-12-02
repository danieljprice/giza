/* 
 * In order to support aliases (e.g. \Sun for Hershey symbol "\(2281)",
 *  \alpha for "\ga", \rm for roman font ("\fr")) create a mapping
 * from "\<special>" => { <new token type>, <new token text> }
 */

typedef struct _giza_remapped_token_entry {
    int const   token;
    char const* text;
} giza_remapped_token_entry;

typedef struct _giza_remapped_tokens_entry {
    char const*                     text;
    giza_remapped_token_entry const replacement;
} giza_remapped_tokens_entry;

static giza_remapped_tokens_entry giza_remappable_tokens[] = {
    /* solar system */
    {"Sun",      {GIZA_TOKEN_HERSHEY, "2281"   }},
    {"Mercury",  {GIZA_TOKEN_HERSHEY, "2282"   }},
    {"Venus",    {GIZA_TOKEN_HERSHEY, "2283"   }},
    {"Earth",    {GIZA_TOKEN_HERSHEY, "2284"   }},
    {"Mars",    {GIZA_TOKEN_HERSHEY, "2285"   }},
    {"Jupiter",    {GIZA_TOKEN_HERSHEY, "2286"   }},
    {"Saturn",    {GIZA_TOKEN_HERSHEY, "2287"   }},
    {"Uranus",    {GIZA_TOKEN_HERSHEY, "2288"   }},
    {"Neptune",    {GIZA_TOKEN_HERSHEY, "2289"   }},
    {"Pluto",    {GIZA_TOKEN_HERSHEY, "2290"   }},
    {"Moon",    {GIZA_TOKEN_HERSHEY, "2291"   }},
    /* mathematical symbols */
    {"." ,       {GIZA_TOKEN_TEXT   , "\u22c5" }},
    {"x",        {GIZA_TOKEN_TEXT   , "\u00d7" }},
    {"times",    {GIZA_TOKEN_TEXT   , "\u00d7" }},
    {"int",      {GIZA_TOKEN_HERSHEY, "2268"   }},
    {"nabla",    {GIZA_TOKEN_HERSHEY, "2266"   }},
    {"odot" ,    {GIZA_TOKEN_MARKER , "9"      }},
    /* misc */
    {"A",        {GIZA_TOKEN_TEXT   , "\u212b" }}, /* Angstrom */
    /* fonts */
    {"rm" ,      {GIZA_TOKEN_FONT   , "r"      }},
    {"it" ,      {GIZA_TOKEN_FONT   , "i"      }},
    {"cal" ,     {GIZA_TOKEN_FONT   , "s"      }},
    {"bf" ,      {GIZA_TOKEN_FONT   , "b"      }},
    /* Greek  lower + upper */
    {"alpha",    {GIZA_TOKEN_GREEK  , "a"      }},
    {"beta",     {GIZA_TOKEN_GREEK  , "b"      }},
    {"gamma",    {GIZA_TOKEN_GREEK  , "g"      }},
    {"delta",    {GIZA_TOKEN_GREEK  , "d"      }},
    {"epsilon",  {GIZA_TOKEN_GREEK  , "e"      }},
    {"zeta",     {GIZA_TOKEN_GREEK  , "z"      }},
    {"eta",      {GIZA_TOKEN_GREEK  , "y"      }},
    {"theta",    {GIZA_TOKEN_GREEK  , "h"      }},
    {"iota",     {GIZA_TOKEN_GREEK  , "i"      }},
    {"kappa",    {GIZA_TOKEN_GREEK  , "k"      }},
    {"lambda",   {GIZA_TOKEN_GREEK  , "l"      }},
    {"mu",       {GIZA_TOKEN_GREEK  , "m"      }},
    {"nu",       {GIZA_TOKEN_GREEK  , "n"      }},
    {"xi",       {GIZA_TOKEN_GREEK  , "c"      }},
    {"omicron",  {GIZA_TOKEN_GREEK  , "o"      }},
    {"pi",       {GIZA_TOKEN_GREEK  , "p"      }},
    {"rho",      {GIZA_TOKEN_GREEK  , "r"      }},
    {"sigma",    {GIZA_TOKEN_GREEK  , "s"      }},
    {"tau",      {GIZA_TOKEN_GREEK  , "t"      }},
    {"phi",      {GIZA_TOKEN_GREEK  , "f"      }},
    {"chi",      {GIZA_TOKEN_GREEK  , "x"      }},
    {"psi",      {GIZA_TOKEN_GREEK  , "q"      }},
    {"omega",    {GIZA_TOKEN_GREEK  , "w"      }},
    {"Alpha",    {GIZA_TOKEN_GREEK  , "A"      }},
    {"Beta",     {GIZA_TOKEN_GREEK  , "B"      }},
    {"Gamma",    {GIZA_TOKEN_GREEK  , "G"      }},
    {"Delta",    {GIZA_TOKEN_GREEK  , "D"      }},
    {"Epsilon",  {GIZA_TOKEN_GREEK  , "E"      }},
    {"Zeta",     {GIZA_TOKEN_GREEK  , "Z"      }},
    {"Eta",      {GIZA_TOKEN_GREEK  , "Y"      }},
    {"Theta",    {GIZA_TOKEN_GREEK  , "H"      }},
    {"Iota",     {GIZA_TOKEN_GREEK  , "I"      }},
    {"Kappa",    {GIZA_TOKEN_GREEK  , "K"      }},
    {"Lambda",   {GIZA_TOKEN_GREEK  , "L"      }},
    {"Mu",       {GIZA_TOKEN_GREEK  , "M"      }},
    {"Nu",       {GIZA_TOKEN_GREEK  , "N"      }},
    {"Xi",       {GIZA_TOKEN_GREEK  , "C"      }},
    {"Omicron",  {GIZA_TOKEN_GREEK  , "O"      }},
    {"Pi",       {GIZA_TOKEN_GREEK  , "P"      }},
    {"Rho",      {GIZA_TOKEN_GREEK  , "R"      }},
    {"Sigma",    {GIZA_TOKEN_GREEK  , "S"      }},
    {"Tau",      {GIZA_TOKEN_GREEK  , "T"      }},
    {"Phi",      {GIZA_TOKEN_GREEK  , "F"      }},
    {"Chi",      {GIZA_TOKEN_GREEK  , "X"      }},
    {"Psi",      {GIZA_TOKEN_GREEK  , "Q"      }},
    {"Omega",    {GIZA_TOKEN_GREEK  , "W"      }},
    /* leave as last; it's a sentinel */
    {NULL, {-1, NULL}}
}; 

