!! giza - a scientific plotting layer built on cairo
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
!  This file contains the Fortran interface to giza
!  (using the Fortran 2003 iso_c_binding module)
!---------------------------------------------------------
!!!NOTE: no interface to get font yet!

module giza
 use, intrinsic :: iso_c_binding, only:c_double,c_float,c_char,c_int
 implicit none
 public  :: &
      giza_arrow, &
      giza_set_arrow_style, &
      giza_get_arrow_style, &
      giza_annotate, &
      giza_band, &
      giza_box, &
      giza_begin_buffer, &
      giza_end_buffer, &
      giza_flush_buffer, &
      giza_circle, &
      giza_set_character_height, &
      giza_get_character_height, &
      giza_get_character_size, &
      giza_set_clipping, &
      giza_get_clipping, &
      giza_set_colour_index, &
      giza_get_colour_index, &
      giza_set_colour_index_range, &
      giza_get_colour_index_range, &
      giza_set_colour_palette, &
      giza_set_colour_representation, &
      giza_set_colour_representation_alpha, &
      giza_get_colour_representation, &
      giza_get_colour_representation_alpha, &
      giza_set_colour_representation_hls, &
      giza_set_colour_table, &
      giza_contour, &
      giza_get_current_point, &
      giza_rgb_from_table, &
      giza_print_device_list, &
      giza_open_device, &
      giza_open_device_size, &
      giza_flush_device, &
      giza_change_page, &
      giza_close_device, &
      giza_get_key_press, &
      giza_device_has_cursor, &
      giza_draw, &
      giza_draw_background, &
      giza_set_environment, &
      giza_error_bars, &
      giza_error_bars_x, &
      giza_error_bars_y, &
      giza_set_fill, &
      giza_get_fill, &
      giza_function_x, &
      giza_function_y, &
      giza_function_t, &
      giza_get_surface_size, &
      giza_set_hatching_style, &
      giza_get_hatching_style, &
      giza_label, &
      giza_line, &
      giza_set_line_width, &
      giza_get_line_width, &
      giza_set_line_cap, &
      giza_get_line_cap, &
      giza_set_line_style, &
      giza_get_line_style, &
      giza_mark_line, &
      giza_mark_line_ordered, &
      giza_mark_points, &
      giza_mark_points_ordered, &
      giza_move, &
      giza_set_paper_size, &
      giza_get_paper_size, &
      giza_points, &
      giza_single_point, &
      giza_polygon, &
      giza_print_id, &
      giza_start_prompting, &
      giza_stop_prompting, &
      giza_set_text_background, &
      giza_get_text_background, &
      giza_text, &
      giza_ptext, &
      giza_qtext, &
      giza_qtextlen, &
      giza_rectangle, &
      giza_render, &
      giza_render_gray, &
      giza_render_transparent, &
      giza_draw_pixels, &
      giza_restore, &
      giza_round, &
      giza_save, &
      giza_set_font, &
      giza_set_font_bold, &
      giza_set_font_italic, &
      giza_set_font_bold_italic, &
      giza_vector, &
      giza_set_viewport_default, &
      giza_set_viewport, &
      giza_get_viewport, &
      giza_set_viewport_inches, &
      giza_start_warnings, &
      giza_stop_warnings, &
      giza_set_window, &
      giza_set_window_equal_scale, &
      giza_get_window, &
      giza_left_click_f, &
      giza_format_number, &
      giza_query_device

#include "giza-shared.h"
  character(len=1),parameter :: giza_left_click_f = GIZA_LEFT_CLICK
  character(len=*), parameter, public :: giza_version_string = GIZA_VERSION_STRING
!
!--the following can be generated automatically using the get-fortran-params.pl script
!
  integer, parameter, public :: giza_ls_solid = GIZA_LS_SOLID
  integer, parameter, public :: giza_ls_short_dash = GIZA_LS_SHORT_DASH
  integer, parameter, public :: giza_ls_long_dash = GIZA_LS_LONG_DASH
  integer, parameter, public :: giza_ls_dot = GIZA_LS_DOT
  integer, parameter, public :: giza_ls_dash_dot = GIZA_LS_DASH_DOT
  integer, parameter, public :: giza_ls_dash_dot_dot_dot = GIZA_LS_DASH_DOT_DOT_DOT
  integer, parameter, public :: giza_number_format_auto = GIZA_NUMBER_FORMAT_AUTO
  integer, parameter, public :: giza_number_format_dec = GIZA_NUMBER_FORMAT_DEC
  integer, parameter, public :: giza_number_format_exp = GIZA_NUMBER_FORMAT_EXP
  integer, parameter, public :: giza_units_normalized = GIZA_UNITS_NORMALIZED
  integer, parameter, public :: giza_units_inches = GIZA_UNITS_INCHES
  integer, parameter, public :: giza_units_mm = GIZA_UNITS_MM
  integer, parameter, public :: giza_units_pixels = GIZA_UNITS_PIXELS
  integer, parameter, public :: giza_units_world = GIZA_UNITS_WORLD
  integer, parameter, public :: giza_units_device = GIZA_UNITS_DEVICE
  integer, parameter, public :: giza_band_none = GIZA_BAND_NONE
  integer, parameter, public :: giza_band_line = GIZA_BAND_LINE
  integer, parameter, public :: giza_band_rectangle = GIZA_BAND_RECTANGLE
  integer, parameter, public :: giza_band_horzlines = GIZA_BAND_HORZLINES
  integer, parameter, public :: giza_band_vertlines = GIZA_BAND_VERTLINES
  integer, parameter, public :: giza_band_horzline = GIZA_BAND_HORZLINE
  integer, parameter, public :: giza_band_vertline = GIZA_BAND_VERTLINE
  integer, parameter, public :: giza_band_crosshair = GIZA_BAND_CROSSHAIR
  integer, parameter, public :: giza_band_circle = GIZA_BAND_CIRCLE
  integer, parameter, public :: giza_background_colour = GIZA_BACKGROUND_COLOUR
  integer, parameter, public :: giza_background_color = GIZA_BACKGROUND_COLOR
  integer, parameter, public :: giza_foreground_colour = GIZA_FOREGROUND_COLOUR
  integer, parameter, public :: giza_foreground_color = GIZA_FOREGROUND_COLOR
  integer, parameter, public :: giza_colour_index_min = GIZA_COLOUR_INDEX_MIN
  integer, parameter, public :: giza_colour_index_max = GIZA_COLOUR_INDEX_MAX
  integer, parameter, public :: giza_colour_palette_default = GIZA_COLOUR_PALETTE_DEFAULT
  integer, parameter, public :: giza_color_palette_default = GIZA_COLOR_PALETTE_DEFAULT
  integer, parameter, public :: giza_colour_palette_pgplot = GIZA_COLOUR_PALETTE_PGPLOT
  integer, parameter, public :: giza_color_palette_pgplot = GIZA_COLOR_PALETTE_PGPLOT
  integer, parameter, public :: giza_fill_solid = GIZA_FILL_SOLID
  integer, parameter, public :: giza_fill_hollow = GIZA_FILL_HOLLOW
  integer, parameter, public :: giza_fill_hatch = GIZA_FILL_HATCH
  integer, parameter, public :: giza_fill_crosshatch = GIZA_FILL_CROSSHATCH
  integer, parameter, public :: giza_max_fill_styles = GIZA_MAX_FILL_STYLES

private

!---------------------------------------------------------
!  The following are pure interfaces to C
!  We make generic interfaces so that the Fortran can
!  be called with either single or double precision
!---------------------------------------------------------

 interface giza_arrow
    subroutine giza_arrow(x1,y1,x2,y2) bind(C)
      import
      real(kind=c_double),intent(in),value :: x1,y1,x2,y2
    end subroutine giza_arrow
    
    subroutine giza_arrow_float(x1,y1,x2,y2) bind(C)
      import
      real(kind=c_float),intent(in),value :: x1,y1,x2,y2
    end subroutine giza_arrow_float
 end interface

 interface giza_set_arrow_style
    subroutine giza_set_arrow_style(fillStyle,angle,cutback) bind(C)
      import
      integer(kind=c_int),intent(in),value :: fillStyle
      real(kind=c_double),intent(in),value :: angle,cutback
    end subroutine giza_set_arrow_style

    subroutine giza_set_arrow_style_float(fillStyle,angle,cutback) bind(C)
      import
      integer(kind=c_int),intent(in),value :: fillStyle
      real(kind=c_float) ,intent(in),value :: angle,cutback
    end subroutine giza_set_arrow_style_float
 end interface

 interface giza_get_arrow_style
    subroutine giza_get_arrow_style(fillStyle,angle,cutback) bind(C)
      import
      integer(kind=c_int),intent(out) :: fillStyle
      real(kind=c_double),intent(out) :: angle,cutback
    end subroutine giza_get_arrow_style

    subroutine giza_get_arrow_style_float(fillStyle,angle,cutback) bind(C)
      import
      integer(kind=c_int),intent(out) :: fillStyle
      real(kind=c_float) ,intent(out) :: angle,cutback
    end subroutine giza_get_arrow_style_float
 end interface

 interface giza_band
    function giza_band(mode,moveCurs,xanc,yanc,x,y,ch) bind(C)
      import
      integer(kind=c_int)                  :: giza_band
      integer(kind=c_int),intent(in),value :: mode,moveCurs
      real(kind=c_double),intent(in),value :: xanc,yanc
      real(kind=c_double),intent(inout)    :: x,y
      character(kind=c_char),intent(out)   :: ch
    end function giza_band

    function giza_band_float(mode,moveCurs,xanc,yanc,x,y,ch) bind(C)
      import
      integer(kind=c_int)                  :: giza_band_float
      integer(kind=c_int),intent(in),value :: mode,moveCurs
      real(kind=c_float),intent(in),value  :: xanc,yanc
      real(kind=c_float),intent(inout)     :: x,y
      character(kind=c_char),intent(out)   :: ch
    end function giza_band_float
 end interface

 interface giza_annotate
    module procedure giza_intern_annotate_f2c
 end interface
 
 interface giza_annotate_c
    subroutine giza_annotate_c (side,displacement,coord,justification,text) bind(C,name="giza_annotate")
      import
      character(kind=c_char),dimension(*),intent(in) :: side,text
      real(kind=c_double),intent(in),value    ::displacement,coord,justification
    end subroutine giza_annotate_c
    
    subroutine giza_annotate_float_c (side,displacement,coord,justification,text) bind(C,name="giza_annotate_float")
      import
      character(kind=c_char),dimension(*),intent(in) :: side,text
      real(kind=c_float),intent(in),value     ::displacement,coord,justification
    end subroutine giza_annotate_float_c
 end interface
 
 interface giza_box
    module procedure giza_intern_box_f2c
 end interface
 
 interface giza_box_c
    subroutine giza_box_float_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box_float")
      import
      implicit none
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_float), value, intent(in)    :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_float_c
    
    subroutine giza_box_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box")
      import
      implicit none
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_double), value, intent(in)   :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_c
 end interface
 
 interface giza_begin_buffer
    subroutine giza_begin_buffer() bind(C)
    end subroutine giza_begin_buffer
 end interface

 interface giza_end_buffer
    subroutine giza_end_buffer() bind(C)
    end subroutine giza_end_buffer
 end interface

 interface giza_flush_buffer
    subroutine giza_flush_buffer() bind(C)
    end subroutine giza_flush_buffer
 end interface

 interface giza_circle
    subroutine giza_circle(x,y,r) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: x,y,r
    end subroutine giza_circle

    subroutine giza_circle_float(x,y,r) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: x,y,r
    end subroutine giza_circle_float
 end interface

 interface giza_set_character_height 
    subroutine giza_set_character_height(ch) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: ch
    end subroutine giza_set_character_height
    
    subroutine giza_set_character_height_float(ch) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: ch
    end subroutine giza_set_character_height_float
 end interface
 
 interface giza_get_character_height
    subroutine giza_get_character_height(ch) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: ch
    end subroutine giza_get_character_height
    
    subroutine giza_get_character_height_float(ch) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: ch
    end subroutine giza_get_character_height_float
 end interface

 interface giza_get_character_size
    subroutine giza_get_character_size(units,xch,ych) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_double),intent(out)      :: xch,ych
    end subroutine giza_get_character_size
    
    subroutine giza_get_character_size_float(units,xch,ych) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_float),intent(out)       :: xch,ych
    end subroutine giza_get_character_size_float
 end interface

 interface giza_set_clipping
    subroutine giza_set_clipping(clip) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: clip
    end subroutine giza_set_clipping
 end interface

 interface giza_get_clipping
    subroutine giza_get_clipping(clip) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(out) :: clip
    end subroutine giza_get_clipping
 end interface

 interface giza_set_colour_index
    subroutine giza_set_colour_index(ci) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: ci
    end subroutine giza_set_colour_index
 end interface

 interface giza_get_colour_index
    subroutine giza_get_colour_index(ci) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(out) :: ci
    end subroutine giza_get_colour_index
 end interface

 interface giza_set_colour_palette
    subroutine giza_set_colour_palette(palette) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: palette
    end subroutine giza_set_colour_palette
 end interface

 interface giza_set_colour_index_range
    subroutine giza_set_colour_index_range(cimin,cimax) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: cimin,cimax
    end subroutine giza_set_colour_index_range
 end interface

 interface giza_get_colour_index_range
    subroutine giza_get_colour_index_range(cimin,cimax) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(out) :: cimin,cimax
    end subroutine giza_get_colour_index_range
 end interface

 interface giza_set_colour_representation
    subroutine giza_set_colour_representation_float(ci,red,green,blue) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in)    :: ci
      real(kind=c_float), value, intent(in)     :: red,green,blue
    end subroutine giza_set_colour_representation_float
    
    subroutine giza_set_colour_representation(ci,red,green,blue) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: ci
      real(kind=c_double), value, intent(in) :: red,green,blue
    end subroutine giza_set_colour_representation
 end interface

 interface giza_set_colour_representation_alpha
    subroutine giza_set_colour_representation_alpha_float(ci,red,green,blue,alpha) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in)    :: ci
      real(kind=c_float), value, intent(in)     :: red,green,blue,alpha
    end subroutine giza_set_colour_representation_alpha_float
    
    subroutine giza_set_colour_representation_alpha(ci,red,green,blue,alpha) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: ci
      real(kind=c_double), value, intent(in) :: red,green,blue,alpha
    end subroutine giza_set_colour_representation_alpha
 end interface

 interface giza_get_colour_representation
    subroutine giza_get_colour_representation(ci,red,green,blue) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_double),intent(out) :: red,green,blue
    end subroutine giza_get_colour_representation
   
    subroutine giza_get_colour_representation_float(ci,red,green,blue) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_float),intent(out) :: red,green,blue
    end subroutine giza_get_colour_representation_float
 end interface

 interface giza_get_colour_representation_alpha
    subroutine giza_get_colour_representation_alpha(ci,red,green,blue,alpha) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_double),intent(out) :: red,green,blue,alpha
    end subroutine giza_get_colour_representation_alpha
   
    subroutine giza_get_colour_representation_alpha_float(ci,red,green,blue,alpha) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_float),intent(out) :: red,green,blue,alpha
    end subroutine giza_get_colour_representation_alpha_float
 end interface

 interface giza_set_colour_representation_hls
    subroutine giza_set_colour_representation_hls(ci,hue,light,sat) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: ci
      real(kind=c_double),intent(in),value :: hue,light,sat
    end subroutine giza_set_colour_representation_hls

    subroutine giza_set_colour_representation_hls_float(ci,hue,light,sat) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: ci
      real(kind=c_float),intent(in),value  :: hue,light,sat
    end subroutine giza_set_colour_representation_hls_float
 end interface

 interface giza_set_colour_table
    subroutine giza_set_colour_table(controlPoints,red,green,blue,n) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value        :: n
      real(kind=c_double),intent(in),dimension(n) :: controlPoints,red,green,blue
    end subroutine giza_set_colour_table
    
    subroutine giza_set_colour_table_float(controlPoints,red,green,blue,n) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value       :: n
      real(kind=c_float),intent(in),dimension(n) :: controlPoints,red,green,blue
    end subroutine giza_set_colour_table_float
 end interface

 interface giza_contour
    subroutine giza_contour(sizex,sizey,data,i1,i2,j1,j2,ncont,cont,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,ncont
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in) :: cont(ncont)
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_contour
    
    subroutine giza_contour_float(sizex,sizey,data,i1,i2,j1,j2,ncont,cont,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,ncont
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in) :: cont(ncont)
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_contour_float
 end interface

 interface giza_get_current_point
    subroutine giza_get_current_point(xpt,ypt) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: xpt,ypt
    end subroutine giza_get_current_point
    
    subroutine giza_get_current_point_float(xpt,ypt) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: xpt,ypt
    end subroutine giza_get_current_point_float
 end interface

 interface giza_rgb_from_table
    subroutine giza_rgb_from_table(pos,r,g,b) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: pos,r,g,b
    end subroutine giza_rgb_from_table

    subroutine giza_rgb_from_table_float(pos,r,g,b) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: pos,r,g,b
    end subroutine giza_rgb_from_table_float
 end interface

 interface giza_print_device_list
    subroutine giza_print_device_list() bind(C)
      implicit none
    end subroutine  giza_print_device_list
 end interface

 interface giza_open_device
    module procedure giza_intern_open_device
 end interface

 interface giza_open_device_c
    integer(kind=c_int) function giza_open_device_c (dev,prefix) bind(C,name="giza_open_device")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: dev,prefix
    end function  giza_open_device_c
 end interface

!--open device: requires string conversion
 interface giza_open_device_size
    module procedure giza_intern_open_device_size
 end interface

 interface giza_open_device_size_c
    integer(kind=c_int) function giza_open_device_size_c (dev,prefix,width,height,units) &
                        bind(C,name="giza_open_device_size")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: dev,prefix
      real(kind=c_double),intent(in),value    :: height,width
      integer(kind=c_int),intent(in),value    :: units
    end function giza_open_device_size_c

    integer(kind=c_int) function giza_open_device_size_float_c(dev,prefix,width,height,units) &
                        bind(C,name="giza_open_device_size_float")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: dev,prefix
      real(kind=c_float),intent(in),value     :: height,width
      integer(kind=c_int),intent(in),value    :: units
    end function giza_open_device_size_float_c
 end interface
 
! device_has_cursor is logical in Fortran but int in c
! (cannot do a generic interface for int/logical), can call c version directly with _int version
 interface giza_device_has_cursor
    module procedure giza_intern_device_has_cursor
 end interface

 interface giza_device_has_cursor_int
    function giza_device_has_cursor_c() bind(C,name="giza_device_has_cursor")
     import
     implicit none
     integer(kind=c_int) :: giza_device_has_cursor_c
    end function giza_device_has_cursor_c
 end interface

 interface giza_flush_device
    subroutine giza_flush_device() bind(C)
    end subroutine giza_flush_device
 end interface

 interface giza_change_page
    subroutine giza_change_page() bind(C)
    end subroutine giza_change_page
 end interface

 interface giza_close_device
    subroutine giza_close_device() bind(C)
    end subroutine giza_close_device
 end interface

 interface giza_get_key_press
    function giza_get_key_press(x,y,ch) bind(C)
      import
      implicit none
      integer(kind=c_int)                :: giza_get_key_press
      real(kind=c_double),intent(out)    :: x,y
      character(kind=c_char),intent(out) :: ch
    end function giza_get_key_press

    function giza_get_key_press_float(x,y,ch) bind(C)
      import
      implicit none
      integer(kind=c_int)                :: giza_get_key_press_float
      real(kind=c_float),intent(out)     :: x,y
      character(kind=c_char),intent(out) :: ch
    end function giza_get_key_press_float
 end interface

 interface giza_draw
    subroutine giza_draw(x,y) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: x,y
    end subroutine giza_draw

    subroutine giza_draw_float(x,y) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: x,y
    end subroutine giza_draw_float
 end interface

 interface giza_draw_background
    subroutine giza_draw_background() bind(C)
    end subroutine
 end interface

 interface giza_set_environment
    subroutine giza_set_environment(xmin,xmax,ymin,ymax,just,axis) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: xmin,xmax,ymin,ymax
      integer(kind=c_int),intent(in),value :: just,axis
    end subroutine giza_set_environment

    subroutine giza_set_environment_float(xmin,xmax,ymin,ymax,just,axis) bind(C)
      import
      implicit none
      real(c_float),intent(in),value       :: xmin,xmax,ymin,ymax
      integer(kind=c_int),intent(in),value :: just,axis
    end subroutine giza_set_environment_float
 end interface

 interface giza_error_bars
    subroutine giza_error_bars(dir,n,xpts,ypts,error,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: dir,n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts,error
    end subroutine giza_error_bars

    subroutine giza_error_bars_float(dir,n,xpts,ypts,error,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: dir,n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts,error
    end subroutine giza_error_bars_float
 end interface

 interface giza_error_bars_x
    subroutine giza_error_bars_hori(n,xpts,ypts1,ypts2,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_hori

    subroutine giza_error_bars_hori_float(n,xpts,ypts1,ypts2,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_hori_float
 end interface

 interface giza_error_bars_y
    subroutine giza_error_bars_vert(n,xpts,ypts1,ypts2,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_vert

    subroutine giza_error_bars_vert_float(n,xpts,ypts1,ypts2,term) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_vert_float
 end interface

 interface giza_set_fill
    subroutine giza_set_fill(fs) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: fs
    end subroutine giza_set_fill
 end interface
 
 interface giza_get_fill
    subroutine giza_get_fill(fs) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(out) :: fs
    end subroutine giza_get_fill
 end interface

 interface giza_function_x
    subroutine giza_function_x(func,n,xmin,xmax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: xmin,xmax
      real(kind=c_double),external :: func
    end subroutine giza_function_x

    subroutine giza_function_x_float(func,n,xmin,xmax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: xmin,xmax
      real(kind=c_float),external :: func
    end subroutine giza_function_x_float
 end interface
 
 interface giza_function_y
    subroutine giza_function_y(func,n,ymin,ymax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: ymin,ymax
      real(kind=c_double),external :: func
    end subroutine giza_function_y

    subroutine giza_function_y_float(func,n,ymin,ymax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: ymin,ymax
      real(kind=c_float),external :: func
    end subroutine giza_function_y_float
 end interface

 interface giza_function_t
    subroutine giza_function_t(funcx,funcy,n,tmin,tmax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: tmin,tmax
      real(kind=c_double),external :: funcx,funcy
    end subroutine giza_function_t

    subroutine giza_function_t_float(funcx,funcy,n,tmin,tmax,flag) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: tmin,tmax
      real(kind=c_float),external :: funcx,funcy
    end subroutine giza_function_t_float
 end interface

 interface giza_get_surface_size
    subroutine giza_get_surface_size_float(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_surface_size_float

    subroutine giza_get_surface_size(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_surface_size
 end interface
 
 interface giza_set_hatching_style
    subroutine giza_set_hatching_style(angle,spacing,phase) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: angle,spacing,phase
    end subroutine giza_set_hatching_style

    subroutine giza_set_hatching_style_float(angle,spacing,phase) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: angle,spacing,phase
    end subroutine giza_set_hatching_style_float
 end interface giza_set_hatching_style

 interface giza_get_hatching_style
    subroutine giza_get_hatching_style(angle,spacing,phase) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: angle,spacing,phase
    end subroutine giza_get_hatching_style

    subroutine giza_get_hatching_style_float(angle,spacing,phase) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: angle,spacing,phase
    end subroutine giza_get_hatching_style_float
 end interface giza_get_hatching_style

 interface giza_label
    module procedure giza_intern_label_f2c
 end interface
 
 interface giza_label_c
    subroutine giza_label_c (labelx,labely,title) bind(C,name="giza_label")
      import
      character(kind=c_char),dimension(*),intent(in) :: labelx,labely,title
    end subroutine giza_label_c
 end interface giza_label_c

 interface giza_line
    subroutine giza_line_float(n,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),value,intent(in)       :: n
      real(kind=c_float),intent(in),dimension(n) :: xpts,ypts
    end subroutine giza_line_float

    subroutine giza_plot_line(n,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),value,intent(in)        :: n
      real(kind=c_double),intent(in),dimension(n) :: xpts,ypts
    end subroutine giza_plot_line
 end interface
 
 interface giza_set_line_width
    subroutine giza_set_line_width(lw) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in), value :: lw
    end subroutine giza_set_line_width
    
    subroutine giza_set_line_width_float(lw) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in), value :: lw
    end subroutine giza_set_line_width_float
    
    module procedure giza_intern_set_line_width_int
 end interface
 
 interface giza_get_line_width
    subroutine giza_get_line_width(lw) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: lw
    end subroutine giza_get_line_width
    
    subroutine giza_get_line_width_float(lw) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: lw
    end subroutine giza_get_line_width_float
    
    module procedure giza_intern_get_line_width_int
 end interface

 interface giza_set_line_cap
    subroutine giza_set_line_cap(lc) bind(C)
      import
      implicit none
      integer(c_int),value,intent(in) :: lc
    end subroutine giza_set_line_cap
 end interface

 interface giza_get_line_cap
    subroutine giza_get_line_cap(lc) bind(C)
      import
      implicit none
      integer(c_int),intent(out) :: lc
    end subroutine giza_get_line_cap
 end interface

 interface giza_set_line_style
    subroutine giza_set_line_style(ls) bind(C)
      import
      implicit none
      integer(c_int),value,intent(in) :: ls
    end subroutine giza_set_line_style
 end interface

 interface giza_get_line_style
    subroutine giza_get_line_style(ls) bind(C)
      import
      implicit none
      integer(c_int),intent(out) :: ls
    end subroutine giza_get_line_style
 end interface
 
 interface giza_mark_line
    subroutine giza_mark_line(maxpts,npts,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line
    
    subroutine giza_mark_line_float(maxpts,npts,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_float
 end interface

 interface giza_mark_line_ordered
    subroutine giza_mark_line_ordered(maxpts,npts,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_ordered
    
    subroutine giza_mark_line_ordered_float(maxpts,npts,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_ordered_float
 end interface

 interface giza_mark_points
    subroutine giza_mark_points(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points
    
    subroutine giza_mark_points_float(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points_float
 end interface

 interface giza_mark_points_ordered
    subroutine giza_mark_points_ordered(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points_ordered
    
    subroutine giza_mark_points_ordered_float(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points_ordered_float
 end interface
 
 interface giza_move
    subroutine giza_move(x,y) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: x,y
    end subroutine giza_move

    subroutine giza_move_float(x,y) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: x,y
    end subroutine giza_move_float
 end interface

 interface giza_get_paper_size
    subroutine giza_get_paper_size(units,width,height) bind(C)
      import
      implicit none
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_double),intent(out) :: width,height
    end subroutine giza_get_paper_size

    subroutine giza_get_paper_size_float(units,width,height) bind(C)
      import
      implicit none
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_float),intent(out) :: width,height
    end subroutine giza_get_paper_size_float
 end interface

 interface giza_set_paper_size
    subroutine giza_set_paper_size(units,width,aspect) bind(C)
      import
      implicit none
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_double), intent(in),value :: width,aspect
    end subroutine giza_set_paper_size

    subroutine giza_set_paper_size_float(units,width,aspect) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_float), intent(in),value :: width,aspect
    end subroutine giza_set_paper_size_float
 end interface

 interface giza_points
    subroutine giza_points(n,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: n, symbol
      real(kind=c_double), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_points

    subroutine giza_points_float(n,xpts,ypts,symbol) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: n, symbol
      real(kind=c_float), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_points_float
 end interface

 interface giza_single_point
    subroutine giza_single_point(x,y,symbol) bind(C)
      import
      implicit none
      real(kind=c_double), value, intent(in) :: x,y
      integer(kind=c_int), value, intent(in) :: symbol
    end subroutine giza_single_point

    subroutine giza_single_point_float(x,y,symbol) bind(C)
      import
      implicit none
      real(kind=c_float),  value, intent(in) :: x,y
      integer(kind=c_int), value, intent(in) :: symbol
    end subroutine giza_single_point_float
 end interface

 interface giza_polygon
    subroutine giza_polygon(n,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: n
      real(kind=c_double), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_polygon

    subroutine giza_polygon_float(n,xpts,ypts) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: n
      real(kind=c_float), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_polygon_float
 end interface

 interface giza_print_id
    subroutine giza_print_id() bind(C)
    end subroutine giza_print_id
 end interface

 interface giza_start_prompting
    subroutine giza_start_prompting() bind(C)
    end subroutine giza_start_prompting
 end interface

 interface giza_stop_prompting
    subroutine giza_stop_prompting() bind(C)
    end subroutine giza_stop_prompting
 end interface

 interface giza_text
    module procedure giza_intern_text_f2c    
    module procedure giza_intern_text_float_f2c
 end interface

 interface giza_set_text_background
    subroutine giza_set_text_background(ci) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in) :: ci
    end subroutine giza_set_text_background
 end interface

 interface giza_get_text_background
    subroutine giza_get_text_background(ci) bind(C)
      import
      implicit none
      integer(kind=c_int), intent(out) :: ci
    end subroutine giza_get_text_background
 end interface

 interface giza_text_c
    subroutine giza_text_c(x,y,text) bind(C,name="giza_text")
      import
      implicit none
      real(kind=c_double),value,intent(in)    :: x,y
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_text_c

    subroutine giza_text_float_c(x,y,text) bind(C,name="giza_text_float")
      import
      implicit none
      real(kind=c_float),value,intent(in)     :: x,y
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_text_float_c
 end interface

 interface giza_ptext
    module procedure giza_intern_ptext_f2c    
    module procedure giza_intern_ptext_float_f2c
 end interface

 interface giza_ptext_c
    subroutine giza_ptext_c(x,y,angle,just,text) bind(C,name="giza_ptext")
      import
      implicit none
      real(kind=c_double),value,intent(in)    :: x,y,angle,just
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_ptext_c

    subroutine giza_ptext_float_c(x,y,angle,just,text) bind(C,name="giza_ptext_float")
      import
      implicit none
      real(kind=c_float),value,intent(in)     :: x,y,angle,just
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_ptext_float_c
 end interface

 interface giza_qtext
    module procedure giza_intern_qtext_f2c   
    module procedure giza_intern_qtext_float_f2c
 end interface

 interface giza_qtext_c
    subroutine giza_qtext_c(x,y,angle,just,text,xbox,ybox) bind(C,name="giza_qtext")
      import
      implicit none
      real(kind=c_double),value,intent(in)    :: x,y,angle,just
      real(kind=c_double),intent(out)         :: xbox(4),ybox(4)
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_qtext_c

    subroutine giza_qtext_float_c(x,y,angle,just,text,xbox,ybox) bind(C,name="giza_qtext_float")
      import
      implicit none
      real(kind=c_float),value,intent(in)     :: x,y,angle,just
      real(kind=c_float),intent(out)          :: xbox(4),ybox(4)
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_qtext_float_c
 end interface

 interface giza_qtextlen
    module procedure giza_intern_qtextlen_f2c   
    module procedure giza_intern_qtextlen_float_f2c
 end interface

 interface giza_qtextlen_c
    subroutine giza_qtextlen_c(units,text,xlen,ylen) bind(C,name="giza_qtextlen")
      import
      implicit none
      integer(kind=c_int),value,intent(in)    :: units
      character(kind=c_char),dimension(*),intent(in) :: text
      real(kind=c_double),intent(out)         :: xlen,ylen
    end subroutine giza_qtextlen_c

    subroutine giza_qtextlen_float_c(units,text,xlen,ylen) bind(C,name="giza_qtextlen_float")
      import
      implicit none
      integer(kind=c_int),value,intent(in)    :: units
      character(kind=c_char),dimension(*),intent(in) :: text
      real(kind=c_float),intent(out)          :: xlen,ylen
    end subroutine giza_qtextlen_float_c
 end interface

 interface giza_render
    subroutine giza_render(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render
    
    subroutine giza_render_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_float

 end interface

 interface giza_render_transparent
    subroutine giza_render_transparent(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render_transparent
    
    subroutine giza_render_transparent_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_transparent_float
 end interface

 interface giza_render_gray
    subroutine giza_render_gray(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render_gray
    
    subroutine giza_render_gray_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_gray_float
 end interface

 interface giza_draw_pixels
    subroutine giza_draw_pixels(sizex,sizey,idata,i1,i2,j1,j2,xmin,xmax,ymin,ymax) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      integer(kind=c_int),intent(in) :: idata(sizex,sizey)
      real(kind=c_double),intent(in),value :: xmin,xmax,ymin,ymax
    end subroutine giza_draw_pixels
    
    subroutine giza_draw_pixels_float(sizex,sizey,idata,i1,i2,j1,j2,xmin,xmax,ymin,ymax) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2
      integer(kind=c_int),intent(in) :: idata(sizex,sizey)
      real(kind=c_float),intent(in),value :: xmin,xmax,ymin,ymax
    end subroutine giza_draw_pixels_float
 end interface

 interface giza_rectangle
    subroutine giza_rectangle(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_double),intent(in),value :: x1,x2,y1,y2
    end subroutine giza_rectangle

    subroutine giza_rectangle_float(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_float),intent(in),value :: x1,x2,y1,y2
    end subroutine giza_rectangle_float
 end interface giza_rectangle

 interface giza_restore
    subroutine giza_restore() bind(C)
    end subroutine giza_restore
 end interface

 interface giza_round
    function giza_round_db(x,nsub) bind(C,name="giza_round")
      import
      implicit none
      real(kind=c_double), intent(in) :: x
      integer(kind=c_int), intent(out) :: nsub
      real(kind=c_double) :: giza_round_db
    end function giza_round_db

    function giza_round_float(x,nsub) bind(C,name="giza_round_float")
      import
      implicit none
      real(kind=c_float), intent(in) :: x
      integer(kind=c_int), intent(out) :: nsub
      real(kind=c_float) :: giza_round_float
    end function giza_round_float
 end interface giza_round
 
 interface giza_save
    subroutine giza_save() bind(C)
    end subroutine giza_save
 end interface

 interface giza_set_font
    module procedure giza_intern_set_font_f2c
 end interface

 interface giza_set_font_bold
    module procedure giza_intern_set_font_bold_f2c
 end interface

 interface giza_set_font_italic
    module procedure giza_intern_set_font_italic_f2c
 end interface

 interface giza_set_font_bold_italic
    module procedure giza_intern_set_font_bold_italic_f2c
 end interface

 interface giza_set_font_c
    subroutine giza_set_font_c(font) bind(C,name="giza_set_font")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_c
 end interface
 interface giza_set_font_bold_c
    subroutine giza_set_font_bold_c(font) bind(C,name="giza_set_font_bold")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_bold_c
 end interface
 interface giza_set_font_italic_c
    subroutine giza_set_font_italic_c(font) bind(C,name="giza_set_font_italic")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_italic_c
 end interface
 interface giza_set_font_bold_italic_c
    subroutine giza_set_font_bold_italic_c(font) bind(C,name="giza_set_font_bold_italic")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_bold_italic_c
 end interface

 interface giza_vector
    subroutine giza_vector(sizex,sizey,horizontal,vertical,i1,i2,j1,j2,scale,position,affine,blank) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,position
      real(kind=c_double),intent(in) :: horizontal(sizex,sizey),vertical(sizex,sizey)
      real(kind=c_double),intent(in),value :: scale,blank
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_vector
    
    subroutine giza_vector_float(sizex,sizey,horizontal,vertical,i1,i2,j1,j2,scale,position,affine,blank) bind(C)
      import
      implicit none
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,position
      real(kind=c_float),intent(in) :: horizontal(sizex,sizey),vertical(sizex,sizey)
      real(kind=c_float),intent(in),value :: scale,blank
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_vector_float
 end interface


 interface giza_set_viewport
    subroutine giza_set_viewport_float(xleft,xright,ybot,ytop) bind(C)
      import
      implicit none
      real(kind=c_float), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_float

    subroutine giza_set_viewport(xleft,xright,ybot,ytop) bind(C)
      import
      implicit none
      real(kind=c_double), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport
 end interface

 interface giza_set_viewport_default
    subroutine giza_set_viewport_default() bind(C)
      implicit none
    end subroutine giza_set_viewport_default
 end interface

 interface giza_get_viewport
    subroutine giza_get_viewport(units,x1,x2,y1,y2) bind(C)
      import
      implicit none
      integer(kind=c_int), value, intent(in)  :: units
      real(kind=c_double),        intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_viewport
    
    subroutine giza_get_viewport_float(units,x1,x2,y1,y2) bind(C)
      import
      implicit none
      integer(kind=c_int),value, intent(in)  :: units
      real(kind=c_float),        intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_viewport_float
 end interface

 interface giza_set_viewport_inches
    subroutine giza_set_viewport_inches_float(xleft,xright,ybot,ytop) bind(C)
      import
      implicit none
      real(kind=c_float), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_inches_float

    subroutine giza_set_viewport_inches(xleft,xright,ybot,ytop) bind(C)
      import
      implicit none
      real(kind=c_double), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_inches
 end interface

 interface giza_start_warnings
    subroutine giza_start_warnings() bind(C)
    end subroutine giza_start_warnings
 end interface

 interface giza_stop_warnings
    subroutine giza_stop_warnings() bind(C)
    end subroutine giza_stop_warnings
 end interface

 interface giza_set_window
    subroutine giza_set_window_float(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_float), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_float

    subroutine giza_set_window(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_double), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window
 end interface

 interface giza_set_window_equal_scale
    subroutine giza_set_window_equal_scale(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_double), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_equal_scale

    subroutine giza_set_window_equal_scale_float(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_float), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_equal_scale_float
 end interface

 interface giza_get_window
    subroutine giza_get_window(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_double),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_window

    subroutine giza_get_window_float(x1,x2,y1,y2) bind(C)
      import
      implicit none
      real(kind=c_float),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_window_float
 end interface

 interface giza_format_number
    module procedure giza_format_number_f2c
 end interface

 !private :: giza_format_number_c
 interface giza_format_number_c
    subroutine giza_format_number_c(mantissa,power,iform,string) bind(C,name="giza_format_number")
      import
      implicit none
      integer(kind=c_int),value,intent(in)    :: mantissa,power,iform
      character(kind=c_char),dimension(*),intent(out) :: string
    end subroutine giza_format_number_c
 end interface

 interface giza_query_device
    module procedure giza_query_device_f2c
 end interface

 interface giza_query_device_c
    integer(kind=c_int) function giza_query_device_c(qtype,string) bind(C,name="giza_query_device")
      import
      implicit none
      character(kind=c_char),dimension(*),intent(in)  :: qtype
      character(kind=c_char),dimension(*),intent(out) :: string
    end function giza_query_device_c
 end interface
!------------------ end of interfaces -----------------------

contains

!-------------------------------------------------------
! these are subroutines whose arguments need conversion
! before passing to c (mainly strings)       
!-------------------------------------------------------

  subroutine giza_intern_annotate_f2c(side,displacement,coord,justification,text)
    implicit none
    character(len=*),intent(in) :: side,text
    real,intent(in)             :: displacement,coord,justification

    call giza_annotate_c(cstring(side),displacement,coord,justification,cstring(text))
  end subroutine giza_intern_annotate_f2c

  subroutine giza_intern_box_f2c(xopt,xtick,nxsub,yopt,ytick,nysub)
    implicit none
    character(len=*),intent(in) :: xopt,yopt
    real,intent(in)             :: xtick,ytick
    integer,intent(in)          :: nxsub,nysub

    call giza_box_c(cstring(xopt),xtick,nxsub,cstring(yopt),ytick,nysub)
  end subroutine giza_intern_box_f2c

  integer function  giza_intern_open_device(dev,prefix)
    implicit none
    character(len=*),intent(in) :: dev,prefix
    
    giza_intern_open_device = giza_open_device_c(cstring(dev),cstring(prefix))
  end function giza_intern_open_device

  integer function giza_intern_open_device_size(dev,prefix,width,height,units)
    implicit none
    character(len=*),intent(in) :: dev,prefix
    real,intent(in)             :: width,height
    integer, intent(in)         :: units
    
    giza_intern_open_device_size = giza_open_device_size_c(cstring(dev),cstring(prefix),width,height,units)
  
  end function giza_intern_open_device_size

  ! So cursor functionality can be queried as logical 
  logical function giza_intern_device_has_cursor()
    implicit none

    giza_intern_device_has_cursor = (giza_device_has_cursor_c().eq.1)

  end function giza_intern_device_has_cursor

  subroutine giza_intern_label_f2c(labelx,labely,title)
    implicit none
    character(len=*),intent(in) :: labelx,labely,title
    
    call giza_label_c(cstring(labelx),cstring(labely),cstring(title))

  end subroutine giza_intern_label_f2c

  subroutine giza_intern_set_line_width_int(lw)
    implicit none
    integer(kind=c_int),value,intent(in) :: lw
    real(kind=c_double) :: reallw
    
    !--cairo line widths are best as .5, 1.5 etc
    reallw = dble(lw) + 0.5d0
    call giza_set_line_width(reallw)
  end subroutine giza_intern_set_line_width_int

  subroutine giza_intern_get_line_width_int(lw)
    implicit none
    integer(kind=c_int),intent(out) :: lw
    real(kind=c_double) :: reallw

    call giza_get_line_width(reallw)
    lw = nint(reallw - 0.5d0)
  end subroutine giza_intern_get_line_width_int

  subroutine giza_intern_text_f2c(x,y,text)
    implicit none
    real(kind=c_double),intent(in) :: x,y
    character(len=*),intent(in)    :: text

    call giza_text_c(x,y,cstring(text))
  end subroutine giza_intern_text_f2c

 subroutine giza_intern_text_float_f2c(x,y,text)
    implicit none
    real(kind=c_float),intent(in) :: x,y
    character(len=*),intent(in)    :: text
    
    call giza_text_c(x,y,cstring(text))
  end subroutine giza_intern_text_float_f2c

  subroutine giza_intern_ptext_f2c(x,y,angle,just,text)
    implicit none
    real(kind=c_double),intent(in) :: x,y,angle,just
    character(len=*),intent(in)    :: text

    call giza_ptext_c(x,y,angle,just,cstring(text))
  end subroutine giza_intern_ptext_f2c

 subroutine giza_intern_ptext_float_f2c(x,y,angle,just,text)
    implicit none
    real(kind=c_float),intent(in) :: x,y,angle,just
    character(len=*),intent(in)    :: text
    
    call giza_ptext_c(x,y,angle,just,cstring(text))
  end subroutine giza_intern_ptext_float_f2c

  subroutine giza_intern_qtext_f2c(x,y,angle,just,text,xbox,ybox)
    implicit none
    real(kind=c_double),intent(in) :: x,y,angle,just
    real(kind=c_double),intent(out):: xbox(4),ybox(4)
    character(len=*),intent(in)    :: text

    call giza_qtext_c(x,y,angle,just,cstring(text),xbox,ybox)
  end subroutine giza_intern_qtext_f2c

  subroutine giza_intern_qtext_float_f2c(x,y,angle,just,text,xbox,ybox)
    implicit none
    real(kind=c_float),intent(in) :: x,y,angle,just
    real(kind=c_float),intent(out):: xbox(4),ybox(4)
    character(len=*),intent(in)   :: text
    
    call giza_qtext_c(x,y,angle,just,cstring(text),xbox,ybox)
  end subroutine giza_intern_qtext_float_f2c

  subroutine giza_intern_qtextlen_f2c(units,text,xlen,ylen)
    implicit none
    integer(kind=c_int),intent(in) :: units
    character(len=*),intent(in)    :: text
    real(kind=c_double),intent(out):: xlen,ylen

    call giza_qtextlen_c(units,cstring(text),xlen,ylen)
  end subroutine giza_intern_qtextlen_f2c

  subroutine giza_intern_qtextlen_float_f2c(units,text,xlen,ylen)
    implicit none
    integer(kind=c_int),intent(in) :: units
    character(len=*),intent(in)    :: text
    real(kind=c_float),intent(out) :: xlen,ylen

    call giza_qtextlen_float_c(units,cstring(text),xlen,ylen)
  end subroutine giza_intern_qtextlen_float_f2c

  subroutine giza_intern_set_font_f2c(font)
    implicit none
    character(len=*),intent(in) :: font
  
    call giza_set_font_c(cstring(font))

  end subroutine giza_intern_set_font_f2c

  subroutine giza_intern_set_font_bold_f2c(font)
    implicit none
    character(len=*),intent(in) :: font
  
    call giza_set_font_bold_c(cstring(font))

  end subroutine giza_intern_set_font_bold_f2c

  subroutine giza_intern_set_font_italic_f2c(font)
    implicit none
    character(len=*),intent(in) :: font
  
    call giza_set_font_italic_c(cstring(font))

  end subroutine giza_intern_set_font_italic_f2c

  subroutine giza_intern_set_font_bold_italic_f2c(font)
    implicit none
    character(len=*),intent(in) :: font
  
    call giza_set_font_bold_italic_c(cstring(font))

  end subroutine giza_intern_set_font_bold_italic_f2c

  subroutine giza_format_number_f2c(mantissa,power,iform,string)
    implicit none
    integer(kind=c_int),intent(in) :: mantissa,power,iform
    character(len=*),  intent(out) :: string
    character(kind=c_char), dimension(len(string)+1) :: stringc
  
    call giza_format_number_c(mantissa,power,iform,stringc)
    string = fstring(stringc)

  end subroutine giza_format_number_f2c

  subroutine giza_query_device_f2c(qtype,string)
    implicit none
    character(len=*),intent(in)  :: qtype
    character(len=*),intent(out) :: string
    integer(kind=c_int) :: ierr
    character(kind=c_char), dimension(len(string)+1) :: stringc
    
    ierr = giza_query_device_c(cstring(qtype),stringc)
    string = fstring(stringc)
  
  end subroutine giza_query_device_f2c


  !---------------------------------------------------------------------------
  !
  ! function to safely convert a string to c format (ie. with a terminating
  ! ascii null character)
  !
  !---------------------------------------------------------------------------
  function cstring(string) result(array)
    implicit none
    character(len=*), intent(in) :: string
    character(kind=c_char), dimension(len(string)+1) :: array
    integer :: i, ilen

    ilen = len_trim(string) ! use the trimmed string length
    do i=1, ilen
      array(i)=string(i:i)
    end do
    array(ilen+1)=achar(0)

  end function cstring

  !---------------------------------------------------------------------------
  !
  ! function to safely convert a string from c format (ie. with a terminating
  ! ascii null character) back to a normal Fortran string
  !
  !---------------------------------------------------------------------------
   function fstring(array)
    implicit none
    character(kind=c_char), dimension(:), intent(in) :: array
    character(len=size(array)-1) :: fstring
    integer :: i

    fstring = ''
    do i=1,size(array)
       if (array(i).eq.achar(0)) exit
       fstring(i:i) = array(i)
    enddo

   end function fstring

end module giza
