/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
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

#include <giza-shared.h>

/* do not allow use of freetype if cairo does not have it */
#ifndef CAIRO_HAS_FT_FONT
#undef GIZA_HAS_FT_FONT
#endif

void giza_arrow (double x1, double y1, double x2, double y2);
void giza_arrow_float (float x1, float y1, float x2, float y2);

void giza_set_arrow_style (int fillStyle, double angle, double cutback);
void giza_set_arrow_style_float (int fillStyle, float angle, float cutback);
void giza_get_arrow_style (int *fillStyle, double *angle, double *cutback);
void giza_get_arrow_style_float (int *fillStyle, float *angle,
				 float *cutback);

void giza_annotate (const char *side, double displacment, double coord,
		    double justification, const char *string);
void giza_annotate_float (const char *side, float displacment, float coord,
			  float justification, const char *string);
void giza_begin_autolog(void);
void giza_end_autolog(void);
int giza_band (int mode, int moveCursm, double xanc, double yanc, double *x,
	       double *y, char *ch);
int giza_band_float (int mode, int moveCurs, float xanc, float yanc, float *x,
               float *y, char *ch);

void giza_get_band_style (int *ls, double *lw);
void giza_set_band_style (int ls, double lw);

void giza_box (const char *xopt, double xtick, int nxsub,
	       const char *yopt, double ytick, int nysub);
void giza_box_float (const char *xopt, float xtick, int nxsub,
		     const char *yopt, float ytick, int nysub);
void giza_box_time (const char *xopt, double xtick, int nxsub,
	       const char *yopt, double ytick, int nysub);
void giza_box_time_float (const char *xopt, float xtick, int nxsub,
		     const char *yopt, float ytick, int nysub);

void giza_begin_buffer (void);
void giza_end_buffer (void);
void giza_flush_buffer (void);
void giza_get_buffering (int *buf);

void giza_circle (double x, double y, double r);
void giza_circle_float (float x, float y, float r);

void giza_set_character_height (double ch);
void giza_set_character_height_float (float ch);
void giza_get_character_height (double *ch);
void giza_get_character_height_float (float *ch);
void giza_get_character_size (int units, double *heightx, double *heighty);
void giza_get_character_size_float (int units, float *xch, float *ych);

void giza_set_clipping (int clip);
void giza_get_clipping (int *clip);

void giza_colour_bar (const char *side, double disp, double width, 
                      double valMin, double valMax, const char *label);
void giza_colour_bar_float (const char *side, float disp, float width, 
                            float valMin, float valMax, const char *label);

void giza_set_colour_index (int ci);
void giza_get_colour_index (int *ci);
void giza_set_colour_index_range (int cimin, int cimax);
void giza_get_colour_index_range (int *cimin, int *cimax);
/*void giza_set_range_as_colour_table (int *cimin, int *cimax)*/

void giza_set_colour_palette(int palette);

void giza_set_colour_representation (int ci, double red, double green,
				     double blue);
void giza_set_colour_representation_float (int ci, float red, float green,
					   float blue);
void giza_set_colour_representation_rgb (int ci, int red, int green, int blue);
void giza_set_colour_representation_rgba (int ci, int red, int green,
				     int blue, double alpha);
void giza_set_colour_representation_rgba_float (int ci, int red, int green,
					   int blue, float alpha);
void giza_set_colour_representation_alpha (int ci, double red, double green,
					   double blue, double alpha);
void giza_set_colour_representation_alpha_float (int ci, float red,
						 float green, float blue,
						 float alpha);
void giza_get_colour_representation (int ci, double *red, double *green,
				     double *blue);
void giza_get_colour_representation_float (int ci, float *red, float *green,
					   float *blue);
void giza_get_colour_representation_alpha (int ci, double *red, double *green,
					   double *blue, double *alpha);
void giza_get_colour_representation_alpha_float (int ci, float *red,
						 float *green, float *blue,
						 float *alpha);
void giza_set_colour_representation_hls (int ci, double hue, double lightness, double saturation);
void giza_set_colour_representation_hls_float (int ci, float hue, float lightness,
                                               float saturation);

int giza_set_colour_table (const double *controlPoints, const double *red, const double *green,
			   const double *blue, int n, double contrast, double brightness);
int giza_set_colour_table_float (const float *controlPoints, const float *red,
				 const float *green, const float *blue, int n, float contrast, float brightness);
int giza_set_colour_table_gray (void);
void giza_save_colour_table (void);
void giza_restore_colour_table (void);
void giza_rgb_from_table (double pos, double *red, double *green,
			  double *blue);
void giza_rgb_from_table_float (float pos, float *red, float *green,
				float *blue);

void giza_contour (int sizex, int sizey, const double* data,
		   int i1, int i2, int j1, int j2, int ncont,
		   const double* cont, const double *affine);
void giza_contour_float (int sizex, int sizey, const float* data, int i1,
	      int i2, int j1, int j2, int ncont, const float* cont, const float *affine);

void giza_get_current_point (double *xpt, double *ypt);
void giza_get_current_point_float (float *xpt, float *ypt);

void giza_print_device_list (void);
int giza_open_device (const char *newDeviceName, const char *newPrefix);
int giza_open_device_size (const char *newDeviceName, const char *newPrefix, double width,
			   double height, int units);
int giza_open_device_size_float (const char *newDeviceName, const char *newPrefix,
				 float width, float height, int units);
void giza_select_device (int devid);
void giza_get_device_id (int *devid);
void giza_flush_device (void);
void giza_change_page (void);
void giza_close_device (void);
int giza_query_device (const char *querytype, char *returnval);
int giza_device_has_cursor (void);
int giza_get_key_press (double *x, double *y, char *ch);
int giza_get_key_press_float (float *x, float *y, char *ch);

void giza_draw (double xpt, double ypt);
void giza_draw_float (float xpt, float ypt);

void giza_draw_background (void);

void giza_set_environment (double xmin, double xmax, double ymin, double ymax,
			   int just, int axis);
void giza_set_environment_float (float xmin, float xmax, float ymin,
				 float ymax, int just, int axis);

void giza_error_bars (int dir, int n, const double *xpts, const double *ypts,
		      const double *error, double term);
void giza_error_bars_float (int dir, int n, const float *xpts, const float *ypts,
			    const float *error, float term);
void giza_error_bars_vert (int n, const double *xpts, const double *ypts, const double *ypts2,
			   double term);
void giza_error_bars_vert_float (int n, const float *xpts, const float *ypts,
				 const float *ypts2, float term);
void giza_error_bars_hori (int n, const double *xpts1, const double *xpts2,
                                  const double *ypts, double term);
void giza_error_bars_hori_float (int n, const float *xpts1, const float *xpts2,
                                  const float *ypts, float term);


void giza_set_fill (int fs);
void giza_get_fill (int *fs);

void giza_format_number (int mantissa, int power, int form, char *str);

void giza_function_x (double (*func) (double *x), int n, double xmin,
		      double xmax, int flag);
void giza_function_x_float (float (*func) (float *x), int n, float xmin,
			    float xmax, int flag);
void giza_function_y (double (*func) (double *y), int n, double ymin,
		      double ymax, int flag);
void giza_function_y_float (float (*func) (float *y), int n, float ymin,
			    float ymax, int flag);
void giza_function_t (double (*funcx) (double *t),
		      double (*funcy) (double *t), int n, double ymin,
		      double ymax, int flag);
void giza_function_t_float (float (*funcx) (float *t),
			    float (*funcy) (float *t), int n, float ymin,
			    float ymax, int flag);

void giza_get_surface_size (double *x1, double *x2, double *y1, double *y2);
void giza_get_surface_size_float (float *x1, float *x2, float *y1, float *y2);

void giza_set_hatching_style (double angle, double spacing, double phase);
void giza_set_hatching_style_float (float angle, float spacing, float phase);
void giza_get_hatching_style (double *angle, double *spacing, double *phase);
void giza_get_hatching_style_float (float *angle, float *spacing, float *phase);

void giza_histogram (int n, const double *dat, double min, double max, int nbin, int flag);
void giza_histogram_float (int n, const float *dat, float min, float max, int nbin, int flag);
void giza_histogram_binned (int n, const double *x, const double *dat, int centre);
void giza_histogram_binned_float (int n, const float *x, const float *dat, int centre);

void giza_label (const char *labelx, const char *labely, const char *title);

void giza_line (int n, const double *xpts, const double *ypts);
void giza_line_float (int n, const float *xpts, const float *ypts);

void giza_set_line_style (int ls);
void giza_get_line_style (int *ls);

void giza_set_line_width (double lw);
void giza_set_line_width_float (float lw);
void giza_get_line_width (double *lw);
void giza_get_line_width_float (float *lw);

void giza_set_line_cap (int lc);
void giza_get_line_cap (int *lc);

void giza_mark_line (int maxpts, int *npts, double* xpts, double* ypts);
void giza_mark_line_float (int maxpts, int *npts, float* xpts, float* ypts);
void giza_mark_line_ordered (int maxpts, int *npts, double* xpts, double* ypts);
void giza_mark_line_ordered_float (int maxpts, int *npts, float* xpts, float* ypts);
void giza_mark_points (int maxpts, int *npts, double* xpts,
                       double* ypts, int symbol);
void giza_mark_points_float (int maxpts, int *npts, float* xpts,
                             float* ypts, int symbol);
void giza_mark_points_ordered (int maxpts, int *npts, double* xpts,
                               double* ypts, int symbol);
void giza_mark_points_ordered_float (int maxpts, int *npts, float* xpts,
                                     float* ypts, int symbol);

void giza_move (double xpt, double ypt);
void giza_move_float (float xpt, float ypt);

void giza_set_paper_size (int units, double width, double height);
void giza_set_paper_size_float (int units, float width, float height);
void giza_get_paper_size (int units, double *width, double *height);
void giza_get_paper_size_float (int units, float *width, float *height);

void giza_points (int n, const double* x, const double* y, int symbol);
void giza_points_float (int n, const float* x, const float* y, int symbol);
void giza_single_point (double x, double y, int symbol);
void giza_single_point_float (float x, float y, int symbol);

void giza_polygon (int n, const double *xpts, const double *ypts);
void giza_polygon_float (int n, const float *xpts, const float *ypts);
void giza_print_id (void);

void giza_start_prompting (void);
void giza_stop_prompting (void);

void giza_text (double x, double y, const char *text);
void giza_text_float (float x, float y, const char *text);
void giza_ptext (double x, double y, double angle, double just, const char *text);
void giza_ptext_float (float x, float y, float angle, float just, const char *text);

void giza_qtext (double x, double y, double angle, double just, const char *text,
		 double xbox[4], double ybox[4]);
void giza_qtext_float (float x, float y, float angle, float just, const char *text,
		       float xbox[4], float ybox[4]);
void giza_qtextlen (int units, const char *text, double *xlen, double *ylen);
void giza_qtextlen_float (int units, const char *text, float *xlen, float *ylen);

void giza_rectangle (double x1, double x2, double y1, double y2);
void giza_rectangle_float (float x1, float x2, float y1, float y2);

void giza_rectangle_rounded (double x1, double x2, double y1, double y2, double radius);
void giza_rectangle_rounded_float (float x1, float x2, float y1, float y2, float radius);

void giza_render (int sizex, int sizey, const double* data, int i1,
		  int i2, int j1, int j2, double valMin, double valMax,
		  int extend, const double *affine);
void giza_render_float (int sizex, int sizey, const float* data,
			int i1, int i2, int j1, int j2, float valMin,
			float valMax, int extend, const float *affine);

void giza_render_transparent (int sizex, int sizey, const double* data, int i1,
		  int i2, int j1, int j2, double valMin, double valMax,
		  int extend, const double *affine);
void giza_render_transparent_float (int sizex, int sizey, const float* data,
			int i1, int i2, int j1, int j2, float valMin,
			float valMax, int extend, const float *affine);

void giza_render_alpha (int sizex, int sizey, const double* data, const double* alpha,
                  int i1, int i2, int j1, int j2, double valMin, double valMax,
                  int extend, const double *affine);
void giza_render_alpha_float (int sizex, int sizey, const float* data, const float* alpha,
                  int i1, int i2, int j1, int j2, float valMin, float valMax,
                  int extend, const float *affine);

void giza_render_gray (int sizex, int sizey, const double* data, int i1,
		  int i2, int j1, int j2, double valMin, double valMax, int extend,
		  const double *affine);
void giza_render_gray_float (int sizex, int sizey, const float* data,
			int i1, int i2, int j1, int j2, float valMin,
			float valMax, int extend, const float *affine);
void giza_draw_pixels (int sizex, int sizey, const int* idata, int i1, int i2,
	    int j1, int j2, double xmin, double xmax, double ymin, double ymax, int extend);
void giza_draw_pixels_float (int sizex, int sizey, const int* idata, int i1, int i2,
	    int j1, int j2, float xmin, float xmax, float ymin, float ymax, int extend);

double giza_round (double x, int *nsub);
float giza_round_float (float x, int *nsub);

void giza_subpanel (int nx, int ny);
void giza_set_panel (int ix, int iy);
void giza_get_panel (int *ix, int *iy);

void giza_set_font (char *font);
void giza_set_font_bold (char *font);
void giza_set_font_bold_italic (char *font);
void giza_set_font_italic (char *font);
void giza_get_font (char *font, int n);

void giza_set_text_background (int colourIndex);
void giza_get_text_background (int *colourIndex);

void
giza_vector (int n, int m, const double* horizontal, const double* vertical,
	     int i1, int i2, int j1, int j2, double scale, int position,
	     const double* affine, double blank);
void
giza_vector_float (int n, int m, const float* horizontal, const float* vertical,
		   int i1, int i2, int j1, int j2, float scale, int position,
		   const float* affine, float blank);

void giza_set_viewport (double xleft, double xright, double ybottom,
			double ytop);
void giza_set_viewport_float (float xleft, float xright, float ybottom,
			      float ytop);
void giza_set_viewport_default (void);
void giza_get_viewport (int units, double *x1, double *x2, double *y1,
			double *y2);
void giza_get_viewport_float (int units, float *x1, float *x2, float *y1,
			      float *y2);
void giza_set_viewport_inches (double xleftin, double xrightin,
                               double ybottomin, double ytopin);
void giza_set_viewport_inches_float (float xleftin, float xrightin,
                                     float ybottomin, float ytopin);
void giza_version (int *major, int *minor, int *micro);

void giza_start_warnings (void);
void giza_stop_warnings (void);

void giza_save (void);
void giza_restore (void);

void giza_set_window (double x1, double x2, double y1, double y2);
void giza_set_window_float (float x1, float x2, float y1, float y2);
void giza_set_window_equal_scale (double x1, double x2, double y1, double y2);
void giza_set_window_equal_scale_float (float x1, float x2, float y1,
					float y2);
void giza_get_window (double *x1, double *x2, double *y1, double *y2);
void giza_get_window_float (float *x1, float *x2, float *y1, float *y2);
