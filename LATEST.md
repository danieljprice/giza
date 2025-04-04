Changes in v1.5.0
-----------------
- fixed issues needed for PGPLOT compatibility in heasoft (#48, #66)
- additional arguments to giza_render to specify the type of pixel interpolation desired
- better compatibility with PGPLOT in settings for pixel interpolation, fixes issue with blurred images in heasoft
- fix seg fault in PGCONT calls if argument NC is negative, fixes issue in running pgdemo programs
- removed tabs from source code
- test_contour and test_render now included in test suite
