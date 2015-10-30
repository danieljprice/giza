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
!  This file contains the Fortran interface to giza
!  (using the Fortran 2003 iso_c_binding module)
!---------------------------------------------------------

module giza
 use, intrinsic :: iso_c_binding, only:c_double,c_float,c_char,c_int
 implicit none
 public  :: &
      giza_plot, &
      giza_arrow, &
      giza_set_arrow_style, &
      giza_get_arrow_style, &
      giza_annotate, &
      giza_band, &
      giza_box, &
      giza_box_time, &
      giza_begin_buffer, &
      giza_end_buffer, &
      giza_flush_buffer, &
      giza_circle, &
      giza_set_character_height, &
      giza_get_character_height, &
      giza_get_character_size, &
      giza_set_clipping, &
      giza_get_clipping, &
      giza_colour_bar, &
      giza_set_colour_index, &
      giza_get_colour_index, &
      giza_set_colour_index_range, &
      giza_get_colour_index_range, &
      giza_set_colour_palette, &
      giza_set_colour_representation, &
      giza_get_colour_representation, &
      giza_set_colour_representation_hls, &
      giza_set_colour_table, &
      giza_contour, &
      giza_get_current_point, &
      giza_rgb_from_table, &
      giza_print_device_list, &
      giza_open, &
      giza_open_device, &
      giza_open_device_size, &
      giza_get_device_id, &
      giza_select_device, &
      giza_flush_device, &
      giza_change_page, &
      giza_close_device, &
      giza_close, &
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
      giza_histogram, &
      giza_histogram_binned, &
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
      giza_subpanel, &
      giza_set_panel, &
      giza_get_panel, &
      giza_vector, &
      giza_set_viewport_default, &
      giza_set_viewport, &
      giza_get_viewport, &
      giza_set_viewport_inches, &
      giza_version, &
      giza_start_warnings, &
      giza_stop_warnings, &
      giza_set_window, &
      giza_set_window_equal_scale, &
      giza_get_window, &
      giza_format_number, &
      giza_query_device

#include "giza-shared.h"
  character(len=1),parameter, public :: giza_left_click_f = GIZA_LEFT_CLICK
  character(len=1),parameter, public :: giza_right_click_f = GIZA_RIGHT_CLICK
  character(len=1),parameter, public :: giza_middle_click_f = GIZA_MIDDLE_CLICK
  character(len=1),parameter, public :: giza_shift_click_f = achar(GIZA_SHIFT_CLICK)
  character(len=1),parameter, public :: giza_scroll_up_f = achar(GIZA_SCROLL_UP)
  character(len=1),parameter, public :: giza_scroll_down_f = achar(GIZA_SCROLL_DOWN)
  character(len=1),parameter, public :: giza_scroll_left_f = achar(GIZA_SCROLL_LEFT)
  character(len=1),parameter, public :: giza_scroll_right_f = achar(GIZA_SCROLL_RIGHT)
  character(len=*),parameter, public :: giza_version_string = GIZA_VERSION_STRING
  integer, parameter, public :: giza_version_major = GIZA_VERSION_MAJOR
  integer, parameter, public :: giza_version_minor = GIZA_VERSION_MINOR
  integer, parameter, public :: giza_version_micro = GIZA_VERSION_MICRO
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
  integer, parameter, public :: giza_extend_none = GIZA_EXTEND_NONE
  integer, parameter, public :: giza_extend_repeat = GIZA_EXTEND_REPEAT
  integer, parameter, public :: giza_extend_reflect = GIZA_EXTEND_REFLECT
  integer, parameter, public :: giza_extend_pad = GIZA_EXTEND_PAD

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

 interface giza_box_time
    module procedure giza_intern_box_time_f2c
 end interface
 
 interface giza_box_c
    subroutine giza_box_float_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box_float")
      import
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_float), value, intent(in)    :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_float_c
    
    subroutine giza_box_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box")
      import
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_double), value, intent(in)   :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_c
 end interface

 interface giza_box_time_c
    subroutine giza_box_time_float_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box_time_float")
      import
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_float), value, intent(in)    :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_time_float_c
    
    subroutine giza_box_time_c(xopt,xtick,nxsub,yopt,ytick,nysub) bind(C,name="giza_box_time")
      import
      character(kind=c_char),dimension(*), intent(in) :: xopt,yopt
      real(kind=c_double), value, intent(in)   :: xtick,ytick
      integer(kind=c_int), value, intent(in)   :: nxsub,nysub
    end subroutine giza_box_time_c
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
      real(kind=c_double),intent(in),value :: x,y,r
    end subroutine giza_circle

    subroutine giza_circle_float(x,y,r) bind(C)
      import
      real(kind=c_float),intent(in),value :: x,y,r
    end subroutine giza_circle_float
 end interface

 interface giza_set_character_height 
    subroutine giza_set_character_height(ch) bind(C)
      import
      real(kind=c_double),intent(in),value :: ch
    end subroutine giza_set_character_height
    
    subroutine giza_set_character_height_float(ch) bind(C)
      import
      real(kind=c_float),intent(in),value :: ch
    end subroutine giza_set_character_height_float
 end interface
 
 interface giza_get_character_height
    subroutine giza_get_character_height(ch) bind(C)
      import
      real(kind=c_double),intent(out) :: ch
    end subroutine giza_get_character_height
    
    subroutine giza_get_character_height_float(ch) bind(C)
      import
      real(kind=c_float),intent(out) :: ch
    end subroutine giza_get_character_height_float
 end interface

 interface giza_get_character_size
    subroutine giza_get_character_size(units,xch,ych) bind(C)
      import
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_double),intent(out)      :: xch,ych
    end subroutine giza_get_character_size
    
    subroutine giza_get_character_size_float(units,xch,ych) bind(C)
      import
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_float),intent(out)       :: xch,ych
    end subroutine giza_get_character_size_float
 end interface

 interface giza_set_clipping
    subroutine giza_set_clipping(clip) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: clip
    end subroutine giza_set_clipping
 end interface

 interface giza_get_clipping
    subroutine giza_get_clipping(clip) bind(C)
      import
      integer(kind=c_int),intent(out) :: clip
    end subroutine giza_get_clipping
 end interface

 interface giza_colour_bar
    module procedure giza_intern_colour_bar_f2c
 end interface

 interface giza_colour_bar_c
    subroutine giza_colour_bar_c(side,disp,width,valmin,valmax,label) bind(C,name="giza_colour_bar")
      import
      character(kind=c_char), dimension(*), intent(in) :: side, label
      real(kind=c_double), value, intent(in) :: disp,width,valmin,valmax
    end subroutine giza_colour_bar_c

    subroutine giza_colour_bar_float_c(side,disp,width,valmin,valmax,label) bind(C,name="giza_colour_bar_float")
      import
      character(kind=c_char), dimension(*), intent(in) :: side, label
      real(kind=c_float), value, intent(in) :: disp,width,valmin,valmax
    end subroutine giza_colour_bar_float_c
 end interface

 interface giza_set_colour_index
    subroutine giza_set_colour_index(ci) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: ci
    end subroutine giza_set_colour_index
 end interface

 interface giza_get_colour_index
    subroutine giza_get_colour_index(ci) bind(C)
      import
      integer(kind=c_int),intent(out) :: ci
    end subroutine giza_get_colour_index
 end interface

 interface giza_set_colour_palette
    subroutine giza_set_colour_palette(palette) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: palette
    end subroutine giza_set_colour_palette
 end interface

 interface giza_set_colour_index_range
    subroutine giza_set_colour_index_range(cimin,cimax) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: cimin,cimax
    end subroutine giza_set_colour_index_range
 end interface

 interface giza_get_colour_index_range
    subroutine giza_get_colour_index_range(cimin,cimax) bind(C)
      import
      integer(kind=c_int),intent(out) :: cimin,cimax
    end subroutine giza_get_colour_index_range
 end interface

 interface giza_set_colour_representation
    subroutine giza_set_colour_representation_float(ci,red,green,blue) bind(C)
      import
      integer(kind=c_int), value, intent(in)    :: ci
      real(kind=c_float), value, intent(in)     :: red,green,blue
    end subroutine giza_set_colour_representation_float
    
    subroutine giza_set_colour_representation(ci,red,green,blue) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: ci
      real(kind=c_double), value, intent(in) :: red,green,blue
    end subroutine giza_set_colour_representation

    subroutine giza_set_colour_representation_alpha_float(ci,red,green,blue,alpha) bind(C)
      import
      integer(kind=c_int), value, intent(in)    :: ci
      real(kind=c_float), value, intent(in)     :: red,green,blue,alpha
    end subroutine giza_set_colour_representation_alpha_float
    
    subroutine giza_set_colour_representation_alpha(ci,red,green,blue,alpha) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: ci
      real(kind=c_double), value, intent(in) :: red,green,blue,alpha
    end subroutine giza_set_colour_representation_alpha
 end interface

 interface giza_get_colour_representation
    subroutine giza_get_colour_representation(ci,red,green,blue) bind(C)
      import
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_double),intent(out) :: red,green,blue
    end subroutine giza_get_colour_representation
   
    subroutine giza_get_colour_representation_float(ci,red,green,blue) bind(C)
      import
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_float),intent(out) :: red,green,blue
    end subroutine giza_get_colour_representation_float

    subroutine giza_get_colour_representation_alpha(ci,red,green,blue,alpha) bind(C)
      import
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_double),intent(out) :: red,green,blue,alpha
    end subroutine giza_get_colour_representation_alpha
   
    subroutine giza_get_colour_representation_alpha_float(ci,red,green,blue,alpha) bind(C)
      import
      integer(kind=c_int),intent(in),value       :: ci
      real(kind=c_float),intent(out) :: red,green,blue,alpha
    end subroutine giza_get_colour_representation_alpha_float
 end interface

 interface giza_set_colour_representation_hls
    subroutine giza_set_colour_representation_hls(ci,hue,light,sat) bind(C)
      import
      integer(kind=c_int),intent(in),value :: ci
      real(kind=c_double),intent(in),value :: hue,light,sat
    end subroutine giza_set_colour_representation_hls

    subroutine giza_set_colour_representation_hls_float(ci,hue,light,sat) bind(C)
      import
      integer(kind=c_int),intent(in),value :: ci
      real(kind=c_float),intent(in),value  :: hue,light,sat
    end subroutine giza_set_colour_representation_hls_float
 end interface

 interface giza_set_colour_table
    subroutine giza_set_colour_table(controlPoints,red,green,blue,n,cont,bright) bind(C)
      import
      integer(kind=c_int),intent(in),value        :: n
      real(kind=c_double),intent(in),dimension(n) :: controlPoints,red,green,blue
      real(kind=c_double),intent(in),value        :: cont,bright
    end subroutine giza_set_colour_table
    
    subroutine giza_set_colour_table_float(controlPoints,red,green,blue,n,cont,bright) bind(C)
      import
      integer(kind=c_int),intent(in),value       :: n
      real(kind=c_float),intent(in),dimension(n) :: controlPoints,red,green,blue
      real(kind=c_float),intent(in),value        :: cont,bright
    end subroutine giza_set_colour_table_float
 end interface

 interface giza_contour
    subroutine giza_contour(sizex,sizey,data,i1,i2,j1,j2,ncont,cont,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,ncont
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in) :: cont(ncont)
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_contour
    
    subroutine giza_contour_float(sizex,sizey,data,i1,i2,j1,j2,ncont,cont,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,ncont
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in) :: cont(ncont)
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_contour_float
 end interface

 interface giza_get_current_point
    subroutine giza_get_current_point(xpt,ypt) bind(C)
      import
      real(kind=c_double),intent(out) :: xpt,ypt
    end subroutine giza_get_current_point
    
    subroutine giza_get_current_point_float(xpt,ypt) bind(C)
      import
      real(kind=c_float),intent(out) :: xpt,ypt
    end subroutine giza_get_current_point_float
 end interface

 interface giza_rgb_from_table
    subroutine giza_rgb_from_table(pos,r,g,b) bind(C)
      import
      real(kind=c_double),intent(in),value :: pos
      real(kind=c_double),intent(out) :: r,g,b
    end subroutine giza_rgb_from_table

    subroutine giza_rgb_from_table_float(pos,r,g,b) bind(C)
      import
      real(kind=c_float),intent(in),value :: pos
      real(kind=c_float),intent(out) :: r,g,b
    end subroutine giza_rgb_from_table_float
 end interface

 interface giza_print_device_list
    subroutine giza_print_device_list() bind(C)
    end subroutine  giza_print_device_list
 end interface

 interface giza_open_device
    module procedure giza_intern_open_device
 end interface

 interface giza_open_device_c
    integer(kind=c_int) function giza_open_device_c (dev,prefix) bind(C,name="giza_open_device")
      import
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
      character(kind=c_char),dimension(*),intent(in) :: dev,prefix
      real(kind=c_double),intent(in),value    :: height,width
      integer(kind=c_int),intent(in),value    :: units
    end function giza_open_device_size_c

    integer(kind=c_int) function giza_open_device_size_float_c(dev,prefix,width,height,units) &
                        bind(C,name="giza_open_device_size_float")
      import
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

 interface giza_select_device
    subroutine giza_select_device(devid) bind(C)
      import
      integer(kind=c_int),intent(in),value :: devid
    end subroutine giza_select_device
 end interface

 interface giza_get_device_id
    subroutine giza_get_device_id(devid) bind(C)
      import
      integer(kind=c_int),intent(out) :: devid
    end subroutine giza_get_device_id
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
      integer(kind=c_int)                :: giza_get_key_press
      real(kind=c_double),intent(out)    :: x,y
      character(kind=c_char),intent(out) :: ch
    end function giza_get_key_press

    function giza_get_key_press_float(x,y,ch) bind(C)
      import
      integer(kind=c_int)                :: giza_get_key_press_float
      real(kind=c_float),intent(out)     :: x,y
      character(kind=c_char),intent(out) :: ch
    end function giza_get_key_press_float
 end interface

 interface giza_draw
    subroutine giza_draw(x,y) bind(C)
      import
      real(kind=c_double),intent(in),value :: x,y
    end subroutine giza_draw

    subroutine giza_draw_float(x,y) bind(C)
      import
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
      real(kind=c_double),intent(in),value :: xmin,xmax,ymin,ymax
      integer(kind=c_int),intent(in),value :: just,axis
    end subroutine giza_set_environment

    subroutine giza_set_environment_float(xmin,xmax,ymin,ymax,just,axis) bind(C)
      import
      real(c_float),intent(in),value       :: xmin,xmax,ymin,ymax
      integer(kind=c_int),intent(in),value :: just,axis
    end subroutine giza_set_environment_float
 end interface

 interface giza_error_bars
    subroutine giza_error_bars(dir,n,xpts,ypts,error,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: dir,n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts,error
    end subroutine giza_error_bars

    subroutine giza_error_bars_float(dir,n,xpts,ypts,error,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: dir,n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts,error
    end subroutine giza_error_bars_float
 end interface

 interface giza_error_bars_x
    subroutine giza_error_bars_hori(n,xpts,ypts1,ypts2,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_hori

    subroutine giza_error_bars_hori_float(n,xpts,ypts1,ypts2,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_hori_float
 end interface

 interface giza_error_bars_y
    subroutine giza_error_bars_vert(n,xpts,ypts1,ypts2,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_double),intent(in),value :: term
      real(kind=c_double),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_vert

    subroutine giza_error_bars_vert_float(n,xpts,ypts1,ypts2,term) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n
      real(kind=c_float),intent(in),value :: term
      real(kind=c_float),dimension(n),intent(in) :: xpts,ypts1,ypts2
    end subroutine giza_error_bars_vert_float
 end interface

 interface giza_set_fill
    subroutine giza_set_fill(fs) bind(C)
      import
      integer(kind=c_int),intent(in),value :: fs
    end subroutine giza_set_fill
 end interface
 
 interface giza_get_fill
    subroutine giza_get_fill(fs) bind(C)
      import
      integer(kind=c_int),intent(out) :: fs
    end subroutine giza_get_fill
 end interface

 interface giza_function_x
    subroutine giza_function_x(func,n,xmin,xmax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: xmin,xmax
      abstract interface
       function func(x) bind(c)
         import
         real(c_double), intent(in) :: x
         real(c_double) :: func
       end function
      end interface
    end subroutine giza_function_x

    subroutine giza_function_x_float(func,n,xmin,xmax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: xmin,xmax
      abstract interface
       function func(x) bind(c)
         import
         real(c_float), intent(in) :: x
         real(c_float) :: func
       end function
      end interface
    end subroutine giza_function_x_float
 end interface

 interface giza_function_y
    subroutine giza_function_y(func,n,ymin,ymax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: ymin,ymax
      abstract interface
       function func(x) bind(c)
         import
         real(c_double), intent(in) :: x
         real(c_double) :: func
       end function
      end interface
    end subroutine giza_function_y

    subroutine giza_function_y_float(func,n,ymin,ymax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: ymin,ymax
      abstract interface
       function func(x) bind(c)
         import
         real(c_float), intent(in) :: x
         real(c_float) :: func
       end function
      end interface
    end subroutine giza_function_y_float
 end interface

 interface giza_function_t
    subroutine giza_function_t(funcx,funcy,n,tmin,tmax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_double),intent(in),value :: tmin,tmax
      abstract interface
       function funcx(x) bind(c)
         import
         real(c_double), intent(in) :: x
         real(c_double) :: funcx
       end function
       function funcy(x) bind(c)
         import
         real(c_double), intent(in) :: x
         real(c_double) :: funcy
       end function
      end interface
    end subroutine giza_function_t

    subroutine giza_function_t_float(funcx,funcy,n,tmin,tmax,flag) bind(C)
      import
      integer(kind=c_int),intent(in),value :: n,flag
      real(kind=c_float), intent(in),value :: tmin,tmax
!      real(kind=c_float),external :: funcx,funcy
      abstract interface
       function funcx(x) bind(c)
         import
         real(c_float), intent(in) :: x
         real(c_float) :: funcx
       end function
       function funcy(x) bind(c)
         import
         real(c_float), intent(in) :: x
         real(c_float) :: funcy
       end function
      end interface
    end subroutine giza_function_t_float
 end interface

 interface giza_get_surface_size
    subroutine giza_get_surface_size_float(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_float),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_surface_size_float

    subroutine giza_get_surface_size(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_double),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_surface_size
 end interface
 
 interface giza_set_hatching_style
    subroutine giza_set_hatching_style(angle,spacing,phase) bind(C)
      import
      real(kind=c_double),intent(in),value :: angle,spacing,phase
    end subroutine giza_set_hatching_style

    subroutine giza_set_hatching_style_float(angle,spacing,phase) bind(C)
      import
      real(kind=c_float),intent(in),value :: angle,spacing,phase
    end subroutine giza_set_hatching_style_float
 end interface giza_set_hatching_style

 interface giza_get_hatching_style
    subroutine giza_get_hatching_style(angle,spacing,phase) bind(C)
      import
      real(kind=c_double),intent(out) :: angle,spacing,phase
    end subroutine giza_get_hatching_style

    subroutine giza_get_hatching_style_float(angle,spacing,phase) bind(C)
      import
      real(kind=c_float),intent(out) :: angle,spacing,phase
    end subroutine giza_get_hatching_style_float
 end interface giza_get_hatching_style

 interface giza_histogram
    subroutine giza_histogram(n,dat,min,max,nbin,flag) bind(C)
      import
      integer(kind=c_int),intent(in), value :: n,nbin,flag
      real(kind=c_double),intent(in) :: dat(*)
      real(kind=c_double),intent(in), value :: min,max
    end subroutine giza_histogram

    subroutine giza_histogram_float(n,dat,min,max,nbin,flag) bind(C)
      import
      integer(kind=c_int),intent(in), value :: n,nbin,flag
      real(kind=c_float), intent(in) :: dat(*)
      real(kind=c_float), intent(in), value :: min,max
    end subroutine giza_histogram_float
 end interface

 interface giza_histogram_binned
    subroutine giza_histogram_binned(nbin,x,dat,centre) bind(C)
      import
      integer(kind=c_int),intent(in), value :: nbin,centre
      real(kind=c_double),intent(in) :: x(*),dat(*)
    end subroutine giza_histogram_binned

    subroutine giza_histogram_binned_float(nbin,x,dat,centre) bind(C)
      import
      integer(kind=c_int),intent(in), value :: nbin,centre
      real(kind=c_float), intent(in) :: x(*),dat(*)
    end subroutine giza_histogram_binned_float
 end interface

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
      integer(kind=c_int),value,intent(in)       :: n
      real(kind=c_float),intent(in),dimension(n) :: xpts,ypts
    end subroutine giza_line_float

    subroutine giza_line(n,xpts,ypts) bind(C)
      import
      integer(kind=c_int),value,intent(in)        :: n
      real(kind=c_double),intent(in),dimension(n) :: xpts,ypts
    end subroutine giza_line
 end interface
 
 interface giza_set_line_width
    subroutine giza_set_line_width(lw) bind(C)
      import
      real(kind=c_double),intent(in), value :: lw
    end subroutine giza_set_line_width
    
    subroutine giza_set_line_width_float(lw) bind(C)
      import
      real(kind=c_float),intent(in), value :: lw
    end subroutine giza_set_line_width_float
    
    module procedure giza_intern_set_line_width_int
 end interface
 
 interface giza_get_line_width
    subroutine giza_get_line_width(lw) bind(C)
      import
      real(kind=c_double),intent(out) :: lw
    end subroutine giza_get_line_width
    
    subroutine giza_get_line_width_float(lw) bind(C)
      import
      real(kind=c_float),intent(out) :: lw
    end subroutine giza_get_line_width_float
    
    module procedure giza_intern_get_line_width_int
 end interface

 interface giza_set_line_cap
    subroutine giza_set_line_cap(lc) bind(C)
      import
      integer(c_int),value,intent(in) :: lc
    end subroutine giza_set_line_cap
 end interface

 interface giza_get_line_cap
    subroutine giza_get_line_cap(lc) bind(C)
      import
      integer(c_int),intent(out) :: lc
    end subroutine giza_get_line_cap
 end interface

 interface giza_set_line_style
    subroutine giza_set_line_style(ls) bind(C)
      import
      integer(c_int),value,intent(in) :: ls
    end subroutine giza_set_line_style
 end interface

 interface giza_get_line_style
    subroutine giza_get_line_style(ls) bind(C)
      import
      integer(c_int),intent(out) :: ls
    end subroutine giza_get_line_style
 end interface
 
 interface giza_mark_line
    subroutine giza_mark_line(maxpts,npts,xpts,ypts) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line
    
    subroutine giza_mark_line_float(maxpts,npts,xpts,ypts) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_float

    subroutine giza_mark_line_char(maxpts,npts,xpts,ypts,ch) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
      character(kind=c_char),intent(out)   :: ch
    end subroutine giza_mark_line_char
    
    subroutine giza_mark_line_char_float(maxpts,npts,xpts,ypts,ch) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
      character(kind=c_char),intent(out)   :: ch
    end subroutine giza_mark_line_char_float
 end interface

 interface giza_mark_line_ordered
    subroutine giza_mark_line_ordered(maxpts,npts,xpts,ypts) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_ordered
    
    subroutine giza_mark_line_ordered_float(maxpts,npts,xpts,ypts) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_float), dimension(*), intent(inout) :: xpts
      real(kind=c_float), dimension(*), intent(inout) :: ypts
    end subroutine giza_mark_line_ordered_float
 end interface

 interface giza_mark_points
    subroutine giza_mark_points(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points
    
    subroutine giza_mark_points_float(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
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
      integer(kind=c_int),intent(in),value :: maxpts
      integer(kind=c_int),intent(inout) :: npts
      real(kind=c_double), dimension(*), intent(inout) :: xpts
      real(kind=c_double), dimension(*), intent(inout) :: ypts
      integer(kind=c_int),intent(in),value :: symbol
    end subroutine giza_mark_points_ordered
    
    subroutine giza_mark_points_ordered_float(maxpts,npts,xpts,ypts,symbol) bind(C)
      import
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
      real(kind=c_double),intent(in),value :: x,y
    end subroutine giza_move

    subroutine giza_move_float(x,y) bind(C)
      import
      real(kind=c_float),intent(in),value :: x,y
    end subroutine giza_move_float
 end interface

 interface giza_get_paper_size
    subroutine giza_get_paper_size(units,width,height) bind(C)
      import
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_double),intent(out) :: width,height
    end subroutine giza_get_paper_size

    subroutine giza_get_paper_size_float(units,width,height) bind(C)
      import
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_float),intent(out) :: width,height
    end subroutine giza_get_paper_size_float
 end interface

 interface giza_set_paper_size
    subroutine giza_set_paper_size(units,width,aspect) bind(C)
      import
      integer(kind=c_int), intent(in),value :: units
      real(kind=c_double), intent(in),value :: width,aspect
    end subroutine giza_set_paper_size

    subroutine giza_set_paper_size_float(units,width,aspect) bind(C)
      import
      integer(kind=c_int),intent(in),value :: units
      real(kind=c_float), intent(in),value :: width,aspect
    end subroutine giza_set_paper_size_float
 end interface

 interface giza_points
    subroutine giza_points(n,xpts,ypts,symbol) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: n, symbol
      real(kind=c_double), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_points

    subroutine giza_points_float(n,xpts,ypts,symbol) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: n, symbol
      real(kind=c_float), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_points_float
 end interface

 interface giza_single_point
    subroutine giza_single_point(x,y,symbol) bind(C)
      import
      real(kind=c_double), value, intent(in) :: x,y
      integer(kind=c_int), value, intent(in) :: symbol
    end subroutine giza_single_point

    subroutine giza_single_point_float(x,y,symbol) bind(C)
      import
      real(kind=c_float),  value, intent(in) :: x,y
      integer(kind=c_int), value, intent(in) :: symbol
    end subroutine giza_single_point_float
 end interface

 interface giza_polygon
    subroutine giza_polygon(n,xpts,ypts) bind(C)
      import
      integer(kind=c_int), value, intent(in) :: n
      real(kind=c_double), dimension(n), intent(in) :: xpts,ypts
    end subroutine giza_polygon

    subroutine giza_polygon_float(n,xpts,ypts) bind(C)
      import
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
      integer(kind=c_int), value, intent(in) :: ci
    end subroutine giza_set_text_background
 end interface

 interface giza_get_text_background
    subroutine giza_get_text_background(ci) bind(C)
      import
      integer(kind=c_int), intent(out) :: ci
    end subroutine giza_get_text_background
 end interface

 interface giza_text_c
    subroutine giza_text_c(x,y,text) bind(C,name="giza_text")
      import
      real(kind=c_double),value,intent(in)    :: x,y
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_text_c

    subroutine giza_text_float_c(x,y,text) bind(C,name="giza_text_float")
      import
      real(kind=c_float),value,intent(in)     :: x,y
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_text_float_c
 end interface

 interface giza_open
    module procedure giza_open_sub
 end interface giza_open

 interface giza_ptext
    module procedure giza_intern_ptext_f2c    
    module procedure giza_intern_ptext_float_f2c
 end interface

 interface giza_ptext_c
    subroutine giza_ptext_c(x,y,angle,just,text) bind(C,name="giza_ptext")
      import
      real(kind=c_double),value,intent(in)    :: x,y,angle,just
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_ptext_c

    subroutine giza_ptext_float_c(x,y,angle,just,text) bind(C,name="giza_ptext_float")
      import
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
      real(kind=c_double),value,intent(in)    :: x,y,angle,just
      real(kind=c_double),intent(out)         :: xbox(4),ybox(4)
      character(kind=c_char),dimension(*),intent(in) :: text
    end subroutine giza_qtext_c

    subroutine giza_qtext_float_c(x,y,angle,just,text,xbox,ybox) bind(C,name="giza_qtext_float")
      import
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
      integer(kind=c_int),value,intent(in)    :: units
      character(kind=c_char),dimension(*),intent(in) :: text
      real(kind=c_double),intent(out)         :: xlen,ylen
    end subroutine giza_qtextlen_c

    subroutine giza_qtextlen_float_c(units,text,xlen,ylen) bind(C,name="giza_qtextlen_float")
      import
      integer(kind=c_int),value,intent(in)    :: units
      character(kind=c_char),dimension(*),intent(in) :: text
      real(kind=c_float),intent(out)          :: xlen,ylen
    end subroutine giza_qtextlen_float_c
 end interface

 interface giza_render
    subroutine giza_render(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render
    
    subroutine giza_render_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_float

    subroutine giza_render_alpha(sizex,sizey,data,alpha,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_double),intent(in) :: data(sizex,sizey), alpha(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render_alpha
    
    subroutine giza_render_alpha_float(sizex,sizey,data,alpha,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_float),intent(in) :: data(sizex,sizey), alpha(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_alpha_float
 end interface

 interface giza_render_transparent
    subroutine giza_render_transparent(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render_transparent
    
    subroutine giza_render_transparent_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_transparent_float
 end interface

 interface giza_render_gray
    subroutine giza_render_gray(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_double),intent(in) :: data(sizex,sizey)
      real(kind=c_double),intent(in),value :: valMin,valMax
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_render_gray
    
    subroutine giza_render_gray_float(sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,extend,affine) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      real(kind=c_float),intent(in) :: data(sizex,sizey)
      real(kind=c_float),intent(in),value :: valMin,valMax
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_render_gray_float
 end interface

 interface giza_draw_pixels
    subroutine giza_draw_pixels(sizex,sizey,idata,i1,i2,j1,j2,xmin,xmax,ymin,ymax,extend) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      integer(kind=c_int),intent(in) :: idata(sizex,sizey)
      real(kind=c_double),intent(in),value :: xmin,xmax,ymin,ymax
    end subroutine giza_draw_pixels
    
    subroutine giza_draw_pixels_float(sizex,sizey,idata,i1,i2,j1,j2,xmin,xmax,ymin,ymax,extend) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,extend
      integer(kind=c_int),intent(in) :: idata(sizex,sizey)
      real(kind=c_float),intent(in),value :: xmin,xmax,ymin,ymax
    end subroutine giza_draw_pixels_float
 end interface

 interface giza_rectangle
    subroutine giza_rectangle(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_double),intent(in),value :: x1,x2,y1,y2
    end subroutine giza_rectangle

    subroutine giza_rectangle_float(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_float),intent(in),value :: x1,x2,y1,y2
    end subroutine giza_rectangle_float

    subroutine giza_rectangle_rounded(x1,x2,y1,y2,r) bind(C)
      import
      real(kind=c_double),intent(in),value :: x1,x2,y1,y2,r
    end subroutine giza_rectangle_rounded

    subroutine giza_rectangle_rounded_float(x1,x2,y1,y2,r) bind(C)
      import
      real(kind=c_float),intent(in),value :: x1,x2,y1,y2,r
    end subroutine giza_rectangle_rounded_float
 end interface giza_rectangle

 interface giza_restore
    subroutine giza_restore() bind(C)
    end subroutine giza_restore
 end interface

 interface giza_round
    function giza_round_db(x,nsub) bind(C,name="giza_round")
      import
      real(kind=c_double), intent(in) :: x
      integer(kind=c_int), intent(out) :: nsub
      real(kind=c_double) :: giza_round_db
    end function giza_round_db

    function giza_round_float(x,nsub) bind(C,name="giza_round_float")
      import
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
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_c
 end interface
 interface giza_set_font_bold_c
    subroutine giza_set_font_bold_c(font) bind(C,name="giza_set_font_bold")
      import
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_bold_c
 end interface
 interface giza_set_font_italic_c
    subroutine giza_set_font_italic_c(font) bind(C,name="giza_set_font_italic")
      import
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_italic_c
 end interface
 interface giza_set_font_bold_italic_c
    subroutine giza_set_font_bold_italic_c(font) bind(C,name="giza_set_font_bold_italic")
      import
      character(kind=c_char),dimension(*),intent(in) :: font
    end subroutine giza_set_font_bold_italic_c
 end interface

 interface giza_subpanel
    subroutine giza_subpanel(nx,ny) bind(C)
      import
      integer(kind=c_int), intent(in),value :: nx,ny
    end subroutine giza_subpanel
 end interface
 
 interface giza_set_panel
    subroutine giza_set_panel(ix,iy) bind(C)
      import
      integer(kind=c_int), intent(in),value :: ix,iy
    end subroutine giza_set_panel
 end interface

 interface giza_get_panel
    subroutine giza_get_panel(ix,iy) bind(C)
      import
      integer(kind=c_int), intent(out) :: ix,iy
    end subroutine giza_get_panel
 end interface

 interface giza_vector
    subroutine giza_vector(sizex,sizey,horizontal,vertical,i1,i2,j1,j2,scale,position,affine,blank) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,position
      real(kind=c_double),intent(in) :: horizontal(sizex,sizey),vertical(sizex,sizey)
      real(kind=c_double),intent(in),value :: scale,blank
      real(kind=c_double),intent(in) :: affine(6)
    end subroutine giza_vector
    
    subroutine giza_vector_float(sizex,sizey,horizontal,vertical,i1,i2,j1,j2,scale,position,affine,blank) bind(C)
      import
      integer(kind=c_int),intent(in),value :: sizex,sizey,i1,i2,j1,j2,position
      real(kind=c_float),intent(in) :: horizontal(sizex,sizey),vertical(sizex,sizey)
      real(kind=c_float),intent(in),value :: scale,blank
      real(kind=c_float),intent(in) :: affine(6)
    end subroutine giza_vector_float
 end interface


 interface giza_set_viewport
    subroutine giza_set_viewport_float(xleft,xright,ybot,ytop) bind(C)
      import
      real(kind=c_float), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_float

    subroutine giza_set_viewport(xleft,xright,ybot,ytop) bind(C)
      import
      real(kind=c_double), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport
 end interface

 interface giza_set_viewport_default
    subroutine giza_set_viewport_default() bind(C)
    end subroutine giza_set_viewport_default
 end interface

 interface giza_get_viewport
    subroutine giza_get_viewport(units,x1,x2,y1,y2) bind(C)
      import
      integer(kind=c_int), value, intent(in)  :: units
      real(kind=c_double),        intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_viewport
    
    subroutine giza_get_viewport_float(units,x1,x2,y1,y2) bind(C)
      import
      integer(kind=c_int),value, intent(in)  :: units
      real(kind=c_float),        intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_viewport_float
 end interface

 interface giza_set_viewport_inches
    subroutine giza_set_viewport_inches_float(xleft,xright,ybot,ytop) bind(C)
      import
      real(kind=c_float), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_inches_float

    subroutine giza_set_viewport_inches(xleft,xright,ybot,ytop) bind(C)
      import
      real(kind=c_double), value, intent(in) :: xleft,xright,ybot,ytop
    end subroutine giza_set_viewport_inches
 end interface
 
 interface giza_version
    subroutine giza_version(major,minor,micro) bind(C)
     import
     integer(kind=c_int), intent(out) :: major,minor,micro
    end subroutine giza_version
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
      real(kind=c_float), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_float

    subroutine giza_set_window(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_double), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window
 end interface

 interface giza_set_window_equal_scale
    subroutine giza_set_window_equal_scale(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_double), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_equal_scale

    subroutine giza_set_window_equal_scale_float(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_float), value, intent(in) :: x1,x2,y1,y2
    end subroutine giza_set_window_equal_scale_float
 end interface

 interface giza_get_window
    subroutine giza_get_window(x1,x2,y1,y2) bind(C)
      import
      real(kind=c_double),intent(out) :: x1,x2,y1,y2
    end subroutine giza_get_window

    subroutine giza_get_window_float(x1,x2,y1,y2) bind(C)
      import
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
    character(len=*),intent(in)    :: side,text
    real,intent(in)                :: displacement,coord,justification
    real(kind=c_double)            :: displacement_c,coord_c,justification_c

    displacement_c = displacement
    coord_c = coord
    justification_c = justification

    call giza_annotate_c(cstring(side),displacement_c,coord_c,justification_c,cstring(text))
  end subroutine giza_intern_annotate_f2c

  subroutine giza_intern_box_f2c(xopt,xtick,nxsub,yopt,ytick,nysub)
    implicit none
    character(len=*),intent(in) :: xopt,yopt
    real,intent(in)             :: xtick,ytick
    integer,intent(in)          :: nxsub,nysub
    real(kind=c_double)         :: xtick_c,ytick_c
    integer(kind=c_int)         :: nxsub_c,nysub_c

    xtick_c = xtick; ytick_c = ytick
    nxsub_c = nxsub; nysub_c = nysub

    call giza_box_c(cstring(xopt),xtick_c,nxsub_c,cstring(yopt),ytick_c,nysub_c)
  end subroutine giza_intern_box_f2c

  subroutine giza_intern_box_time_f2c(xopt,xtick,nxsub,yopt,ytick,nysub)
    implicit none
    character(len=*),intent(in) :: xopt,yopt
    real,intent(in)             :: xtick,ytick
    integer,intent(in)          :: nxsub,nysub
    real(kind=c_double)         :: xtick_c,ytick_c
    integer(kind=c_int)         :: nxsub_c,nysub_c

    xtick_c = xtick; ytick_c = ytick
    nxsub_c = nxsub; nysub_c = nysub

    call giza_box_time_c(cstring(xopt),xtick_c,nxsub_c,cstring(yopt),ytick_c,nysub_c)
  end subroutine giza_intern_box_time_f2c

  subroutine giza_intern_colour_bar_f2c(side,disp,width,valmin,valmax,label)
    implicit none
    character(len=*), intent(in) :: side,label
    real, intent(in) :: disp,width,valmin,valmax
    real(kind=c_double) :: disp_c,width_c,valmin_c,valmax_c
    
    disp_c = disp
    width_c = width
    valmin_c = valmin
    valmax_c = valmax
    
    call giza_colour_bar_c(cstring(side),disp_c,width_c,valmin_c,valmax_c,cstring(label))
    
  end subroutine giza_intern_colour_bar_f2c

  integer function  giza_intern_open_device(dev,prefix)
    implicit none
    character(len=*),intent(in) :: dev,prefix
    
    giza_intern_open_device = giza_open_device_c(cstring(dev),cstring(prefix))
  end function giza_intern_open_device

  subroutine giza_open_sub(dev,prefix,width,height,units,error)
    implicit none
    character(len=*),intent(in), optional  :: dev
    character(len=*), intent(in), optional :: prefix
    real,intent(in), optional     :: width,height
    integer, intent(in), optional :: units
    integer, intent(out), optional :: error
    integer :: giza_open
    
    character(len=40) :: fdev
    if (present(dev)) then
       fdev = dev
    else
       fdev = '?'
    endif
    
    if (present(units) .and. present(width) .and. present(height)) then    
       if (present(prefix)) then
          giza_open = giza_open_device_size_c(cstring(fdev),cstring(prefix),width,height,units)
       else
          giza_open = giza_open_device_size_c(cstring(fdev),cstring('giza'),width,height,units)
       endif
    else
       if (present(prefix)) then
          giza_open = giza_open_device_c(cstring(fdev),cstring(prefix))
       else
          giza_open = giza_open_device_c(cstring(fdev),cstring('giza'))
       endif
    endif
    if (present(error)) then
       error = giza_open
    endif
  
  end subroutine giza_open_sub
  
  subroutine giza_close
     call giza_close_device()
  end subroutine giza_close


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
    
    call giza_text_float_c(x,y,cstring(text))
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
    
    call giza_ptext_float_c(x,y,angle,just,cstring(text))
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
    
    call giza_qtext_float_c(x,y,angle,just,cstring(text),xbox,ybox)
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

!---------------------------------------------------------------------------
!---------------------------------------------------------------------------
!              HIGH LEVEL FORTRAN INTERFACE
!---------------------------------------------------------------------------
!---------------------------------------------------------------------------

subroutine giza_plot(y,x,img,dev,prefix,width,height,units,&
                     xmin,xmax,ymin,ymax,imgmin,imgmax,affine,&
                     vptxmin,vptxmax,vptymin,vptymax, &
                     xlabel,ylabel,title,font,&
                     ls,lw,ci,ch,symbol,just,axis,extend,printid)
 implicit none
 real, intent(in), dimension(:), optional :: y,x
 real, intent(in), dimension(:,:), optional :: img
 integer, intent(in), optional :: units
 real, intent(in), optional :: width,height
 character(len=*), intent(in), optional :: dev,prefix
 real, intent(in), optional :: xmin,xmax,ymin,ymax,imgmin,imgmax
 real, intent(in), optional :: vptxmin,vptxmax,vptymin,vptymax
 character(len=*), intent(in), optional :: xlabel,ylabel,title
 character(len=*), intent(in), optional :: font
 integer, intent(in), optional :: ls,ci,symbol,axis,just,extend
 real, intent(in), optional :: lw,ch
 real, dimension(6), intent(in), optional :: affine
 logical, intent(in), optional :: printid
 integer :: iunits,nx,ny,i,iaxis,ijust
 real(kind=c_double) :: xmini,xmaxi,ymini,ymaxi,valmin,valmax,dx,dy
 real(kind=c_double) :: vptxmini,vptxmaxi,vptymini,vptymaxi
 real(kind=c_double), dimension(6) :: affinei
 character(len=20) :: devi
 character(len=40) :: xlabeli,ylabeli,titlei
 real(kind=c_double), dimension(:), allocatable :: arrtmp
 
 real(kind=c_double)  :: lw_c, ch_c
 integer(kind=c_int)  :: n, nx_c, ny_c
 integer(kind=c_int)  :: symbol_c, iextend

!
!--open giza device
!
 if (present(dev)) then
    devi = dev
 else
    devi = '?'
 endif
 if (present(width) .and. present(height)) then
    if (present(units)) then
       iunits = units
    else
       iunits = giza_units_pixels
    endif
    if (present(prefix)) then
       call giza_open(dev=devi,prefix=prefix,width=width,height=height,units=iunits)
    else
       call giza_open(dev=devi,width=width,height=height,units=iunits)
    endif
 else
    if (present(prefix)) then
       call giza_open(dev=devi,prefix=prefix)    
    else
       call giza_open(dev=devi)
    endif
 endif

!
!--max/min limits for plot window
!
 if (present(xmin)) then
    xmini = xmin
 else
    if (present(x)) then
       xmini = minval(x)
    else
       xmini = 0.
    endif
 endif
 if (present(xmax)) then    
    xmaxi = xmax
 else
    if (present(x)) then
       xmaxi = maxval(x)
    else
       xmaxi = 1.
    endif
 endif
 if (present(ymin)) then
    ymini = ymin
 else
    if (present(y)) then
       ymini = minval(y)
    else
       ymini = 0.
    endif
 endif
 if (present(ymax)) then
    ymaxi = ymax
 else
    if (present(y)) then
       ymaxi = maxval(y)
    else
       ymaxi = 1.
    endif
 endif
 !--plot labels
 if (present(title)) then
    titlei = title
 else
    titlei = ''
 endif
 if (present(xlabel)) then
    xlabeli = xlabel
 else
    xlabeli = ''
 endif
 if (present(ylabel)) then
    ylabeli = ylabel
 else
    ylabeli = ''
 endif
 if (present(font)) then
    call giza_set_font(font)
 endif
 if (present(ch)) then
    ch_c = ch
    call giza_set_character_height(ch_c)
 endif
 if (present(just)) then
    ijust = just
 else
    ijust = 0
 endif
 if (present(axis)) then
    iaxis = axis
 else
    iaxis = 0
 endif

 if (present(vptxmin) .or. present(vptxmax) .or. present(vptymin) .or. present(vptymax)) then
    call giza_set_viewport_default()
    call giza_get_viewport(giza_units_normalized,vptxmini,vptxmaxi,vptymini,vptymaxi)

    if (present(vptxmin)) vptxmini = vptxmin
    if (present(vptxmax)) vptxmaxi = vptxmax
    if (present(vptymin)) vptymini = vptymin
    if (present(vptymax)) vptymaxi = vptymax

    call giza_set_viewport(vptxmini,vptxmaxi,vptymini,vptymaxi)
    if (ijust.eq.1) then
       call giza_set_window(xmini,xmaxi,ymini,ymaxi)
    else
       call giza_set_window_equal_scale(xmini,xmaxi,ymini,ymaxi)
    endif
    call giza_box("BCTSN",0.,0,"BCTSN",0.,0)
    call giza_label(xlabeli,ylabeli,titlei)
 else
    call giza_set_environment(xmini,xmaxi,ymini,ymaxi,ijust,iaxis)
    call giza_label(xlabeli,ylabeli,titlei)
 endif
 if (present(printid)) then
    if (printid) call giza_print_id()
 endif

 if (present(ls)) then
    call giza_set_line_style(ls)
 endif
 if (present(lw)) then
    lw_c = lw
    call giza_set_line_width(lw_c)
 endif
 if (present(ci)) then
    call giza_set_colour_index(ci)
 endif
 
 if (present(x) .and. present(y)) then
    n = min(size(x),size(y))
    if (present(symbol)) then
       symbol_c = symbol
       call giza_points(n,real(x,kind=c_double),real(y,kind=c_double),symbol_c)    
    else
       call giza_line(n,real(x,kind=c_double),real(y,kind=c_double))
    endif
 elseif (present(x) .and. size(x).gt.1) then
    n = size(x)
    allocate(arrtmp(n))
    do i=1,n
       arrtmp(i) = (i-1)/(real(n-1))
    enddo
    if (present(symbol)) then
       symbol_c = symbol
       call giza_points(n,real(x,kind=c_double),arrtmp,symbol_c)
    else
       call giza_line(n,real(x,kind=c_double),arrtmp)
    endif
    deallocate(arrtmp)
 elseif (present(y) .and. size(y).gt.1) then
    n = size(y)
    allocate(arrtmp(n))
    do i=1,n
       arrtmp(i) = (i-1)/(real(n-1))
    enddo
    if (present(symbol)) then
       symbol_c = symbol
       call giza_points(n,arrtmp,real(y,kind=c_double),symbol_c)
    else
       call giza_line(n,arrtmp,real(y,kind=c_double))
    endif
    deallocate(arrtmp)
 endif
 
 if (present(img)) then
    nx = size(img(:,1))
    ny = size(img(1,:))
    if (present(imgmin)) then
       valmin = imgmin
    else
       valmin = minval(img)
    endif
    if (present(imgmax)) then
       valmax = imgmax
    else
       valmax = maxval(img)
    endif
    if (present(affine)) then
       affinei = affine
    else
       dx = real((xmaxi - xmini)/real(nx))
       dy = real((ymaxi - ymini)/real(ny))
       affinei = (/dx,0.d0,0.d0,dy,xmini,ymini/)
    endif
    if (present(extend)) then
       iextend = extend
    else
       iextend = giza_extend_none
    endif
    nx_c = nx; ny_c = ny
    call giza_render(nx_c,ny_c,real(img,kind=c_double),int(1,kind=c_int),&
                     nx_c,int(1,kind=c_int),ny_c,valmin,valmax,iextend,affinei)
 endif
 
 call giza_close()
 
end subroutine giza_plot

end module giza
