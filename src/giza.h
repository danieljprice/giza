/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include <giza-shared-cpp.h>

void giza_arrow (double x1, double y1, double x2, double y2);
void giza_arrow_float (float x1, float y1, float x2, float y2);

void giza_set_arrow_style (int fillStyle, double angle, double cutback);
void giza_set_arrow_style_float (int fillStyle, float angle, float cutback);
void giza_get_arrow_style (int *fillStyle, double *angle, double *cutback);
void giza_get_arrow_style_float (int *fillStyle, float *angle, float *cutback);

void giza_annotate (char *side, double displacment, double coord,
		    double justification, char *string);
void giza_annotate_float (char *side, float displacment, float coord,
			  float justification, char *string);

int giza_band (int mode, int moveCursm, double xanc, double yanc, double *x,
	       double *y, char *ch);

void giza_set_band_style (int ls, double lw);

void giza_box (const char *xopt, double xtick, int nxsub,
	       const char *yopt, double ytick, int nysub);
void giza_box_float (const char *xopt, float xtick, int nxsub,
		     const char *yopt, float ytick, int nysub);

void giza_begin_buffer ();
void giza_end_buffer ();

void giza_circle (double x, double y, double r);
void giza_circle_float (float x, float y, float r);

void giza_set_character_height (double ch);
void giza_set_character_height_float (float ch);
void giza_get_character_height (double *ch);
void giza_get_character_height_float (float *ch);
void giza_get_character_size (int units, double *heightx, double *heighty);
void giza_get_character_size_float (int units, float *xch, float *ych);

void giza_set_colour_index (int ci);
void giza_get_colour_index (int *ci);
void giza_set_colour_representation (int ci, double red, double green,
				     double blue);
void giza_set_colour_representation_float (int ci, float red, float green,
					   float blue);
void giza_get_colour_representation (int ci, double *red, double *green,
				     double *blue);
void giza_get_colour_representation_float (int ci, float *red, float *green,
					   float *blue);

int giza_set_colour_table (double *controlPoints, double *red, double *green,
			   double *blue, int n);
int giza_set_colour_table_float (float *controlPoints, float *red,
				 float *green, float *blue, int n);
void giza_rgb_from_table (double pos, double *red, double *green,
			  double *blue);
void giza_rgb_from_table_float (double pos, float *red, float *green,
				float *blue);

void giza_contour (int sizex, int sizey, const double data[sizey][sizex],
		   int i1, int i2, int j1, int j2, int ncont,
		   const double cont[ncont], const double *affine);

void giza_print_device_list ();
int giza_open_device (char *newDeviceName, char *newPrefix);
int giza_open_device_size (char *newDeviceName, char *newPrefix, double width,
			   double height);
int giza_open_device_size_float (char *newDeviceName, char *newPrefix,
				 float width, float height);
void giza_flush_device ();
void giza_change_page ();
void giza_close_device ();
int giza_get_key_press (double *x, double *y, char *ch);
int giza_get_key_press_float (float *x, float *y, char *ch);

void giza_draw (double xpt, double ypt);
void giza_draw_float (float xpt, float ypt);

void giza_set_environment (double xmin, double xmax, double ymin, double ymax, int just, int axis);
void giza_set_environment_float (float xmin, float xmax, float ymin, float ymax, int just, int axis);

void giza_error_bars (int dir, int n, double *xpts, double *ypts, double *error, double term);
void giza_error_bars_float (int dir, int n, float *xpts, float *ypts, float *error, float term);
void giza_error_bars_vert (int n, double *xpts, double *ypts, double *ypts2, double term);
void giza_error_bars_vert_float (int n, float *xpts, float *ypts, float *ypts2, float term);

void giza_set_fill (int fs);
void giza_get_fill (int *fs);

void giza_format_number (double x, int n, char *str);

void giza_function_x (double (*func)(double *x), int n, double xmin, double xmax, int flag);
void giza_function_x_float (float (*func)(float *x), int n, float xmin, float xmax, int flag);
void giza_function_y (double (*func)(double *y), int n, double ymin, double ymax, int flag);
void giza_function_y_float (float (*func)(float *y), int n, float ymin, float ymax, int flag);
void giza_function_t (double (*funcx)(double *t),double (*funcy)(double *t), int n, double ymin, double ymax, int flag);
void giza_function_t_float (float (*funcx)(float *t),float (*funcy)(float *t), int n, float ymin, float ymax, int flag);

void giza_get_surface_size (double *x1, double *x2, double *y1, double *y2);
void giza_get_surface_size_float (float *x1, float *x2, float *y1, float *y2);

int giza_device_has_cursor ();

void giza_label (char *labelx, char *labely, char *title);

void giza_line (int n, double *xpts, double *ypts);
void giza_line_float (int n, float *xpts, float *ypts);

void giza_set_line_style (int ls);
void giza_get_line_style (int *ls);

void giza_set_line_width (double lw);
void giza_set_line_width_float (float lw);
void giza_get_line_width (double *lw);
void giza_get_line_width_float (float *lw);

void giza_set_line_cap (int lc);
void giza_get_line_cap (int *lc);

void giza_move (double xpt, double ypt);
void giza_move_float (float xpt, float ypt);

void giza_points (int n, double x[n], double y[n], int symbol);
void giza_points_float (int n, float x[n], float y[n], int symbol);
void giza_single_point (double x, double y, int symbol);
void giza_single_point_float (float x, float y, int symbol);

void giza_polygon (int n, double *xpts, double *ypts);
void giza_polygon_float (int n, float *xpts, float *ypts);

void giza_start_prompting ();
void giza_stop_prompting ();

void giza_text (double x, double y, char *text);
void giza_text_float (float x, float y, char *text);
void giza_ptext (double x, double y, double angle, double just, char *text);
void giza_ptext_float (float x, float y, float angle, float just, char *text);

void giza_qtext (double x, double y, double angle, double just, char *text,
		 double xbox[4], double ybox[4]);
void giza_qtext_float (float x, float y, float angle, float just, char *text,
		       float xbox[4], float ybox[4]);

void giza_rectangle (double x1, double x2, double y1, double y2);
void giza_rectangle_float (float x1, float x2, float y1, float y2);

void giza_render (int sizex, int sizey, double data[sizey][sizex], int i1,
		  int i2, int j1, int j2, double valMin, double valMax,
		  double *affine);
void giza_render_float (int sizex, int sizey, float data[sizey][sizex],
			int i1, int i2, int j1, int j2, float valMin,
			float valMax, float *affine);

void giza_set_font (char *font);
void giza_get_font (char *font, int n);

void giza_set_text_background (int colourIndex);

void
giza_vector (int n, int m, double horizontal[m][n], double vertical[m][n],
	     int i1, int i2, int j1, int j2, double scale, int position,
	     double affine[6], double blank);
void
giza_vector_float (int n, int m, float horizontal[m][n], float vertical[m][n],
		   int i1, int i2, int j1, int j2, float scale, int position,
		   float affine[6], float blank);


void giza_set_viewport (double xleft, double xright, double ybottom,
			double ytop);
void giza_set_viewport_float (float xleft, float xright, float ybottom,
			float ytop);
void giza_set_viewport_default ();
void giza_get_viewport (int units, double *x1, double *x2, double *y1, double *y2);
void giza_get_viewport_float (int units, float *x1, float *x2, float *y1, float *y2);

void giza_start_warnings ();
void giza_stop_warnings ();

void giza_save ();
void giza_restore ();

void giza_set_window (double x1, double x2, double y1, double y2);
void giza_set_window_float (float x1, float x2, float y1, float y2);
void giza_set_window_equal_scale (double x1, double x2, double y1, double y2);
void giza_set_window_equal_scale_float (float x1, float x2, float y1,
					float y2);
void giza_get_window (double *x1, double *x2, double *y1, double *y2);
void giza_get_window_float (float *x1, float *x2, float *y1, float *y2);
