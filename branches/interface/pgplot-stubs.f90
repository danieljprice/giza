! giza - a scientific plotting layer built on cairo
!
! This file is (or was) part of GIZA, a scientific plotting
! layer built on cairo.
! GIZA comes with ABSOLUTELY NO WARRANTY.
! This is free software; and you are welcome to redistribute
! it under the terms of the GNU General Public License
! (see LICENSE file for details) and the provision that
! this notice remains intact. If you modify this file, please
! note section 2a) of the GPLv2 states that:
!
!  a) You must cause the modified files to carry prominent notices
!     stating that you changed the files and the date of any change.
!
! Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
! Contact: wetter.j@gmail.com or daniel.price@sci.monash.edu.au

!---------------------------------------------------------
!  This file can be used to build a PGPLOT-compatible
!  interface to giza (implements the full public API
!  for PGPLOT), so old PGPLOT-based programs do not need
!  to be recompiled to use giza.
!
!  We have implemented the last public version of PGPLOT
!  as available from the PGPLOT web page.
!---------------------------------------------------------

!------------------------------------------------------------------------
! Module: PGARRO -- draw an arrow
!------------------------------------------------------------------------

subroutine PGARRO (X1, Y1, X2, Y2)
 implicit none
 real, intent(in) :: X1, Y1, X2, Y2

end subroutine pgarro

!------------------------------------------------------------------------
! Module: PGASK -- control new page prompting
!------------------------------------------------------------------------

subroutine PGASK (FLAG)
 implicit none
 logical, intent(in) :: FLAG

end subroutine pgask

!------------------------------------------------------------------------
! Module: PGAXIS -- draw an axis
!------------------------------------------------------------------------

subroutine PGAXIS (OPT, X1, Y1, X2, Y2, V1, V2, STEP, NSUB, DMAJL, DMAJR, FMIN, DISP, ORIENT)
 implicit none
 character*(*), intent(in) :: OPT
 real, intent(in) :: X1, Y1, X2, Y2, V1, V2, STEP, DMAJL, DMAJR, FMIN, DISP
 real, intent(in) :: ORIENT
 integer, intent(in) :: NSUB

end subroutine pgaxis

!------------------------------------------------------------------------
! Module: PGBAND -- read cursor position, with anchor
!------------------------------------------------------------------------

integer function PGBAND (MODE, POSN, XREF, YREF, X, Y, CH)
 implicit none
 integer, intent(in) :: MODE, POSN
 real, intent(in)    :: XREF, YREF
 real, intent(inout) :: X, Y
 character*(*), intent(out) :: CH

end function pgband

!------------------------------------------------------------------------
! Module: PGBBUF -- begin batch of output (buffer)
!------------------------------------------------------------------------

subroutine PGBBUF
 implicit none
end subroutine pgbbuf

!------------------------------------------------------------------------
! Module: PGBEG -- open a graphics device
!------------------------------------------------------------------------

integer function PGBEG (UNIT, FILE, NXSUB, NYSUB)
 implicit none
 integer,       intent(in) :: UNIT
 character*(*), intent(in) :: FILE
 integer,       intent(in) :: NXSUB, NYSUB

end function PGBEG

!------------------------------------------------------------------------
! Module: PGBIN -- histogram of binned data
!------------------------------------------------------------------------

subroutine PGBIN (NBIN, X, DATA, CENTER)
 implicit none
 integer, intent(in) :: NBIN
 real,    intent(in) :: X(*), DATA(*)
 logical, intent(in) :: CENTER

end subroutine PGBIN

!------------------------------------------------------------------------
! Module: PGBOX -- draw labeled frame around viewport
!------------------------------------------------------------------------

subroutine PGBOX (XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)
 implicit none
 character*(*), intent(in) :: XOPT, YOPT
 real,          intent(in) :: XTICK, YTICK
 integer,       intent(in) :: NXSUB, NYSUB

end subroutine PGBOX

!------------------------------------------------------------------------
! Module: PGCIRC -- draw a circle, using fill-area attributes
!------------------------------------------------------------------------

subroutine PGCIRC (XCENT, YCENT, RADIUS)
 implicit none
 real, intent(in) :: XCENT, YCENT, RADIUS

end subroutine PGCIRC
!------------------------------------------------------------------------
! Module: PGCLOS -- close the selected graphics device
!------------------------------------------------------------------------

subroutine PGCLOS
 implicit none

end subroutine PGCLOS

!------------------------------------------------------------------------
! Module: PGCONB -- contour map of a 2D data array, with blanking
!------------------------------------------------------------------------

subroutine PGCONB (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR, BLANK)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6), BLANK

end subroutine PGCONB

!------------------------------------------------------------------------
! Module: PGCONF -- fill between two contours
!------------------------------------------------------------------------

subroutine PGCONF (A, IDIM, JDIM, I1, I2, J1, J2, C1, C2, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) ::    A(IDIM,JDIM), C1, C2, TR(6)

end subroutine PGCONF

!------------------------------------------------------------------------
! Module: PGCONL -- label contour map of a 2D data array 
!------------------------------------------------------------------------

subroutine PGCONL (A, IDIM, JDIM, I1, I2, J1, J2, C, TR, LABEL, INTVAL, MININT)
 implicit none
 integer,       intent(in) :: IDIM, JDIM, I1, J1, I2, J2, INTVAL, MININT
 real,          intent(in) :: A(IDIM,JDIM), C, TR(6)
 character*(*), intent(in) :: LABEL

end subroutine PGCONL

!------------------------------------------------------------------------
! Module: PGCONS -- contour map of a 2D data array (fast algorithm)
!------------------------------------------------------------------------

subroutine PGCONS (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6)

end subroutine PGCONS

!------------------------------------------------------------------------
! Module: PGCONT -- contour map of a 2D data array (contour-following)
!------------------------------------------------------------------------

subroutine PGCONT (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, J1, I2, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6)

end subroutine PGCONT

!------------------------------------------------------------------------
! Module: PGCONX -- contour map of a 2D data array (non rectangular)
!------------------------------------------------------------------------

subroutine PGCONX (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, PLOT)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, J1, I2, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*)
 external :: PLOT

end subroutine PGCONX

!------------------------------------------------------------------------
! Module: PGCTAB -- install the color table to be used by PGIMAG
!------------------------------------------------------------------------

subroutine PGCTAB(L, R, G, B, NC, CONTRA, BRIGHT)
 implicit none
 integer, intent(in) :: NC
 real,    intent(in) :: L(NC), R(NC), G(NC), B(NC), CONTRA, BRIGHT

end subroutine PGCTAB

!------------------------------------------------------------------------
! Module: PGCURS -- read cursor position
!------------------------------------------------------------------------

integer function PGCURS (X, Y, CH)
 implicit none
 real,          intent(inout) :: X, Y
 character*(*), intent(out)   :: CH

end function PGCURS

!------------------------------------------------------------------------
! Module: PGDRAW -- draw a line from the current pen position to a point
!------------------------------------------------------------------------

subroutine PGDRAW (X, Y)
 implicit none
 real, intent(in) :: X, Y

end subroutine PGDRAW

!------------------------------------------------------------------------
! Module: PGEBUF -- end batch of output (buffer)
!------------------------------------------------------------------------
subroutine PGEBUF
 implicit none
end subroutine PGEBUF

!------------------------------------------------------------------------
! Module: PGEND -- close all open graphics devices
!------------------------------------------------------------------------
subroutine PGEND
 implicit none
end subroutine PGEND

!------------------------------------------------------------------------
! Module: PGENV -- set window and viewport and draw labeled frame
!------------------------------------------------------------------------

subroutine PGENV (XMIN, XMAX, YMIN, YMAX, JUST, AXIS)
 implicit none
 real,    intent(in) :: XMIN, XMAX, YMIN, YMAX
 integer, intent(in) :: JUST, AXIS

end subroutine PGENV

!------------------------------------------------------------------------
! Module: PGERAS -- erase all graphics from current page
!------------------------------------------------------------------------

subroutine PGERAS
 implicit none
end subroutine PGERAS

!------------------------------------------------------------------------
! Module: PGERR1 -- horizontal or vertical error bar
!------------------------------------------------------------------------

subroutine PGERR1 (DIR, X, Y, E, T)
 implicit none
 integer, intent(in) :: DIR
 real,    intent(in) :: X, Y, E
 real,    intent(in) :: T

end subroutine PGERR1

!------------------------------------------------------------------------
! Module: PGERRB -- horizontal or vertical error bar
!------------------------------------------------------------------------

subroutine PGERRB (DIR, N, X, Y, E, T)
 implicit none
 integer, intent(in) :: DIR, N
 real,    intent(in) :: X(*), Y(*), E(*)
 real,    intent(in) :: T

end subroutine PGERRB

!------------------------------------------------------------------------
! Module: PGERRX -- horizontal error bar
!------------------------------------------------------------------------

subroutine PGERRX (N, X1, X2, Y, T)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: X1(*), X2(*), Y(*)
 real,    intent(in) :: T

end subroutine PGERRX

!------------------------------------------------------------------------
! Module: PGERRY -- vertical error bar
!------------------------------------------------------------------------

subroutine PGERRY (N, X, Y1, Y2, T)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: X(*), Y1(*), Y2(*)
 real,    intent(in) :: T

end subroutine PGERRY

!------------------------------------------------------------------------
! Module: PGETXT -- erase text from graphics display
!------------------------------------------------------------------------
subroutine PGETXT
 implicit none
end subroutine PGETXT

!------------------------------------------------------------------------
! Module: PGFUNT -- function defined by X = F(T), Y = G(T)
!------------------------------------------------------------------------

subroutine PGFUNT (FX, FY, N, TMIN, TMAX, PGFLAG)
 implicit none
 real,    external   :: FX, FY
 integer, intent(in) :: N
 real,    intent(in) :: TMIN, TMAX
 integer, intent(in) :: PGFLAG

end subroutine PGFUNT

!------------------------------------------------------------------------
! Module: PGFUNX -- function defined by Y = F(X)
!------------------------------------------------------------------------

subroutine PGFUNX (FY, N, XMIN, XMAX, PGFLAG)
 implicit none
 real, external :: FY
 integer, intent(in) :: N
 real,    intent(in) :: XMIN, XMAX
 integer, intent(in) :: PGFLAG

end subroutine PGFUNX

!------------------------------------------------------------------------
! Module: PGFUNY -- function defined by X = F(Y)
!------------------------------------------------------------------------

subroutine PGFUNY (FX, N, YMIN, YMAX, PGFLAG)
 implicit none
 real, external :: FX
 integer, intent(in) :: N
 real,    intent(in) :: YMIN, YMAX
 integer, intent(in) :: PGFLAG

end subroutine PGFUNY

!------------------------------------------------------------------------
! Module: PGGRAY -- gray-scale map of a 2D data array
!------------------------------------------------------------------------

subroutine PGGRAY (A, IDIM, JDIM, I1, I2, J1, J2, FG, BG, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) :: A(IDIM,JDIM), FG, BG, TR(6)

end subroutine PGGRAY

!------------------------------------------------------------------------
! Module: PGHI2D -- cross-sections through a 2D data array
!------------------------------------------------------------------------

subroutine PGHI2D (DATA, NXV, NYV, IX1, IX2, IY1, IY2, X, IOFF, BIAS, CENTER, YLIMS)
 implicit none
 integer, intent(in) :: NXV, NYV, IX1, IX2, IY1, IY2
 real,    intent(in) :: DATA(NXV,NYV)
 real,    intent(in) :: X(IX2-IX1+1), YLIMS(IX2-IX1+1)
 integer, intent(in) :: IOFF
 real,    intent(in) :: BIAS
 logical, intent(in) :: CENTER

end subroutine PGHI2D

!------------------------------------------------------------------------
! Module: PGHIST -- histogram of unbinned data
!------------------------------------------------------------------------

subroutine PGHIST(N, DATA, DATMIN, DATMAX, NBIN, PGFLAG)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: DATA(*)
 real,    intent(in) :: DATMIN, DATMAX
 integer, intent(in) :: NBIN, PGFLAG

end subroutine PGHIST

!------------------------------------------------------------------------
! Module: PGIDEN -- write username, date, and time at bottom of plot
!------------------------------------------------------------------------
subroutine PGIDEN
 implicit none

end subroutine PGIDEN

!------------------------------------------------------------------------
! Module: PGIMAG -- color image from a 2D data array
!------------------------------------------------------------------------
subroutine PGIMAG (A, IDIM, JDIM, I1, I2, J1, J2, A1, A2, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) :: A(IDIM,JDIM), A1, A2, TR(6)

end subroutine PGIMAG

!------------------------------------------------------------------------
! Module: PGLAB -- write labels for x-axis, y-axis, and top of plot
!------------------------------------------------------------------------

subroutine PGLAB (XLBL, YLBL, TOPLBL)
 implicit none
 character*(*), intent(in) :: XLBL, YLBL, TOPLBL

end subroutine PGLAB

!------------------------------------------------------------------------
! Module: PGLCUR -- draw a line using the cursor
!------------------------------------------------------------------------

subroutine PGLCUR (MAXPT, NPT, X, Y)
 implicit none
 integer, intent(in) :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)

end subroutine PGLCUR

!------------------------------------------------------------------------
! Module: PGLDEV -- list available device types on standard output
!------------------------------------------------------------------------

subroutine PGLDEV
 implicit none

end subroutine PGLDEV

!------------------------------------------------------------------------
! Module: PGLEN -- find length of a string in a variety of units
!------------------------------------------------------------------------

subroutine PGLEN (UNITS, STRING, XL, YL)
 implicit none
 real,          intent(out) :: XL, YL
 integer,       intent(in) :: UNITS
 character*(*), intent(in) :: STRING

end subroutine PGLEN

!------------------------------------------------------------------------
! Module: PGLINE -- draw a polyline (curve defined by line-segments)
!------------------------------------------------------------------------

subroutine PGLINE (N, XPTS, YPTS)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)

end subroutine PGLINE

!------------------------------------------------------------------------
! Module: PGMOVE -- move pen (change current pen position)
!------------------------------------------------------------------------

subroutine PGMOVE (X, Y)
 implicit none
 real, intent(in) :: X, Y

end subroutine PGMOVE

!------------------------------------------------------------------------
! Module: PGMTXT -- write text at position relative to viewport
!------------------------------------------------------------------------

subroutine PGMTXT (SIDE, DISP, COORD, FJUST, TEXT)
 implicit none
 character*(*), intent(in) :: SIDE, TEXT
 real, intent(in) :: DISP, COORD, FJUST

end subroutine PGMTXT

!------------------------------------------------------------------------
! Module: PGNCUR -- mark a set of points using the cursor
!------------------------------------------------------------------------

subroutine PGNCUR (MAXPT, NPT, X, Y, SYMBOL)
 implicit none
 integer, intent(in)    :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)
 integer, intent(in)    :: SYMBOL

end subroutine PGNCUR

!------------------------------------------------------------------------
! Module: PGNUMB -- convert a number into a plottable character string
!------------------------------------------------------------------------

subroutine PGNUMB (MM, PP, FORM, STRING, NC)
 implicit none
 integer,       intent(in)  :: MM, PP, FORM
 character*(*), intent(out) :: STRING
 integer,       intent(out) :: NC

end subroutine PGNUMB

!------------------------------------------------------------------------
! Module: PGOLIN -- mark a set of points using the cursor
!------------------------------------------------------------------------

subroutine PGOLIN (MAXPT, NPT, X, Y, SYMBOL)
 implicit none
 integer, intent(in)    :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)
 integer, intent(in)    :: SYMBOL

end subroutine PGOLIN
!------------------------------------------------------------------------
! Module: PGOPEN -- open a graphics device
!------------------------------------------------------------------------

integer function PGOPEN (DEVICE)
 implicit none
 character*(*), intent(in) :: DEVICE

end function PGOPEN
!------------------------------------------------------------------------
! Module: PGPAGE -- advance to new page
!------------------------------------------------------------------------

subroutine PGPAGE

end subroutine PGPAGE

!------------------------------------------------------------------------
! Module: PGPANL -- switch to a different panel on the view surface
!------------------------------------------------------------------------

subroutine PGPANL(IX, IY)
 implicit none
 integer, intent(in) :: IX, IY

end subroutine PGPANL
!------------------------------------------------------------------------
! Module: PGPAP -- change the size of the view surface 
!------------------------------------------------------------------------

subroutine PGPAP (WIDTH, ASPECT)
 implicit none
 real, intent(in) :: WIDTH, ASPECT

end subroutine PGPAP

!------------------------------------------------------------------------
! Module: PGPIXL -- draw pixels
!------------------------------------------------------------------------

subroutine PGPIXL (IA, IDIM, JDIM, I1, I2, J1, J2, X1, X2, Y1, Y2)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 integer, intent(in) :: IA(IDIM,JDIM)
 real,    intent(in) :: X1, X2, Y1, Y2

end subroutine PGPIXL

!------------------------------------------------------------------------
! Module: PGPNTS -- draw several graph markers, not all the same
!------------------------------------------------------------------------

subroutine PGPNTS (N, X, Y, SYMBOL, NS)
 implicit none
 integer, intent(in) :: N, NS
 real,    intent(in) :: X(*), Y(*)
 integer, intent(in) :: SYMBOL(*)

end subroutine PGPNTS

!------------------------------------------------------------------------
! Module: PGPOLY -- draw a polygon, using fill-area attributes
!------------------------------------------------------------------------

subroutine PGPOLY (N, XPTS, YPTS)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)

end subroutine PGPOLY

!------------------------------------------------------------------------
! Module: PGPT -- draw several graph markers
!------------------------------------------------------------------------

subroutine PGPT (N, XPTS, YPTS, SYMBOL)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)
 integer, intent(in) :: SYMBOL

end subroutine PGPT

!------------------------------------------------------------------------
! Module: PGPT1 -- draw one graph marker
!------------------------------------------------------------------------

subroutine PGPT1 (XPT, YPT, SYMBOL)
 implicit none
 real,    intent(in) :: XPT, YPT
 integer, intent(in) :: SYMBOL

end subroutine PGPT1

!------------------------------------------------------------------------
! Module: PGPTXT -- write text at arbitrary position and angle
!------------------------------------------------------------------------

subroutine PGPTXT (X, Y, ANGLE, FJUST, TEXT)
 implicit none
 real,          intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT

end subroutine PGPTXT

!------------------------------------------------------------------------
! Module: PGQAH -- inquire arrow-head style
!------------------------------------------------------------------------

subroutine PGQAH (FS, ANGLE, BARB)
 implicit none
 integer, intent(out) :: FS
 real,    intent(out) :: ANGLE, BARB

end subroutine PGQAH

!------------------------------------------------------------------------
! Module: PGQCF -- inquire character font
!------------------------------------------------------------------------

subroutine PGQCF (FONT)
 implicit none
 integer, intent(out) :: FONT

end subroutine PGQCF

!------------------------------------------------------------------------
! Module: PGQCH -- inquire character height
!------------------------------------------------------------------------

subroutine PGQCH (SIZE)
 implicit none
 real, intent(out) :: SIZE

end subroutine PGQCH

!------------------------------------------------------------------------
! Module: PGQCI -- inquire color index
!------------------------------------------------------------------------

subroutine PGQCI (CI)
 implicit none
 integer, intent(out) :: CI

end subroutine PGQCI

!------------------------------------------------------------------------
! Module: PGQCIR -- inquire color index range
!------------------------------------------------------------------------

subroutine PGQCIR(ICILO, ICIHI)
 implicit none
 integer, intent(out) :: ICILO, ICIHI

end subroutine PGQCIR

!------------------------------------------------------------------------
! Module: PGQCLP -- inquire clipping status
!------------------------------------------------------------------------

subroutine PGQCLP(STATE)
 implicit none
 integer, intent(out) :: STATE

end subroutine PGQCLP

!------------------------------------------------------------------------
! Module: PGQCOL -- inquire color capability
!------------------------------------------------------------------------

subroutine PGQCOL (CI1, CI2)
 implicit none
 integer, intent(out) :: CI1, CI2

end subroutine PGQCOL

!------------------------------------------------------------------------
! Module: PGQCR  -- inquire color representation
!------------------------------------------------------------------------

subroutine PGQCR (CI, CR, CG, CB)
 implicit none
 integer, intent(in)  :: CI
 real,    intent(out) :: CR, CG, CB

end subroutine PGQCR

!------------------------------------------------------------------------
! Module: PGQCS  -- inquire character height in a variety of units
!------------------------------------------------------------------------

subroutine PGQCS(UNITS, XCH, YCH)
 implicit none
 integer, intent(in)  :: UNITS
 real,    intent(out) :: XCH, YCH

end subroutine PGQCS

!------------------------------------------------------------------------
! Module: PGQDT -- inquire name of nth available device type
!------------------------------------------------------------------------

subroutine PGQDT(N, TYPE, TLEN, DESCR, DLEN, INTER)
 implicit none
 integer, intent(in) :: N
 character*(*), intent(out) :: TYPE, DESCR
 integer, intent(out) :: TLEN, DLEN, INTER

end subroutine PGQDT

!------------------------------------------------------------------------
! Module: PGQFS -- inquire fill-area style
!------------------------------------------------------------------------

subroutine PGQFS (FS)
 implicit none
 integer, intent(out) :: FS

end subroutine PGQFS

!------------------------------------------------------------------------
! Module: PGQHS -- inquire hatching style
!------------------------------------------------------------------------

subroutine PGQHS (ANGLE, SEPN, PHASE)
 implicit none
 real, intent(out) :: ANGLE, SEPN, PHASE

end subroutine PGQHS

!------------------------------------------------------------------------
! Module: PGQID -- inquire current device identifier
!------------------------------------------------------------------------

subroutine PGQID (ID)
 implicit none
 integer  ID

end subroutine PGQID

!------------------------------------------------------------------------
! Module: PGQINF -- inquire PGPLOT general information
!------------------------------------------------------------------------

subroutine PGQINF (ITEM, VALUE, LENGTH)
 implicit none
 character*(*), intent(in) :: ITEM
 character*(*), intent(out) :: VALUE
 integer,       intent(out) :: LENGTH

end subroutine PGQINF

!------------------------------------------------------------------------
! Module: PGQITF -- inquire image transfer function
!------------------------------------------------------------------------

subroutine PGQITF (ITF)
 implicit none
 integer, intent(out) :: ITF

end subroutine PGQITF

!------------------------------------------------------------------------
! Module: PGQLS -- inquire line style
!------------------------------------------------------------------------

subroutine PGQLS (LS)
 implicit none
 integer, intent(out) :: LS

end subroutine PGQLS

!------------------------------------------------------------------------
! Module: PGQLW -- inquire line width
!------------------------------------------------------------------------

subroutine PGQLW (LW)
 implicit none
 integer, intent(out) :: LW

end subroutine PGQLW

!------------------------------------------------------------------------
! Module: PGQNDT -- inquire number of available device types
!------------------------------------------------------------------------

subroutine PGQNDT(N)
 implicit none
 integer, intent(out) :: N

end subroutine PGQNDT

!------------------------------------------------------------------------
! Module: PGQPOS -- inquire current pen position
!------------------------------------------------------------------------

subroutine PGQPOS (X, Y)
 implicit none
 real, intent(out) :: X, Y

end subroutine PGQPOS

!------------------------------------------------------------------------
! Module: PGQTBG -- inquire text background color index
!------------------------------------------------------------------------

subroutine PGQTBG (TBCI)
 implicit none
 integer, intent(in) :: TBCI

end subroutine PGQTBG

!------------------------------------------------------------------------
! Module: PGQTXT -- find bounding box of text string
!------------------------------------------------------------------------

subroutine PGQTXT (X, Y, ANGLE, FJUST, TEXT, XBOX, YBOX)
 implicit none
 real, intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT
 real, intent(out) :: XBOX(4), YBOX(4)

end subroutine PGQTXT

!------------------------------------------------------------------------
! Module: PGQVP -- inquire viewport size and position
!------------------------------------------------------------------------

subroutine PGQVP (UNITS, X1, X2, Y1, Y2)
 implicit none
 integer, intent(in) :: UNITS
 real,   intent(out) :: X1, X2, Y1, Y2

end subroutine PGQVP

!------------------------------------------------------------------------
! Module: PGQVSZ -- inquire size of view surface
!------------------------------------------------------------------------

subroutine PGQVSZ (UNITS, X1, X2, Y1, Y2)
 implicit none
 integer, intent(in)  :: UNITS
 real,    intent(out) :: X1, X2, Y1, Y2
end subroutine PGQVSZ

!------------------------------------------------------------------------
! Module: PGQWIN -- inquire window boundary coordinates
!------------------------------------------------------------------------

subroutine PGQWIN (X1, X2, Y1, Y2)
 implicit none
 real, intent(out) :: X1, X2, Y1, Y2

end subroutine PGQWIN

!------------------------------------------------------------------------
! Module: PGRECT -- draw a rectangle, using fill-area attributes
!------------------------------------------------------------------------

subroutine PGRECT (X1, X2, Y1, Y2)
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

end subroutine PGRECT

!------------------------------------------------------------------------
! Module: PGRND -- find the smallest `round' number greater than x
!------------------------------------------------------------------------

real function PGRND (X, NSUB)
 implicit none
 real,    intent(in)  :: X
 integer, intent(out) :: NSUB

end function PGRND

!------------------------------------------------------------------------
! Module: PGRNGE -- choose axis limits
!------------------------------------------------------------------------

subroutine PGRNGE (X1, X2, XLO, XHI)
 implicit none
 real, intent(in)  :: X1, X2
 real, intent(out) :: XLO, XHI

end subroutine PGRNGE

!------------------------------------------------------------------------
! Module: PGSAH -- set arrow-head style
!------------------------------------------------------------------------

subroutine PGSAH (FS, ANGLE, BARB)
 implicit none
 integer, intent(in) :: FS
 real,    intent(in) :: ANGLE, BARB

end subroutine PGSAH

!------------------------------------------------------------------------
! Module: PGSAVE -- save PGPLOT attributes
!------------------------------------------------------------------------

subroutine PGSAVE
 implicit none

end subroutine PGSAVE

!------------------------------------------------------------------------
! Module: PGUNSA -- restore PGPLOT attributes
!------------------------------------------------------------------------
!ENTRY PGUNSA
!end entry PGUNSA

subroutine PGUNSA
 implicit none

end subroutine PGUNSA

!------------------------------------------------------------------------
! Module: PGSCF -- set character font
!------------------------------------------------------------------------

subroutine PGSCF (FONT)
 implicit none
 integer, intent(in) :: FONT

end subroutine PGSCF

!------------------------------------------------------------------------
! Module: PGSCH -- set character height
!------------------------------------------------------------------------

subroutine PGSCH (SIZE)
 implicit none
 real, intent(in) :: SIZE

end subroutine PGSCH

!------------------------------------------------------------------------
! Module: PGSCI -- set color index
!------------------------------------------------------------------------

subroutine PGSCI (CI)
 implicit none
 integer, intent(in) :: CI

end subroutine PGSCI

!------------------------------------------------------------------------
! Module: PGSCIR -- set color index range
!------------------------------------------------------------------------

subroutine PGSCIR(ICILO, ICIHI)
 implicit none
 integer, intent(in) :: ICILO, ICIHI

end subroutine PGSCIR

!------------------------------------------------------------------------
! Module: PGSCLP -- enable or disable clipping at edge of viewport
!------------------------------------------------------------------------

subroutine PGSCLP(STATE)
 integer, intent(in) :: STATE

end subroutine PGSCLP

!------------------------------------------------------------------------
! Module: PGSCR -- set color representation
!------------------------------------------------------------------------

subroutine PGSCR (CI, CR, CG, CB)
 implicit none
 integer, intent(in) :: CI
 real,    intent(in) :: CR, CG, CB

end subroutine PGSCR

!------------------------------------------------------------------------
! Module: PGSCRL -- scroll window
!------------------------------------------------------------------------

subroutine PGSCRL (DX, DY)
 implicit none
 real, intent(in) :: DX, DY

end subroutine PGSCRL

!------------------------------------------------------------------------
! Module: PGSCRN -- set color representation by name
!------------------------------------------------------------------------

subroutine PGSCRN(CI, NAME, IER)
 implicit none
 integer,       intent(in)  :: CI
 character*(*), intent(in)  :: NAME
 integer,       intent(out) :: IER

end subroutine PGSCRN

!------------------------------------------------------------------------
! Module: PGSFS -- set fill-area style
!------------------------------------------------------------------------

subroutine PGSFS (FS)
 implicit none
 integer, intent(in) :: FS

end subroutine PGSFS

!------------------------------------------------------------------------
! Module: PGSHLS -- set color representation using HLS system
!------------------------------------------------------------------------

subroutine PGSHLS (CI, CH, CL, CS)
 implicit none
 integer, intent(in) :: CI
 real,    intent(in) :: CH, CL, CS

end subroutine PGSHLS

!------------------------------------------------------------------------
! Module: PGSHS -- set hatching style
!------------------------------------------------------------------------

subroutine PGSHS (ANGLE, SEPN, PHASE)
 implicit none
 real, intent(in) :: ANGLE, SEPN, PHASE

end subroutine PGSHS

!------------------------------------------------------------------------
! Module: PGSITF -- set image transfer function
!------------------------------------------------------------------------

subroutine PGSITF (ITF)
 implicit none
 integer, intent(in) :: ITF

end subroutine PGSITF

!------------------------------------------------------------------------
! Module: PGSLCT -- select an open graphics device
!------------------------------------------------------------------------

subroutine PGSLCT(ID)
 implicit none
 integer, intent(in) :: ID

end subroutine PGSLCT

!------------------------------------------------------------------------
! Module: PGSLS -- set line style
!------------------------------------------------------------------------

subroutine PGSLS (LS)
 implicit none
 integer, intent(in) :: LS
end subroutine PGSLS
!------------------------------------------------------------------------
! Module: PGSLW -- set line width
!------------------------------------------------------------------------

subroutine PGSLW (LW)
 implicit none
 integer, intent(in) :: LW
end subroutine PGSLW

!------------------------------------------------------------------------
! Module: PGSTBG -- set text background color index
!------------------------------------------------------------------------

subroutine PGSTBG (TBCI)
 implicit none
 integer, intent(in) :: TBCI

end subroutine PGSTBG

!------------------------------------------------------------------------
! Module: PGSUBP -- subdivide view surface into panels
!------------------------------------------------------------------------

subroutine PGSUBP (NXSUB, NYSUB)
 implicit none
 integer, intent(in) :: NXSUB, NYSUB

end subroutine PGSUBP

!------------------------------------------------------------------------
! Module: PGSVP -- set viewport (normalized device coordinates)
!------------------------------------------------------------------------

subroutine PGSVP (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

end subroutine PGSVP

!------------------------------------------------------------------------
! Module: PGSWIN -- set window
!------------------------------------------------------------------------

subroutine PGSWIN (X1, X2, Y1, Y2)
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

end subroutine PGSWIN

!------------------------------------------------------------------------
! Module: PGTBOX -- draw frame and write (DD) HH MM SS.S labelling
!------------------------------------------------------------------------

subroutine PGTBOX (XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)
 implicit none
 real,      intent(in) :: XTICK, YTICK
 integer,   intent(in) :: NXSUB, NYSUB
 character, intent(in) :: XOPT*(*), YOPT*(*)

end subroutine PGTBOX

!------------------------------------------------------------------------
! Module: PGTEXT -- write text (horizontal, left-justified)
!------------------------------------------------------------------------

subroutine PGTEXT (X, Y, TEXT)
 implicit none
 real,          intent(in) :: X, Y
 character*(*), intent(in) :: TEXT

end subroutine PGTEXT

!------------------------------------------------------------------------
! Module: PGTICK -- draw a single tick mark on an axis
!------------------------------------------------------------------------

subroutine PGTICK (X1, Y1, X2, Y2, V, TIKL, TIKR, DISP, ORIENT, STR)
 implicit none
 real, intent(in) :: X1, Y1, X2, Y2, V, TIKL, TIKR, DISP, ORIENT
 character*(*), intent(in) :: STR

end subroutine PGTICK

!------------------------------------------------------------------------
! Module: PGUPDT -- update display
!------------------------------------------------------------------------

subroutine PGUPDT
 implicit none

end subroutine PGUPDT

!------------------------------------------------------------------------
! Module: PGVECT -- vector map of a 2D data array, with blanking
!------------------------------------------------------------------------

subroutine PGVECT (A, B, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR, BLANK)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), B(IDIM, JDIM), TR(6), BLANK, C

end subroutine PGVECT

!------------------------------------------------------------------------
! Module: PGVSIZ -- set viewport (inches)
!------------------------------------------------------------------------

subroutine PGVSIZ (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

end subroutine PGVSIZ
!------------------------------------------------------------------------
! Module: PGVSTD -- set standard (default) viewport
!------------------------------------------------------------------------

subroutine PGVSTD
 implicit none

end subroutine PGVSTD

!------------------------------------------------------------------------
! Module: PGWEDG -- annotate an image plot with a wedge
!------------------------------------------------------------------------

subroutine PGWEDG(SIDE, DISP, WIDTH, FG, BG, LABEL)
 implicit none
 character *(*), intent(in) :: SIDE,LABEL
 real,           intent(in) :: DISP, WIDTH, FG, BG

end subroutine PGWEDG

!------------------------------------------------------------------------
! Module: PGWNAD -- set window and adjust viewport to same aspect ratio
!------------------------------------------------------------------------

subroutine PGWNAD (X1, X2, Y1, Y2)
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

end subroutine PGWNAD

!------------------------------------------------------------------------
! Module: PGADVANCE -- non-standard alias for PGPAGE
!------------------------------------------------------------------------

subroutine PGADVANCE
 implicit none
 
 call PGPAGE

end subroutine PGADVANCE

!------------------------------------------------------------------------
! Module: PGBEGIN -- non-standard alias for PGBEG
!------------------------------------------------------------------------

integer function PGBEGIN (UNIT, FILE, NXSUB, NYSUB)
 implicit none
 integer,       intent(in) :: UNIT
 character*(*), intent(in) :: FILE
 integer,       intent(in) :: NXSUB, NYSUB
 integer, external :: PGBEG

 PGBEGIN = PGBEG(UNIT,FILE,NXSUB,NYSUB)

end function PGBEGIN

!------------------------------------------------------------------------
! Module: PGCURSE -- non-standard alias for PGCURS
!------------------------------------------------------------------------

integer function PGCURSE (X, Y, CH)
 implicit none
 real,        intent(in) :: X, Y
 character*1, intent(in) :: CH
 integer, external :: PGCURS

 PGCURSE = PGCURS(X, Y, CH)

end function PGCURSE

!------------------------------------------------------------------------
! Module: PGLABEL -- non-standard alias for PGLAB
!------------------------------------------------------------------------

subroutine PGLABEL (XLBL, YLBL, TOPLBL)
 implicit none
 character*(*), intent(in) :: XLBL, YLBL, TOPLBL

 call PGLAB(XLBL, YLBL, TOPLBL)

end subroutine PGLABEL

!------------------------------------------------------------------------
! Module: PGMTEXT -- non-standard alias for PGMTXT
!------------------------------------------------------------------------

subroutine PGMTEXT (SIDE, DISP, COORD, FJUST, TEXT)
 implicit none
 character*(*), intent(in) :: SIDE, TEXT
 real,          intent(in) :: DISP, COORD, FJUST

 call PGMTXT(SIDE, DISP, COORD, FJUST, TEXT)

end subroutine PGMTEXT

!------------------------------------------------------------------------
! Module: PGNCURSE -- non-standard alias for PGNCUR
!------------------------------------------------------------------------

subroutine PGNCURSE (MAXPT, NPT, X, Y, SYMBOL)
 implicit none
 integer, intent(in)  :: MAXPT
 integer, intent(out) :: NPT
 real,    intent(out) :: X(*), Y(*)
 integer, intent(in)  :: SYMBOL

 CALL PGNCUR(MAXPT, NPT, X, Y, SYMBOL)

end subroutine PGNCURSE

!------------------------------------------------------------------------
! Module: PGPAPER -- non-standard alias for PGPAP
!------------------------------------------------------------------------

subroutine PGPAPER (WIDTH, ASPECT)
 implicit none
 real, intent(in) :: WIDTH, ASPECT

 call PGPAP(WIDTH, ASPECT)

end subroutine PGPAPER

!------------------------------------------------------------------------
! Module: PGPOINT -- non-standard alias for PGPT
!------------------------------------------------------------------------

subroutine PGPOINT (N, XPTS, YPTS, SYMBOL)
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)
 integer, intent(in) :: SYMBOL

 call PGPT(N, XPTS, YPTS, SYMBOL)

end subroutine PGPOINT

!------------------------------------------------------------------------
! Module: PGPTEXT -- non-standard alias for PGPTXT
!------------------------------------------------------------------------

subroutine PGPTEXT (X, Y, ANGLE, FJUST, TEXT)
 implicit none
 real, intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT

 call PGPTXT(X, Y, ANGLE, FJUST, TEXT)

end subroutine PGPTEXT

!------------------------------------------------------------------------
! Module: PGVPORT -- non-standard alias for PGSVP
!------------------------------------------------------------------------

subroutine PGVPORT (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call PGSVP(XLEFT, XRIGHT, YBOT, YTOP)

end subroutine PGVPORT

!------------------------------------------------------------------------
! Module: PGVSIZE -- non-standard alias for PGVSIZ
!------------------------------------------------------------------------

subroutine PGVSIZE (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call PGVSIZ(XLEFT, XRIGHT, YBOT, YTOP)
 
end subroutine PGVSIZE

!------------------------------------------------------------------------
! Module: PGVSTAND -- non-standard alias for PGVSTD
!------------------------------------------------------------------------

subroutine PGVSTAND
 implicit none

 call PGVSTD

end subroutine PGVSTAND

!------------------------------------------------------------------------
! Module: PGWINDOW -- non-standard alias for PGSWIN
!------------------------------------------------------------------------

subroutine PGWINDOW (X1, X2, Y1, Y2)
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

 call PGSWIN(X1, X2, Y1, Y2)

end subroutine PGWINDOW
