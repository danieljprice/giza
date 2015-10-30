!-----------------------------------------------------------------------
!  giza - a scientific plotting library built on cairo
!
!  Copyright (c) 2010      James Wetter and Daniel Price
!  Copyright (c) 2010-2012 Daniel Price
!
!  This library is free software; and you are welcome to redistribute
!  it under the terms of the GNU General Public License
!  (GPL, see LICENSE file for details) and the provision that
!   this notice remains intact. If you modify this file, please
!  note section 2a) of the GPLv2 states that:
!
!   a) You must cause the modified files to carry prominent notices
!      stating that you changed the files and the date of any change.
!
!  This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
!  See the GPL for specific language governing rights and limitations.
!
!  The Original code is the giza plotting library.
!
!  Contributor(s):
!       James Wetter <wetter.j@gmail.com>
!       Daniel Price <daniel.price@monash.edu> (main contact)
!
!-----------------------------------------------------------------------
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
! This is a utility module containing routines for converting
! various things from giza-style to pgplot-style where necessary
!------------------------------------------------------------------------
module gizapgplot
 implicit none
 !--Settings unique to PGPLOT
 integer, public :: pgfont = 1

contains
!------------------------------------------------------------
! Function to convert PGPLOT units value to giza units value
!------------------------------------------------------------
 integer function units_giza(pgplotunits)
  use giza, only:giza_units_normalized,giza_units_inches, &
                 giza_units_mm,giza_units_device,giza_units_world,&
                 giza_units_pixels
  implicit none
  integer, intent(in) :: pgplotunits
  
  select case(pgplotunits)
  case(0)
     units_giza = giza_units_normalized
  case(1)
     units_giza = giza_units_inches
  case(2)
     units_giza = giza_units_mm
  case(3)
     units_giza = giza_units_pixels
  case(4)
     units_giza = giza_units_world
  case default  ! giza will give an error
     units_giza = pgplotunits
  end select

 end function units_giza

!----------------------------------------------------------------
! Routine to convert PGPLOT "trans" array to giza "affine" array
!----------------------------------------------------------------
 subroutine convert_tr_to_affine(tr,affine)
  implicit none
  real, dimension(6), intent(in)  :: tr
  real, dimension(6), intent(out) :: affine

  affine(1) = TR(2)
  affine(2) = TR(5)
  affine(3) = TR(3)
  affine(4) = TR(6)
  affine(5) = TR(1) + 0.5 * TR(2)
  affine(6) = TR(4) + 0.5 * TR(6)

 end subroutine convert_tr_to_affine

end module gizapgplot

!------------------------------------------------------------------------
! Module: PGARRO -- draw an arrow
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGARRO (X1, Y1, X2, Y2)
 use giza, only:giza_arrow
 implicit none
 real, intent(in) :: X1, Y1, X2, Y2

 call giza_arrow(X1, Y1, X2, Y2)

end subroutine pgarro

!------------------------------------------------------------------------
! Module: PGASK -- control new page prompting
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGASK (FLAG)
 use giza, only:giza_start_prompting, giza_stop_prompting
 implicit none
 logical, intent(in) :: FLAG

 if (FLAG) then
   call giza_start_prompting
 else
   call giza_stop_prompting
 end if

end subroutine PGASK

!------------------------------------------------------------------------
! Module: PGAXIS -- draw an axis
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGAXIS (OPT, X1, Y1, X2, Y2, V1, V2, STEP, NSUB, DMAJL, DMAJR, FMIN, DISP, ORIENT)
 implicit none
 character*(*), intent(in) :: OPT
 real, intent(in) :: X1, Y1, X2, Y2, V1, V2, STEP, DMAJL, DMAJR, FMIN, DISP
 real, intent(in) :: ORIENT
 integer, intent(in) :: NSUB

end subroutine PGAXIS

!------------------------------------------------------------------------
! Module: PGBAND -- read cursor position, with anchor
! Status: IMPLEMENTED
!------------------------------------------------------------------------
integer function PGBAND (MODE, POSN, XREF, YREF, X, Y, CH)
 use giza, only:giza_band
 implicit none
 integer, intent(in) :: MODE, POSN
 real, intent(in)    :: XREF, YREF
 real, intent(inout) :: X, Y
 character*(*), intent(out) :: CH
 integer :: ierr

 ierr = giza_band(MODE, POSN, XREF, YREF, X, Y, CH)

 !--PGBAND returns 1 on successful completion
 if (ierr.eq.0) then
    PGBAND = 1
 else
    PGBAND = 0
 endif

end function PGBAND

!------------------------------------------------------------------------
! Module: PGBBUF -- begin batch of output (buffer)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGBBUF
 use giza, only:giza_begin_buffer
 implicit none
 
 call giza_begin_buffer()

end subroutine PGBBUF

!------------------------------------------------------------------------
! Module: PGBEG -- open a graphics device
! Status: IMPLEMENTED
!------------------------------------------------------------------------
integer function PGBEG (UNIT, FILE, NXSUB, NYSUB)
 implicit none
 integer,       intent(in) :: UNIT
 character*(*), intent(in) :: FILE
 integer,       intent(in) :: NXSUB, NYSUB
 integer, external :: PGOPEN

 PGBEG = PGOPEN(FILE)
 if (PGBEG > 0 .and. (NXSUB.GT.1 .or. NYSUB.GT.1)) then
    call PGSUBP(NXSUB, NYSUB)
 endif

end function PGBEG

!------------------------------------------------------------------------
! Module: PGBIN -- histogram of binned data
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGBIN (NBIN, X, DATA, CENTER)
 use giza, only:giza_histogram_binned
 implicit none
 integer, intent(in) :: NBIN
 real,    intent(in) :: X(*), DATA(*)
 logical, intent(in) :: CENTER
 integer :: iflag

 if (CENTER) then
    iflag = 1
 else
    iflag = 0
 endif 

 call giza_histogram_binned(NBIN, X, DATA, iflag)

end subroutine PGBIN

!------------------------------------------------------------------------
! Module: PGBOX -- draw labeled frame around viewport
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGBOX (XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)
 use giza, only:giza_box
 implicit none
 character*(*), intent(in) :: XOPT, YOPT
 real,          intent(in) :: XTICK, YTICK
 integer,       intent(in) :: NXSUB, NYSUB

 call giza_box(XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)

end subroutine PGBOX

!------------------------------------------------------------------------
! Module: PGCIRC -- draw a circle, using fill-area attributes
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCIRC (XCENT, YCENT, RADIUS)
 use giza, only:giza_circle
 implicit none
 real, intent(in) :: XCENT, YCENT, RADIUS

 call giza_circle(XCENT,YCENT,RADIUS)

end subroutine PGCIRC
!------------------------------------------------------------------------
! Module: PGCLOS -- close the selected graphics device
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCLOS
 use giza, only:giza_close_device
 implicit none

 call giza_close_device

end subroutine PGCLOS

!------------------------------------------------------------------------
! Module: PGCONB -- contour map of a 2D data array, with blanking
! Status: PARTIALLY IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONB (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR, BLANK)
 use giza,       only:giza_contour
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6), BLANK
 real                :: affine(6)

 call convert_tr_to_affine(tr,affine)
 call giza_contour(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,abs(nc),C,affine)

end subroutine PGCONB

!------------------------------------------------------------------------
! Module: PGCONF -- fill between two contours
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONF (A, IDIM, JDIM, I1, I2, J1, J2, C1, C2, TR)
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) :: A(IDIM,JDIM), C1, C2, TR(6)

end subroutine PGCONF

!------------------------------------------------------------------------
! Module: PGCONL -- label contour map of a 2D data array 
! Status: PARTIALLY IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONL (A, IDIM, JDIM, I1, I2, J1, J2, C, TR, LABEL, INTVAL, MININT)
 use giza,       only:giza_contour
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer,       intent(in) :: IDIM, JDIM, I1, J1, I2, J2, INTVAL, MININT
 real,          intent(in) :: A(IDIM,JDIM), C, TR(6)
 character*(*), intent(in) :: LABEL
 real                      :: affine(6)
 integer, parameter  :: nc = 1
 real, dimension(nc) :: cc
 
 cc(1) = C
 call convert_tr_to_affine(tr,affine)
 call giza_contour(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,nc,cc,affine)

end subroutine PGCONL

!------------------------------------------------------------------------
! Module: PGCONS -- contour map of a 2D data array (fast algorithm)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONS (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR)
 use giza,       only:giza_contour
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6)
 real                :: affine(6)

 call convert_tr_to_affine(tr,affine)
 call giza_contour(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,abs(nc),C,affine)

end subroutine PGCONS

!------------------------------------------------------------------------
! Module: PGCONT -- contour map of a 2D data array (contour-following)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONT (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR)
 use giza,       only:giza_contour
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, J1, I2, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*), TR(6)
 real                :: affine(6)

 call convert_tr_to_affine(tr,affine)
 call giza_contour(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,abs(nc),C,affine)

end subroutine PGCONT

!------------------------------------------------------------------------
! Module: PGCONX -- contour map of a 2D data array (non rectangular)
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCONX (A, IDIM, JDIM, I1, I2, J1, J2, C, NC, PLOT)
 use giza,       only:giza_contour
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, J1, I2, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), C(*)
 external :: PLOT

end subroutine PGCONX

!------------------------------------------------------------------------
! Module: PGCTAB -- install the color table to be used by PGIMAG
! Status: PARTIALLY IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGCTAB(L, R, G, B, NC, CONTRA, BRIGHT)
 use giza, only:giza_set_colour_table
 implicit none
 integer, intent(in) :: NC
 real,    intent(in) :: L(NC), R(NC), G(NC), B(NC), CONTRA, BRIGHT

 call giza_set_colour_table(L, R, G, B, NC, CONTRA, BRIGHT)

end subroutine PGCTAB

!------------------------------------------------------------------------
! Module: PGCURS -- read cursor position
! Status: IMPLEMENTED
!------------------------------------------------------------------------
integer function PGCURS (X, Y, CH)
 use giza, only:giza_get_key_press
 implicit none
 real,          intent(inout) :: X, Y
 character*(*), intent(out)   :: CH

 PGCURS = giza_get_key_press(X, Y, CH)

end function PGCURS

!------------------------------------------------------------------------
! Module: PGDRAW -- draw a line from the current pen position to a point
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGDRAW (X, Y)
 use giza, only:giza_draw
 implicit none
 real, intent(in) :: X, Y
 
 call giza_draw(X, Y)

end subroutine PGDRAW

!------------------------------------------------------------------------
! Module: PGEBUF -- end batch of output (buffer)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGEBUF
 use giza, only:giza_end_buffer
 implicit none
 
 call giza_end_buffer()
 
end subroutine PGEBUF

!------------------------------------------------------------------------
! Module: PGEND -- close all open graphics devices
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGEND
 use giza, only:giza_close_device
 implicit none
 
 call giza_close_device()
 
end subroutine PGEND

!------------------------------------------------------------------------
! Module: PGENV -- set window and viewport and draw labeled frame
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGENV (XMIN, XMAX, YMIN, YMAX, JUST, AXIS)
 use giza, only:giza_set_environment
 implicit none
 real,    intent(in) :: XMIN, XMAX, YMIN, YMAX
 integer, intent(in) :: JUST, AXIS

 call giza_set_environment(XMIN, XMAX, YMIN, YMAX, JUST, AXIS)

end subroutine PGENV

!------------------------------------------------------------------------
! Module: PGERAS -- erase all graphics from current page
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGERAS
 use giza, only:giza_draw_background
 implicit none
 
 call giza_draw_background()
 
end subroutine PGERAS

!------------------------------------------------------------------------
! Module: PGERR1 -- horizontal or vertical error bar
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGERR1 (DIR, X, Y, E, T)
 implicit none
 integer, intent(in) :: DIR
 real,    intent(in) :: X, Y, E
 real,    intent(in) :: T
 real, dimension(1) :: xi,yi,ei
  
 xi(1) = x
 yi(1) = y
 ei(1) = e
 call PGERRB(DIR,1,XI,YI,EI,T)

end subroutine PGERR1

!------------------------------------------------------------------------
! Module: PGERRB -- horizontal or vertical error bar
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGERRB (DIR, N, X, Y, E, T)
 use giza, only:giza_error_bars
 implicit none
 integer, intent(in) :: DIR, N
 real,    intent(in) :: X(*), Y(*), E(*)
 real,    intent(in) :: T

 call giza_error_bars(DIR, N, X, Y, E, T)

end subroutine PGERRB

!------------------------------------------------------------------------
! Module: PGERRX -- horizontal error bar
! Status: IMPLEMENTED
!------------------------------------------------------------------------

subroutine PGERRX (N, X1, X2, Y, T)
 use giza, only:giza_error_bars_x
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: X1(*), X2(*), Y(*)
 real,    intent(in) :: T

 call giza_error_bars_x(N, X1, X2, Y, T)

end subroutine PGERRX

!------------------------------------------------------------------------
! Module: PGERRY -- vertical error bar
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGERRY (N, X, Y1, Y2, T)
 use giza, only:giza_error_bars_y
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: X(*), Y1(*), Y2(*)
 real,    intent(in) :: T
 
 call giza_error_bars_y(N, X, Y1, Y2, T)

end subroutine PGERRY

!------------------------------------------------------------------------
! Module: PGETXT -- erase text from graphics display
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGETXT
 implicit none
end subroutine PGETXT

!------------------------------------------------------------------------
! Module: PGFUNT -- function defined by X = F(T), Y = G(T)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGFUNT (FX, FY, N, TMIN, TMAX, PGFLAG)
 use giza, only:giza_function_t
 implicit none
 real,    external   :: FX, FY
 integer, intent(in) :: N
 real,    intent(in) :: TMIN, TMAX
 integer, intent(in) :: PGFLAG

 call giza_function_t(FX, FY, N, TMIN, TMAX, PGFLAG)

end subroutine PGFUNT

!------------------------------------------------------------------------
! Module: PGFUNX -- function defined by Y = F(X)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGFUNX (FY, N, XMIN, XMAX, PGFLAG)
 use giza, only:giza_function_x
 implicit none
 real, external :: FY
 integer, intent(in) :: N
 real,    intent(in) :: XMIN, XMAX
 integer, intent(in) :: PGFLAG
 
 call giza_function_x(FY, N, XMIN, XMAX, PGFLAG)

end subroutine PGFUNX

!------------------------------------------------------------------------
! Module: PGFUNY -- function defined by X = F(Y)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGFUNY (FX, N, YMIN, YMAX, PGFLAG)
 use giza, only:giza_function_y
 implicit none
 real, external :: FX
 integer, intent(in) :: N
 real,    intent(in) :: YMIN, YMAX
 integer, intent(in) :: PGFLAG

 call giza_function_y(FX, N, YMIN, YMAX, PGFLAG)

end subroutine PGFUNY

!------------------------------------------------------------------------
! Module: PGGRAY -- gray-scale map of a 2D data array
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGGRAY (A, IDIM, JDIM, I1, I2, J1, J2, FG, BG, TR)
 use giza,       only:giza_render_gray,giza_extend_none
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) :: A(IDIM,JDIM), FG, BG, TR(6)
 real, dimension(6)  :: affine

 call convert_tr_to_affine(tr,affine)
 call giza_render_gray(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,fg,bg,giza_extend_none,affine)

end subroutine PGGRAY

!------------------------------------------------------------------------
! Module: PGHI2D -- cross-sections through a 2D data array
! Status: NOT IMPLEMENTED
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
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGHIST(N, DATA, DATMIN, DATMAX, NBIN, PGFLAG)
 use giza, only:giza_histogram
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: DATA(*)
 real,    intent(in) :: DATMIN, DATMAX
 integer, intent(in) :: NBIN, PGFLAG

 call giza_histogram(N,DATA,DATMIN,DATMAX,NBIN,PGFLAG)

end subroutine PGHIST

!------------------------------------------------------------------------
! Module: PGIDEN -- write username, date, and time at bottom of plot
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGIDEN
 use giza, only:giza_print_id
 implicit none
 
 call giza_print_id()

end subroutine PGIDEN

!------------------------------------------------------------------------
! Module: PGIMAG -- color image from a 2D data array
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGIMAG (A, IDIM, JDIM, I1, I2, J1, J2, A1, A2, TR)
 use giza,       only:giza_render,giza_extend_none
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 real,    intent(in) :: A(IDIM,JDIM), A1, A2, TR(6)
 real                :: affine(6)

 call convert_tr_to_affine(tr,affine)
 call giza_render(idim,jdim,a,i1-1,i2-1,j1-1,j2-1,a1,a2,giza_extend_none,affine)

end subroutine PGIMAG

!------------------------------------------------------------------------
! Module: PGLAB -- write labels for x-axis, y-axis, and top of plot
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLAB (XLBL, YLBL, TOPLBL)
 use giza, only:giza_label
 implicit none
 character*(*), intent(in) :: XLBL, YLBL, TOPLBL

 call giza_label(XLBL,YLBL,TOPLBL)

end subroutine PGLAB

!------------------------------------------------------------------------
! Module: PGLCUR -- draw a line using the cursor
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLCUR (MAXPT, NPT, X, Y)
 use giza, only:giza_mark_line
 implicit none
 integer, intent(in) :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)

 call giza_mark_line(MAXPT, NPT, X, Y)

end subroutine PGLCUR

!------------------------------------------------------------------------
! Module: PGLDEV -- list available device types on standard output
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLDEV
 use giza, only:giza_print_device_list
 implicit none
 
 call giza_print_device_list()

end subroutine PGLDEV

!------------------------------------------------------------------------
! Module: PGLEN -- find length of a string in a variety of units
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLEN (UNITS, STRING, XL, YL)
 use gizapgplot, only:units_giza
 use giza,       only:giza_qtextlen
 implicit none
 real,          intent(out) :: XL, YL
 integer,       intent(in) :: UNITS
 character*(*), intent(in) :: STRING

 call giza_qtextlen(units_giza(UNITS),STRING,XL,YL)

end subroutine PGLEN

!------------------------------------------------------------------------
! Module: PGLINE -- draw a polyline (curve defined by line-segments)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLINE (N, XPTS, YPTS)
 use giza, only:giza_line
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)

 call giza_line(N,XPTS,YPTS)

end subroutine PGLINE

!------------------------------------------------------------------------
! Module: PGMOVE -- move pen (change current pen position)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGMOVE (X, Y)
 use giza, only:giza_move
 implicit none
 real, intent(in) :: X, Y

 call giza_move(X, Y)

end subroutine PGMOVE

!------------------------------------------------------------------------
! Module: PGMTXT -- write text at position relative to viewport
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGMTXT (SIDE, DISP, COORD, FJUST, TEXT)
 use giza, only:giza_annotate
 implicit none
 character*(*), intent(in) :: SIDE, TEXT
 real, intent(in) :: DISP, COORD, FJUST

 call giza_annotate(SIDE,DISP,COORD,FJUST,TEXT)

end subroutine PGMTXT

!------------------------------------------------------------------------
! Module: PGNCUR -- mark a set of points using the cursor
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGNCUR (MAXPT, NPT, X, Y, SYMBOL)
 use giza, only:giza_mark_points_ordered
 implicit none
 integer, intent(in)    :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)
 integer, intent(in)    :: SYMBOL

 call giza_mark_points_ordered(MAXPT, NPT, X, Y, SYMBOL)

end subroutine PGNCUR

!------------------------------------------------------------------------
! Module: PGNUMB -- convert a number into a plotable character string
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGNUMB (MM, PP, FORM, STRING, NC)
 use giza, only:giza_format_number
 implicit none
 integer,       intent(in)  :: MM, PP, FORM
 character*(*), intent(out) :: STRING
 integer,       intent(out) :: NC

 call giza_format_number(mm,pp,form,string)
 nc = len_trim(string)

end subroutine PGNUMB

!------------------------------------------------------------------------
! Module: PGOLIN -- mark a set of points using the cursor
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGOLIN (MAXPT, NPT, X, Y, SYMBOL)
 use giza, only:giza_mark_points
 implicit none
 integer, intent(in)    :: MAXPT
 integer, intent(inout) :: NPT
 real,    intent(inout) :: X(*), Y(*)
 integer, intent(in)    :: SYMBOL

 call giza_mark_points(maxpt,npt,x,y,symbol)

end subroutine PGOLIN

!------------------------------------------------------------------------
! Module: PGOPEN -- open a graphics device
! Status: IMPLEMENTED
!------------------------------------------------------------------------
integer function PGOPEN (DEVICE)
 use giza, only:giza_open_device,giza_set_colour_palette,giza_colour_palette_pgplot
! use giza, only:giza_open_device_size,giza_units_mm,giza_units_inches
 implicit none
 character*(*), intent(in) :: DEVICE

! print*,'giza units mm = ',giza_units_mm
! pgopen = giza_open_device_size(device,'giza',11.0,8.5,giza_units_inches)
 pgopen = giza_open_device(device,'giza')
 
 call giza_set_colour_palette(giza_colour_palette_pgplot)

end function PGOPEN

!------------------------------------------------------------------------
! Module: PGPAGE -- advance to new page
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPAGE
 use giza, only:giza_change_page
 implicit none
 
 call giza_change_page

end subroutine PGPAGE

!------------------------------------------------------------------------
! Module: PGPANL -- switch to a different panel on the view surface
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPANL(IX, IY)
 use giza, only:giza_set_panel
 implicit none
 integer, intent(in) :: IX, IY

 call giza_set_panel(IX, IY)

end subroutine PGPANL

!------------------------------------------------------------------------
! Module: PGPAP -- change the size of the view surface 
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPAP (WIDTH, ASPECT)
 use giza, only:giza_set_paper_size, giza_units_inches
 implicit none
 real, intent(in) :: WIDTH, ASPECT

 call giza_set_paper_size(giza_units_inches,WIDTH,WIDTH*ASPECT)

end subroutine PGPAP

!------------------------------------------------------------------------
! Module: PGPIXL -- draw pixels
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPIXL (IA, IDIM, JDIM, I1, I2, J1, J2, X1, X2, Y1, Y2)
 use giza, only:giza_draw_pixels,giza_extend_none
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2
 integer, intent(in) :: IA(IDIM,JDIM)
 real,    intent(in) :: X1, X2, Y1, Y2

 call giza_draw_pixels(IDIM, JDIM, IA, I1-1, I2-1, J1-1, J2-1, X1, X2, Y1, Y2,&
                       giza_extend_none)

end subroutine PGPIXL

!------------------------------------------------------------------------
! Module: PGPNTS -- draw several graph markers, not all the same
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPNTS (N, X, Y, SYMBOL, NS)
 implicit none
 integer, intent(in) :: N, NS
 real,    intent(in) :: X(*), Y(*)
 integer, intent(in) :: SYMBOL(*)

end subroutine PGPNTS

!------------------------------------------------------------------------
! Module: PGPOLY -- draw a polygon, using fill-area attributes
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPOLY (N, XPTS, YPTS)
 use giza, only:giza_polygon
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)

 call giza_polygon(N, XPTS, YPTS)

end subroutine PGPOLY

!------------------------------------------------------------------------
! Module: PGPT -- draw several graph markers
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPT (N, XPTS, YPTS, SYMBOL)
 use giza, only:giza_points
 implicit none
 integer, intent(in) :: N
 real,    intent(in) :: XPTS(*), YPTS(*)
 integer, intent(in) :: SYMBOL

 call giza_points(n,xpts,ypts,symbol)

end subroutine PGPT

!------------------------------------------------------------------------
! Module: PGPT1 -- draw one graph marker
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPT1 (XPT, YPT, SYMBOL)
 use giza, only:giza_single_point
 implicit none
 real,    intent(in) :: XPT, YPT
 integer, intent(in) :: SYMBOL

 call giza_single_point(xpt,ypt,symbol)

end subroutine PGPT1

!------------------------------------------------------------------------
! Module: PGPTXT -- write text at arbitrary position and angle
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPTXT (X, Y, ANGLE, FJUST, TEXT)
 use giza, only:giza_ptext
 implicit none
 real,          intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT

 call giza_ptext(X, Y, ANGLE, FJUST, TEXT)

end subroutine PGPTXT

!------------------------------------------------------------------------
! Module: PGQAH -- inquire arrow-head style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQAH (FS, ANGLE, BARB)
 use giza, only:giza_get_arrow_style
 implicit none
 integer, intent(out) :: FS
 real,    intent(out) :: ANGLE, BARB

 call giza_get_arrow_style(FS, ANGLE, BARB)

end subroutine PGQAH

!------------------------------------------------------------------------
! Module: PGQCF -- inquire character font
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCF (FONT)
 use gizapgplot, only:pgfont
 implicit none
 integer, intent(out) :: FONT

 !
 !--this is a setting for the PGPLOT frontend only
 !  (giza has much more general fonts than PGPLOT)
 !
 !  the value of pgfont is set by the call to PGSCF 
 !
 FONT = pgfont

end subroutine PGQCF

!------------------------------------------------------------------------
! Module: PGQCH -- inquire character height
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCH (SIZE)
 use giza, only:giza_get_character_height
 implicit none
 real, intent(out) :: SIZE
 
 call giza_get_character_height(SIZE)

end subroutine PGQCH

!------------------------------------------------------------------------
! Module: PGQCI -- inquire color index
! Status: IMPLEMENTED
!------------------------------------------------------------------------

subroutine PGQCI (CI)
 use giza, only:giza_get_colour_index
 implicit none
 integer, intent(out) :: CI

 call giza_get_colour_index(CI)

end subroutine PGQCI

!------------------------------------------------------------------------
! Module: PGQCIR -- inquire color index range
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCIR(ICILO, ICIHI)
 use giza, only:giza_get_colour_index_range
 implicit none
 integer, intent(out) :: ICILO, ICIHI

 call giza_get_colour_index_range(ICILO,ICIHI)

end subroutine PGQCIR

!------------------------------------------------------------------------
! Module: PGQCLP -- inquire clipping status
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCLP(STATE)
 use giza, only:giza_get_clipping
 implicit none
 integer, intent(out) :: STATE
 
 call giza_get_clipping(STATE)

end subroutine PGQCLP

!------------------------------------------------------------------------
! Module: PGQCOL -- inquire color capability
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCOL (CI1, CI2)
 use giza, only:giza_colour_index_min,giza_colour_index_max
 implicit none
 integer, intent(out) :: CI1, CI2

 CI1 = giza_colour_index_min
 CI2 = giza_colour_index_max

end subroutine PGQCOL

!------------------------------------------------------------------------
! Module: PGQCR  -- inquire color representation
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCR (CI, CR, CG, CB)
 use giza, only:giza_get_colour_representation
 implicit none
 integer, intent(in)  :: CI
 real,    intent(out) :: CR, CG, CB

 call giza_get_colour_representation(CI, CR, CG, CB)

end subroutine PGQCR

!------------------------------------------------------------------------
! Module: PGQCS  -- inquire character height in a variety of units
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQCS(UNITS, XCH, YCH)
 use giza,       only:giza_get_character_size
 use gizapgplot, only:units_giza
 implicit none
 integer, intent(in)  :: UNITS
 real,    intent(out) :: XCH, YCH

 call giza_get_character_size(units_giza(UNITS), XCH, YCH)

end subroutine PGQCS

!------------------------------------------------------------------------
! Module: PGQDT -- inquire name of nth available device type
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQDT(N, TYPE, TLEN, DESCR, DLEN, INTER)
 implicit none
 integer, intent(in) :: N
 character*(*), intent(out) :: TYPE, DESCR
 integer, intent(out) :: TLEN, DLEN, INTER

end subroutine PGQDT

!------------------------------------------------------------------------
! Module: PGQFS -- inquire fill-area style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQFS (FS)
 use giza, only:giza_get_fill
 implicit none
 integer, intent(out) :: FS
 
 call giza_get_fill(FS)

end subroutine PGQFS

!------------------------------------------------------------------------
! Module: PGQHS -- inquire hatching style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQHS (ANGLE, SEPN, PHASE)
 use giza, only:giza_get_hatching_style
 implicit none
 real, intent(out) :: ANGLE, SEPN, PHASE

 call giza_get_hatching_style(ANGLE,SEPN,PHASE)

end subroutine PGQHS

!------------------------------------------------------------------------
! Module: PGQID -- inquire current device identifier
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQID (ID)
 use giza, only:giza_get_device_id
 implicit none
 integer, intent(out) :: ID

 call giza_get_device_id(ID)

end subroutine PGQID

!------------------------------------------------------------------------
! Module: PGQINF -- inquire PGPLOT general information
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQINF (ITEM, VALUE, LENGTH)
 use giza, only:giza_query_device,giza_version_string
 implicit none
 character*(*), intent(in) :: ITEM
 character*(*), intent(out) :: VALUE
 integer,       intent(out) :: LENGTH

 character(len=10) :: datestring,timestring

 select case(trim(item))
 case('VERSION','version')
    value = 'giza-'//trim(giza_version_string)
 case('STATE','state')
    call giza_query_device('state',value)
 case('USER','user')
    call giza_query_device('user',value)
 case('NOW','now')
    call date_and_time(datestring,timestring)
    value = datestring(7:8)//'-'//datestring(5:6)//'-'//datestring(1:4)// &
            ' '//timestring(1:2)//':'//timestring(3:4)
 case('DEVICE','device')
    call giza_query_device('device',value)
 case('FILE','file')
    call giza_query_device('file',value)
 case('TYPE','type')
    call giza_query_device('type',value)
 case('DEV/TYPE','dev/type')
    call giza_query_device('dev/type',value)
 case('HARDCOPY','hardcopy')
    call giza_query_device('hardcopy',value)
 case('TERMINAL','terminal')
    !--in giza the current device is never the terminal
    value = 'NO'
 case('CURSOR','cursor')
    call giza_query_device('cursor',value)
 case('SCROLL','scroll')
    !--no scroll capability in any current giza devices
    value = 'NO'
 case default
    value = ' '
 end select
 length = len_trim(value)

end subroutine PGQINF

!------------------------------------------------------------------------
! Module: PGQITF -- inquire image transfer function
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQITF (ITF)
 implicit none
 integer, intent(out) :: ITF

end subroutine PGQITF

!------------------------------------------------------------------------
! Module: PGQLS -- inquire line style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQLS (LS)
 use giza, only:giza_get_line_style
 implicit none
 integer, intent(out) :: LS
 
 call giza_get_line_style(LS)

end subroutine PGQLS

!------------------------------------------------------------------------
! Module: PGQLW -- inquire line width
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQLW (LW)
 use giza, only:giza_get_line_width
 implicit none
 integer, intent(out) :: LW
 real :: giza_lw
 
 call giza_get_line_width(giza_lw)
 LW = int(2.*giza_lw)

end subroutine PGQLW

!------------------------------------------------------------------------
! Module: PGQNDT -- inquire number of available device types
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQNDT(N)
 implicit none
 integer, intent(out) :: N
 
 N = 1

end subroutine PGQNDT

!------------------------------------------------------------------------
! Module: PGQPOS -- inquire current pen position
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQPOS (X, Y)
 use giza, only:giza_get_current_point
 implicit none
 real, intent(out) :: X, Y

 call giza_get_current_point(X, Y)

end subroutine PGQPOS

!------------------------------------------------------------------------
! Module: PGQTBG -- inquire text background color index
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQTBG (TBCI)
 use giza, only:giza_get_text_background
 implicit none
 integer, intent(out) :: TBCI
 
 call giza_get_text_background(TBCI)

end subroutine PGQTBG

!------------------------------------------------------------------------
! Module: PGQTXT -- find bounding box of text string
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQTXT (X, Y, ANGLE, FJUST, TEXT, XBOX, YBOX)
 use giza, only:giza_qtext
 implicit none
 real, intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT
 real, intent(out) :: XBOX(4), YBOX(4)

 call giza_qtext(X, Y, ANGLE, FJUST, TEXT, XBOX, YBOX)

end subroutine PGQTXT

!------------------------------------------------------------------------
! Module: PGQVP -- inquire viewport size and position
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQVP (UNITS, X1, X2, Y1, Y2)
 use giza,       only:giza_get_viewport
 use gizapgplot, only:units_giza
 implicit none
 integer, intent(in) :: UNITS
 real,   intent(out) :: X1, X2, Y1, Y2

 call giza_get_viewport(units_giza(UNITS), X1, X2, Y1, Y2)

end subroutine PGQVP

!------------------------------------------------------------------------
! Module: PGQVSZ -- inquire size of view surface
! Status: IMPLEMENTED
!-----------------------------------------------------------------------
subroutine PGQVSZ (UNITS, X1, X2, Y1, Y2)
 use giza,       only:giza_get_paper_size
 use gizapgplot, only:units_giza
 implicit none
 integer, intent(in)  :: UNITS
 real,    intent(out) :: X1, X2, Y1, Y2
 
 X1 = 0.
 Y1 = 0.
 call giza_get_paper_size(units_giza(UNITS),X2,Y2)
 
end subroutine PGQVSZ

!------------------------------------------------------------------------
! Module: PGQWIN -- inquire window boundary coordinates
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGQWIN (X1, X2, Y1, Y2)
 use giza, only:giza_get_window
 implicit none
 real, intent(out) :: X1, X2, Y1, Y2

 call giza_get_window(X1, X2, Y1, Y2)

end subroutine PGQWIN

!------------------------------------------------------------------------
! Module: PGRECT -- draw a rectangle, using fill-area attributes
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGRECT (X1, X2, Y1, Y2)
 use giza, only:giza_rectangle
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

 call giza_rectangle(X1, X2, Y1, Y2)

end subroutine PGRECT

!------------------------------------------------------------------------
! Module: PGRND -- find the smallest `round' number greater than x
! Status: IMPLEMENTED
!------------------------------------------------------------------------
real function PGRND (X, NSUB)
 use giza, only:giza_round
 implicit none
 real,    intent(in)  :: X
 integer, intent(out) :: NSUB

 PGRND = giza_round(X, NSUB)

end function PGRND

!------------------------------------------------------------------------
! Module: PGRNGE -- choose axis limits
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGRNGE (X1, X2, XLO, XHI)
 implicit none
 real, intent(in)  :: X1, X2
 real, intent(out) :: XLO, XHI
 real :: dx

 dx = X2-X1
 XLO = X1 - 0.1*dx
 XHI = X2 + 0.1*dx
 if (XLO.lt.0. .and. X1.ge.0.) XLO = 0.
 if (XHI.gt.0. .and. X2.le.0.) XHI = 0.

end subroutine PGRNGE

!------------------------------------------------------------------------
! Module: PGSAH -- set arrow-head style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSAH (FS, ANGLE, BARB)
 use giza, only:giza_set_arrow_style
 implicit none
 integer, intent(in) :: FS
 real,    intent(in) :: ANGLE, BARB

 call giza_set_arrow_style(FS, ANGLE, BARB)

end subroutine PGSAH

!------------------------------------------------------------------------
! Module: PGSAVE -- save PGPLOT attributes
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSAVE
 use giza, only:giza_save
 implicit none
 
 call giza_save()

end subroutine PGSAVE

!------------------------------------------------------------------------
! Module: PGUNSA -- restore PGPLOT attributes
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGUNSA
 use giza, only:giza_restore
 implicit none
 
 call giza_restore()

end subroutine PGUNSA

!------------------------------------------------------------------------
! Module: PGSCF -- set character font
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCF (FONT)
 use giza,       only:giza_set_font,giza_set_font_italic !,giza_set_font_bold
 use gizapgplot, only:pgfont
 implicit none
 integer, intent(in) :: FONT
 
 select case(FONT)
 case(4) 
    call giza_set_font('cursive')
 case(3) 
    call giza_set_font_italic('times')
 case(2)
    call giza_set_font('times')
 case default
    call giza_set_font('arial')
 end select
 !
 !--this is a setting for the PGPLOT frontend only
 !  (giza has much more general fonts than PGPLOT)
 !
 !  set pgfont so that query calls to PGQCF are successful 
 !
 pgfont = FONT

end subroutine PGSCF

!------------------------------------------------------------------------
! Module: PGSCH -- set character height
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCH (SIZE)
 use giza, only:giza_set_character_height
 implicit none
 real, intent(in) :: SIZE
 
 call giza_set_character_height(SIZE)

end subroutine PGSCH

!------------------------------------------------------------------------
! Module: PGSCI -- set color index
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCI (CI)
 use giza, only:giza_set_colour_index
 implicit none
 integer, intent(in) :: CI

 call giza_set_colour_index(CI)

end subroutine PGSCI

!------------------------------------------------------------------------
! Module: PGSCIR -- set color index range
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCIR(ICILO, ICIHI)
 use giza, only:giza_set_colour_index_range
 implicit none
 integer, intent(in) :: ICILO, ICIHI
 
 call giza_set_colour_index_range(ICILO,ICIHI)

end subroutine PGSCIR

!------------------------------------------------------------------------
! Module: PGSCLP -- enable or disable clipping at edge of viewport
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCLP(STATE)
 use giza, only:giza_set_clipping
 integer, intent(in) :: STATE

 call giza_set_clipping(STATE)

end subroutine PGSCLP

!------------------------------------------------------------------------
! Module: PGSCR -- set color representation
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCR (CI, CR, CG, CB)
 use giza, only:giza_set_colour_representation
 implicit none
 integer, intent(in) :: CI
 real,    intent(in) :: CR, CG, CB

 call giza_set_colour_representation(CI, CR, CG, CB)

end subroutine PGSCR

!------------------------------------------------------------------------
! Module: PGSCRL -- scroll window
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCRL (DX, DY)
 implicit none
 real, intent(in) :: DX, DY

end subroutine PGSCRL

!------------------------------------------------------------------------
! Module: PGSCRN -- set color representation by name
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSCRN(CI, NAME, IER)
 implicit none
 integer,       intent(in)  :: CI
 character*(*), intent(in)  :: NAME
 integer,       intent(out) :: IER

end subroutine PGSCRN

!------------------------------------------------------------------------
! Module: PGSFS -- set fill-area style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSFS (FS)
 use giza, only:giza_set_fill
 implicit none
 integer, intent(in) :: FS
 
 call giza_set_fill(FS)

end subroutine PGSFS

!------------------------------------------------------------------------
! Module: PGSHLS -- set color representation using HLS system
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSHLS (CI, CH, CL, CS)
 use giza, only:giza_set_colour_representation_hls
 implicit none
 integer, intent(in) :: CI
 real,    intent(in) :: CH, CL, CS
 
 call giza_set_colour_representation_hls(CI, CH, CL, CS)

end subroutine PGSHLS

!------------------------------------------------------------------------
! Module: PGSHS -- set hatching style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSHS (ANGLE, SEPN, PHASE)
 use giza, only:giza_set_hatching_style
 implicit none
 real, intent(in) :: ANGLE, SEPN, PHASE

 call giza_set_hatching_style(ANGLE,SEPN,PHASE)

end subroutine PGSHS

!------------------------------------------------------------------------
! Module: PGSITF -- set image transfer function
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSITF (ITF)
 implicit none
 integer, intent(in) :: ITF

end subroutine PGSITF

!------------------------------------------------------------------------
! Module: PGSLCT -- select an open graphics device
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSLCT(ID)
 use giza, only:giza_select_device
 integer, intent(in) :: ID
 integer :: ierr

 call giza_select_device(ID)

end subroutine PGSLCT

!------------------------------------------------------------------------
! Module: PGSLS -- set line style
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSLS (LS)
 use giza, only:giza_set_line_style,giza_ls_solid,giza_ls_short_dash, &
                giza_ls_long_dash, giza_ls_dot, giza_ls_dash_dot, &
                giza_ls_dash_dot_dot_dot
 implicit none
 integer, intent(in) :: LS
 
 select case(LS)
 case(1)
    call giza_set_line_style(giza_ls_solid) 
 case(2)
    call giza_set_line_style(giza_ls_short_dash)
 case(3)
    call giza_set_line_style(giza_ls_dash_dot)
 case(4)
    call giza_set_line_style(giza_ls_dot)
 case(5)
    call giza_set_line_style(giza_ls_dash_dot_dot_dot) 
 case(6) ! extension: we allow extra line styles beyond PGPLOT
    call giza_set_line_style(giza_ls_long_dash)
 case default
    call giza_set_line_style(giza_ls_solid)
 end select

end subroutine PGSLS

!------------------------------------------------------------------------
! Module: PGSLW -- set line width
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSLW (LW)
 use giza, only:giza_set_line_width
 implicit none
 integer, intent(in) :: LW
 
 call giza_set_line_width(0.5 + 0.5*LW)
 
end subroutine PGSLW

!------------------------------------------------------------------------
! Module: PGSTBG -- set text background color index
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSTBG (TBCI)
 use giza, only:giza_set_text_background
 implicit none
 integer, intent(in) :: TBCI
 
 call giza_set_text_background(TBCI)

end subroutine PGSTBG

!------------------------------------------------------------------------
! Module: PGSUBP -- subdivide view surface into panels
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSUBP (NXSUB, NYSUB)
 use giza, only:giza_subpanel
 implicit none
 integer, intent(in) :: NXSUB, NYSUB

 call giza_subpanel(NXSUB, NYSUB)

end subroutine PGSUBP

!------------------------------------------------------------------------
! Module: PGSVP -- set viewport (normalized device coordinates)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSVP (XLEFT, XRIGHT, YBOT, YTOP)
 use giza, only:giza_set_viewport
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call giza_set_viewport(XLEFT, XRIGHT, YBOT, YTOP)

end subroutine PGSVP

!------------------------------------------------------------------------
! Module: PGSWIN -- set window
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGSWIN (X1, X2, Y1, Y2)
 use giza, only:giza_set_window
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

 call giza_set_window(X1, X2, Y1, Y2)

end subroutine PGSWIN

!------------------------------------------------------------------------
! Module: PGTBOX -- draw frame and write (DD) HH MM SS.S labelling
! Status: PARTIALLY IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGTBOX (XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)
 use giza, only:giza_box_time
 implicit none
 real,      intent(in) :: XTICK, YTICK
 integer,   intent(in) :: NXSUB, NYSUB
 character, intent(in) :: XOPT*(*), YOPT*(*)

 call giza_box_time(XOPT, XTICK, NXSUB, YOPT, YTICK, NYSUB)

end subroutine PGTBOX

!------------------------------------------------------------------------
! Module: PGTEXT -- write text (horizontal, left-justified)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGTEXT (X, Y, TEXT)
 use giza, only:giza_text
 implicit none
 real,          intent(in) :: X, Y
 character*(*), intent(in) :: TEXT
 
 call giza_text(X, Y, TEXT)

end subroutine PGTEXT

!------------------------------------------------------------------------
! Module: PGTICK -- draw a single tick mark on an axis
! Status: NOT IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGTICK (X1, Y1, X2, Y2, V, TIKL, TIKR, DISP, ORIENT, STR)
 implicit none
 real, intent(in) :: X1, Y1, X2, Y2, V, TIKL, TIKR, DISP, ORIENT
 character*(*), intent(in) :: STR

end subroutine PGTICK

!------------------------------------------------------------------------
! Module: PGUPDT -- update display
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGUPDT
 use giza, only:giza_flush_buffer
 implicit none
 
 call giza_flush_buffer()

end subroutine PGUPDT

!------------------------------------------------------------------------
! Module: PGVECT -- vector map of a 2D data array, with blanking
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVECT (A, B, IDIM, JDIM, I1, I2, J1, J2, C, NC, TR, BLANK)
 use giza,       only:giza_vector
 use gizapgplot, only:convert_tr_to_affine
 implicit none
 integer, intent(in) :: IDIM, JDIM, I1, I2, J1, J2, NC
 real,    intent(in) :: A(IDIM,JDIM), B(IDIM, JDIM), TR(6), BLANK, C
 real                :: affine(6)

 call convert_tr_to_affine(tr,affine)
 call giza_vector(idim,jdim,a,b,i1-1,i2-1,j1-1,j2-1,c,nc,affine,blank)

end subroutine PGVECT

!------------------------------------------------------------------------
! Module: PGVSIZ -- set viewport (inches)
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVSIZ (XLEFT, XRIGHT, YBOT, YTOP)
 use giza, only:giza_set_viewport_inches
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call giza_set_viewport_inches(XLEFT, XRIGHT, YBOT, YTOP)

end subroutine PGVSIZ

!------------------------------------------------------------------------
! Module: PGVSTD -- set standard (default) viewport
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVSTD
 use giza, only:giza_set_viewport_default
 implicit none
 
 call giza_set_viewport_default()

end subroutine PGVSTD

!------------------------------------------------------------------------
! Module: PGWEDG -- annotate an image plot with a wedge
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGWEDG(SIDE, DISP, WIDTH, FG, BG, LABEL)
 use giza, only:giza_colour_bar
 implicit none
 character *(*), intent(in) :: SIDE,LABEL
 real,           intent(in) :: DISP, WIDTH, FG, BG

 call giza_colour_bar(SIDE, DISP, WIDTH, FG, BG, LABEL)

end subroutine PGWEDG

!------------------------------------------------------------------------
! Module: PGWNAD -- set window and adjust viewport to same aspect ratio
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGWNAD (X1, X2, Y1, Y2)
 use giza, only:giza_set_window_equal_scale
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

 call giza_set_window_equal_scale(X1, X2, Y1, Y2)

end subroutine PGWNAD

!------------------------------------------------------------------------
! Module: PGADVANCE -- non-standard alias for PGPAGE
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGADVANCE
 implicit none
 
 call PGPAGE

end subroutine PGADVANCE

!------------------------------------------------------------------------
! Module: PGBEGIN -- non-standard alias for PGBEG
! Status: IMPLEMENTED
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
! Status: IMPLEMENTED
!------------------------------------------------------------------------
integer function PGCURSE (X, Y, CH)
 implicit none
 real,        intent(inout) :: X, Y
 character*1, intent(inout) :: CH
 integer, external :: PGCURS

 PGCURSE = PGCURS(X, Y, CH)

end function PGCURSE

!------------------------------------------------------------------------
! Module: PGLABEL -- non-standard alias for PGLAB
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGLABEL (XLBL, YLBL, TOPLBL)
 implicit none
 character*(*), intent(in) :: XLBL, YLBL, TOPLBL

 call PGLAB(XLBL, YLBL, TOPLBL)

end subroutine PGLABEL

!------------------------------------------------------------------------
! Module: PGMTEXT -- non-standard alias for PGMTXT
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGMTEXT (SIDE, DISP, COORD, FJUST, TEXT)
 implicit none
 character*(*), intent(in) :: SIDE, TEXT
 real,          intent(in) :: DISP, COORD, FJUST

 call PGMTXT(SIDE, DISP, COORD, FJUST, TEXT)

end subroutine PGMTEXT

!------------------------------------------------------------------------
! Module: PGNCURSE -- non-standard alias for PGNCUR
! Status: IMPLEMENTED
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
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPAPER (WIDTH, ASPECT)
 implicit none
 real, intent(in) :: WIDTH, ASPECT

 call PGPAP(WIDTH, ASPECT)

end subroutine PGPAPER

!------------------------------------------------------------------------
! Module: PGPOINT -- non-standard alias for PGPT
! Status: IMPLEMENTED
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
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGPTEXT (X, Y, ANGLE, FJUST, TEXT)
 implicit none
 real, intent(in) :: X, Y, ANGLE, FJUST
 character*(*), intent(in) :: TEXT

 call PGPTXT(X, Y, ANGLE, FJUST, TEXT)

end subroutine PGPTEXT

!------------------------------------------------------------------------
! Module: PGVPORT -- non-standard alias for PGSVP
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVPORT (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call PGSVP(XLEFT, XRIGHT, YBOT, YTOP)

end subroutine PGVPORT

!------------------------------------------------------------------------
! Module: PGVSIZE -- non-standard alias for PGVSIZ
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVSIZE (XLEFT, XRIGHT, YBOT, YTOP)
 implicit none
 real, intent(in) :: XLEFT, XRIGHT, YBOT, YTOP

 call PGVSIZ(XLEFT, XRIGHT, YBOT, YTOP)
 
end subroutine PGVSIZE

!------------------------------------------------------------------------
! Module: PGVSTAND -- non-standard alias for PGVSTD
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGVSTAND
 implicit none

 call PGVSTD

end subroutine PGVSTAND

!------------------------------------------------------------------------
! Module: PGWINDOW -- non-standard alias for PGSWIN
! Status: IMPLEMENTED
!------------------------------------------------------------------------
subroutine PGWINDOW (X1, X2, Y1, Y2)
 implicit none
 real, intent(in) :: X1, X2, Y1, Y2

 call PGSWIN(X1, X2, Y1, Y2)

end subroutine PGWINDOW
