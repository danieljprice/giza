#!/bin/bash
destdir=../docs/documentation
./pgplot_status.pl > $destdir/pgplot-status.html;
./cpgplot_status.pl > $destdir/cpgplot-status.html;
./api.pl ../src/*.c;
mv api.html $destdir;
cd $destdir;
sed -e "/\<\!--\#include virtual=\"pgplot-status.html\"--\>/r pgplot-status.html" -e "//d" pgplot.shtml > tmp.html
sed -e "/\<\!--\#include virtual=\"cpgplot-status.html\"--\>/r cpgplot-status.html" -e "//d" tmp.html > pgplot.html
rm tmp.html
