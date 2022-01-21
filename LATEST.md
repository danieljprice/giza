Changes in v1.3.0 compared to v1.2.1
------------------------------------
- new library functions giza_set_motion_callback and giza_end_motion_callback
- can now specify a callback function func(x,y,mode) that will be called every time the cursor moves
- implemented giza_axis, which has replacement functionality for PGAXIS (#12)
- Fortran API no longer trims text strings before passing them to giza routines. This enables one to send a string of blank characters with an opaque background colour in order to erase previously drawn text
