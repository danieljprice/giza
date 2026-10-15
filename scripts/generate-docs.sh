#!/bin/bash
#
# This file is part of giza, a scientific plotting library built on cairo
# Copyright (c) 2022 Daniel Price
# Licensed under the GNU Lesser General Public License (LGPL, see COPYING.LESSER)
#
destdir=../docs/documentation
./pgplot_status.pl > $destdir/pgplot-status.html;
./cpgplot_status.pl > $destdir/cpgplot-status.html;
./api.pl ../src/*.c;
mv api.html $destdir;
cd $destdir;
cat pgplot-header.html > pgplot.html
cat >> pgplot.html << EOF
 <div id="content">
    <h2><a name="libpgplot"></a>Current status (libpgplot):</h2>
EOF
cat pgplot-status.html >> pgplot.html
cat >> pgplot.html << EOF
</div>
EOF
cat >> pgplot.html << EOF
 <div id="content">
    <h2><a name="libcpgplot"></a>Current status (libcpgplot):</h2>
EOF
cat cpgplot-status.html >> pgplot.html
cat >> pgplot.html << EOF
</div>
</html>
EOF
