!-----------------------------------------------------------------------
!  giza - Lick-Mongo Fortran plotting interface (libmgo)
!
!  Drop-in replacements for libmgo routines used by Kepler
!  (e.g. plotmongo.f90) and a subset of the MONGO() batch driver.
!  Mirrors the libpgplot / giza-pgplot.f90 packaging.
!-----------------------------------------------------------------------

module gizamongo
 use giza
 implicit none

 integer, parameter :: dp = kind(0.d0)
 integer, parameter :: sp = kind(0.e0)
 real(dp), parameter :: mongo_pi = 3.141592653589793238462643383279502884197d0
 integer, parameter :: maxpol = 64
 integer, parameter :: maxcmd = 256
 integer, parameter :: point_open = 0
 integer, parameter :: point_skeletal = 1
 integer, parameter :: point_stellated = 2
 integer, parameter :: point_filled = 3

 logical :: device_open = .false.
 logical :: xlog = .false.
 logical :: ylog = .false.

 integer :: mongo_device = 11
 integer :: mongo_nlines = 0
 integer :: mongo_line_start = 1
 integer :: mongo_line_end = 0
 integer :: mongo_xcol = 1
 integer :: mongo_ycol = 2
 integer :: mongo_colour = 1
 integer :: mongo_lweight = 1
integer :: mongo_ltype = 1

 integer :: panel_nx = 1
 integer :: panel_ny = 1

 real(dp) :: wxmin = 0.d0
 real(dp) :: wymin = 0.d0
 real(dp) :: wxmax = 1.d0
 real(dp) :: wymax = 1.d0

 ! MONGOPAR-like geometry (mongo-stuff/mongo/source/plotsub/MONGOPAR.F)
 real(dp) :: gx1 = 0.d0
 real(dp) :: gx2 = 32767.d0
 real(dp) :: gy1 = 0.d0
 real(dp) :: gy2 = 32767.d0
 real(dp) :: lx1 = 0.d0
 real(dp) :: lx2 = 32767.d0
 real(dp) :: ly1 = 0.d0
 real(dp) :: ly2 = 32767.d0
 real(dp) :: xp = 0.d0
 real(dp) :: yp = 0.d0
 real(dp) :: asmallx = 0.d0
 real(dp) :: abigx = 0.d0
 real(dp) :: asmally = 0.d0
 real(dp) :: abigy = 0.d0
 real(dp) :: cheight = 14.d0
 real(dp) :: cwidth = 10.d0
 real(dp) :: xpin = 72.d0
 real(dp) :: ypin = 72.d0
 real(dp) :: submarg_x = 1.d0
 real(dp) :: submarg_y = 1.d0
 real(dp) :: old_gx1 = 0.d0
 real(dp) :: old_gx2 = 32767.d0
 real(dp) :: old_gy1 = 0.d0
 real(dp) :: old_gy2 = 32767.d0
 real(dp) :: old_expand = 1.d0
 logical :: ps_landscape = .false.
 integer :: psfmode = 1
 logical :: physical_init = .false.
 logical :: mongo_custom_viewport = .false.
 logical :: mongo_movie_mode = .false.

 real(dp) :: expand = 1.d0
 real(dp) :: mongo_angle = 0.d0
 real(dp) :: pdef = 5.d0
 real(dp) :: xlexp = 1.d-4
 real(dp) :: xhexp = 1.d5
 real(dp) :: ylexp = 1.d-4
 real(dp) :: yhexp = 1.d5
 real(dp) :: tick_xmaj = 0.d0
 real(dp) :: tick_xmin = 0.d0
 real(dp) :: tick_ymaj = 0.d0
 real(dp) :: tick_ymin = 0.d0

 character(len=256) :: label_x = ' '
 character(len=256) :: label_y = ' '
 character(len=256) :: label_r = ' '
 character(len=256) :: label_t = ' '
 character(len=256) :: mongo_window_title = ' '

 integer :: npol = 0
 integer :: pol_current_color = 1
 real(dp), dimension(maxpol) :: pol_x = 0.d0
 real(dp), dimension(maxpol) :: pol_y = 0.d0
 integer, dimension(maxpol) :: pol_c = 1

 real(dp) :: pen_x = 0.d0
 real(dp) :: pen_y = 0.d0

contains

 subroutine mongo_reset_state()
  xlog = .false.
  ylog = .false.
  mongo_nlines = 0
  mongo_line_start = 1
  mongo_line_end = 0
  mongo_xcol = 1
  mongo_ycol = 2
  mongo_colour = 1
  mongo_lweight = 1
  mongo_ltype = 1
  xlexp = 1.d-4
  xhexp = 1.d5
  ylexp = 1.d-4
  yhexp = 1.d5
  asmallx = 0.d0
  abigx = 0.d0
  asmally = 0.d0
  abigy = 0.d0
  tick_xmaj = 0.d0
  tick_xmin = 0.d0
  tick_ymaj = 0.d0
  tick_ymin = 0.d0
  label_x = ' '
  label_y = ' '
  label_r = ' '
  label_t = ' '
  psfmode = 1
  mongo_window_title = ' '
  mongo_angle = 0.d0
  mongo_movie_mode = .false.
 end subroutine mongo_reset_state

 subroutine mongo_parse_x11_geometry(loc, width, height)
  character(len=*), intent(in) :: loc
  integer, intent(out) :: width, height
  character(len=256) :: geom
  integer :: ios, ix, ip, iw, ih

  width = 0
  height = 0
  geom = adjustl(loc)
  ix = index(geom, 'x')
  if (ix <= 1) return
  read (geom(1:ix-1), *, iostat=ios) iw
  if (ios /= 0) return
  ip = index(geom(ix+1:), '+')
  if (ip <= 1) return
  read (geom(ix+1:ix+ip-1), *, iostat=ios) ih
  if (ios /= 0) return
  width = iw
  height = ih
 end subroutine mongo_parse_x11_geometry

 subroutine mongo_world_to_dev(wx, wy, xd, yd)
  real(dp), intent(in) :: wx, wy
  real(dp), intent(out) :: xd, yd
  if (gx2 > gx1) then
     xd = gx1 + (wx - wxmin) * (gx2 - gx1) / (wxmax - wxmin)
  else
     xd = wx
  end if
  if (gy2 > gy1) then
     yd = gy1 + (wy - wymin) * (gy2 - gy1) / (wymax - wymin)
  else
     yd = wy
  end if
 end subroutine mongo_world_to_dev

 subroutine mongo_dev_to_world(xd, yd, wx, wy)
  real(dp), intent(in) :: xd, yd
  real(dp), intent(out) :: wx, wy
  if (gx2 > gx1) then
     wx = wxmin + (xd - gx1) * (wxmax - wxmin) / (gx2 - gx1)
  else
     wx = xd
  end if
  if (gy2 > gy1) then
     wy = wymin + (yd - gy1) * (wymax - wymin) / (gy2 - gy1)
  else
     wy = yd
  end if
 end subroutine mongo_dev_to_world

 subroutine mongo_sync_giza_default_viewport()
  ! Match Mongo device plot area (gx, gy) to giza's PGPLOT-style default viewport.
  real(dp) :: vp1, vp2, vp3, vp4, denomx, denomy
  call giza_set_viewport_default()
  call giza_get_viewport(giza_units_normalized, vp1, vp2, vp3, vp4)
  denomx = max(lx2 - lx1 + 1.d0, 1.d0)
  denomy = max(ly2 - ly1 + 1.d0, 1.d0)
  gx1 = lx1 + vp1 * denomx
  gx2 = lx1 + vp2 * denomx
  gy1 = ly1 + vp3 * denomy
  gy2 = ly1 + vp4 * denomy
  old_gx1 = gx1
  old_gx2 = gx2
  old_gy1 = gy1
  old_gy2 = gy2
 end subroutine mongo_sync_giza_default_viewport

 subroutine mongo_init_physical()
  real :: px1, px2, py1, py2
  if (physical_init) return
  call giza_get_surface_size(px1, px2, py1, py2)
  lx1 = 0.d0
  ly1 = 0.d0
  lx2 = max(1.d0, real(px2 - px1, kind=dp))
  ly2 = max(1.d0, real(py2 - py1, kind=dp))
  mongo_custom_viewport = .false.
  call mongo_sync_giza_default_viewport()
  old_expand = expand
  physical_init = .true.
 end subroutine mongo_init_physical

 subroutine mongo_apply_viewport()
  real(dp) :: vp1, vp2, vp3, vp4, denomx, denomy
  if (.not. physical_init) call mongo_init_physical
  if (.not. mongo_custom_viewport) then
     call mongo_sync_giza_default_viewport()
     return
  end if
  denomx = max(lx2 - lx1 + 1.d0, 1.d0)
  denomy = max(ly2 - ly1 + 1.d0, 1.d0)
  if (gx2 <= gx1 .or. gy2 <= gy1) return
  vp1 = max(0.d0, min(1.d0, (gx1 - lx1) / denomx))
  vp2 = max(vp1 + 0.01d0, min(1.d0, (gx2 - lx1) / denomx))
  vp3 = max(0.d0, min(1.d0, (gy1 - ly1) / denomy))
  vp4 = max(vp3 + 0.01d0, min(1.d0, (gy2 - ly1) / denomy))
  call giza_set_viewport(vp1, vp2, vp3, vp4)
 end subroutine mongo_apply_viewport

 subroutine mongo_warn(msg)
  character(len=*), intent(in) :: msg
  write (*, '(a)') 'giza-mongo: '//trim(msg)
 end subroutine mongo_warn

 subroutine mongo_ensure_device()
  character(len=16) :: devstr
  character(len=256) :: prefix
  if (device_open) return
  if (len_trim(mongo_window_title) > 0) then
     prefix = trim(mongo_window_title)
  else
     prefix = 'giza'
  end if
  select case (mongo_device)
  case (11)
     devstr = '/xw'
  case (2)
     devstr = '/ps'
  case (5)
     devstr = '/png'
  case default
     devstr = '/png'
  end select
  call giza_open(dev=trim(devstr), prefix=trim(prefix))
  device_open = .true.
  call mongo_init_physical
  call mongo_apply_viewport
 end subroutine mongo_ensure_device

 subroutine mongo_apply_window()
  call mongo_apply_viewport
  call giza_set_window(wxmin, wxmax, wymin, wymax)
 end subroutine mongo_apply_window

 subroutine mongo_build_frame_opts(ixlab, iylab, ixnum, iynum, xopt, yopt)
  integer, intent(in) :: ixlab, iylab, ixnum, iynum
  character(len=16), intent(out) :: xopt, yopt
  ! Frame and ticks only (no numeric labels).  giza_box N/M apply to all
  ! numbers on an axis, so numeric labels are drawn in separate passes.
  xopt = ''
  yopt = ''
  if (ixlab >= 0) xopt = trim(xopt)//'B'
  if (ixnum >= 0) xopt = trim(xopt)//'C'
  if (iylab >= 0) yopt = trim(yopt)//'B'
  if (iynum >= 0) yopt = trim(yopt)//'C'
  if (len_trim(xopt) > 0) xopt = trim(xopt)//'TS'
  if (len_trim(yopt) > 0) yopt = trim(yopt)//'TS'
  if (xlog) xopt = trim(xopt)//'L'
  if (ylog) yopt = trim(yopt)//'L'
  if (len_trim(xopt) == 0) xopt = ' '
  if (len_trim(yopt) == 0) yopt = ' '
 end subroutine mongo_build_frame_opts

 subroutine mongo_draw_box(ixlab, iylab, ixnum, iynum)
  integer, intent(in) :: ixlab, iylab, ixnum, iynum
  character(len=16) :: xopt, yopt
  real(dp) :: xmaj, ymaj, saved_lw
  logical :: split_x, split_y
  integer :: saved_ci
  call mongo_ensure_device
  call mongo_apply_window
  xmaj = tick_xmaj
  ymaj = tick_ymaj
  if (abigx /= 0.d0) xmaj = abigx
  if (abigy /= 0.d0) ymaj = abigy
  if (asmallx < 0.d0) xlog = .true.
  if (asmally < 0.d0) ylog = .true.
  split_x = (ixlab >= 0 .and. ixnum >= 0)
  split_y = (iylab >= 0 .and. iynum >= 0)
  saved_ci = mongo_colour
  call giza_set_colour_index(1)
  call giza_get_line_width(saved_lw)
  call giza_set_line_width(1.d0)
  call mongo_build_frame_opts(ixlab, iylab, ixnum, iynum, xopt, yopt)
  if (.not. split_x) then
     if (ixlab >= 0) xopt = trim(xopt)//'N'
     if (ixnum >= 0) xopt = trim(xopt)//'M'
  end if
  if (.not. split_y) then
     if (iylab >= 0) yopt = trim(yopt)//'N'
     if (iynum >= 0) yopt = trim(yopt)//'M'
  end if
  call giza_box(trim(xopt), real(xmaj), 0, trim(yopt), real(ymaj), 0)
  if (split_x) then
     if (ixlab >= 0) then
        xopt = 'N'
        if (xlog) xopt = trim(xopt)//'L'
        call giza_box(trim(xopt), real(xmaj), 0, ' ', 0., 0)
     end if
     if (ixnum >= 0) then
        xopt = 'M'
        if (xlog) xopt = trim(xopt)//'L'
        call giza_box(trim(xopt), real(xmaj), 0, ' ', 0., 0)
     end if
  end if
  if (split_y) then
     if (iylab >= 0) then
        yopt = 'N'
        if (ylog) yopt = trim(yopt)//'L'
        call giza_box(' ', 0., 0, trim(yopt), real(ymaj), 0)
     end if
     if (iynum >= 0) then
        yopt = 'M'
        if (ylog) yopt = trim(yopt)//'L'
        call giza_box(' ', 0., 0, trim(yopt), real(ymaj), 0)
     end if
  end if
  if (len_trim(label_x) > 0 .or. len_trim(label_y) > 0 .or. len_trim(label_t) > 0) then
     call giza_label(trim(label_x), trim(label_y), trim(label_t))
  end if
  if (len_trim(label_r) > 0) then
     call giza_ptext(1.02d0, 0.5d0, 90.d0, 0.d0, trim(label_r))
  end if
  call giza_set_line_width(saved_lw)
  call giza_set_colour_index(saved_ci)
  mongo_colour = saved_ci
 end subroutine mongo_draw_box

 subroutine mongo_parse_limits(cmd, xmin, xmax)
  character(len=*), intent(in) :: cmd
  real(dp), intent(out) :: xmin, xmax
  integer :: ios, i
  character(len=256) :: word
  i = index(cmd, ' ')
  if (i <= 0) return
  word = adjustl(cmd(i+1:))
  read (word, *, iostat=ios) xmin, xmax
  if (ios /= 0) return
 end subroutine mongo_parse_limits

subroutine mongo_auto_limits(col, npts, ncol, data, vmin, vmax)
 integer, intent(in) :: col, npts, ncol
 real(dp), intent(in) :: data(npts, ncol)
 real(dp), intent(out) :: vmin, vmax
 integer :: i, ilow, ihigh
 ilow = 1
 ihigh = npts
 if (mongo_line_end >= mongo_line_start .and. mongo_line_start > 0) then
    ilow = max(1, mongo_line_start)
    ihigh = min(npts, mongo_line_end)
 else if (mongo_nlines > 0) then
    ihigh = min(npts, mongo_nlines)
 end if
 if (col < 1 .or. col > ncol .or. ihigh < ilow) then
    vmin = 0.d0
    vmax = 1.d0
    return
 end if
 vmin = data(ilow, col)
 vmax = data(ilow, col)
 do i = ilow + 1, ihigh
    vmin = min(vmin, data(i, col))
    vmax = max(vmax, data(i, col))
 end do
 if (vmin == vmax) then
    vmin = vmin - 0.5d0
    vmax = vmax + 0.5d0
 end if
end subroutine mongo_auto_limits

 subroutine mongo_exec_command(cmd, npts, ncol, data)
  character(len=*), intent(in) :: cmd
  integer, intent(in) :: npts, ncol
  real(dp), intent(in) :: data(npts, ncol)
  character(len=256) :: line, verb
  integer :: ios, i, n, ix, iy, inx, iny
  integer :: i1, i2, ilow, ihigh
  character(len=16) :: xopt, yopt

  line = adjustl(cmd)
  if (len_trim(line) == 0) return
  i = index(line, ' ')
  if (i > 0) then
     verb = line(1:i-1)
  else
     verb = trim(line)
  end if

  select case (trim(verb))
  case ('terminal')
     read (line(i+1:), *, iostat=ios) mongo_device
     if (ios == 0) call mongo_ensure_device
  case ('reset')
     call mongo_reset_state()
     call window(1, 1, 1)
     call setlweight(1)
     call setcolor(1)
     if (device_open) then
        mongo_custom_viewport = .false.
        call mongo_sync_giza_default_viewport()
     end if
  case ('delete')
     ! no-op
  case ('data')
     ! no-op
  case ('lines')
     read (line(i+1:), *, iostat=ios) i1, i2
     if (ios == 0) then
        mongo_line_start = i1
        mongo_line_end = i2
        mongo_nlines = max(0, i2 - i1 + 1)
     else
        read (line(i+1:), *, iostat=ios) mongo_nlines
        if (ios /= 0) mongo_nlines = npts
        mongo_line_start = 1
        mongo_line_end = mongo_nlines
     end if
  case ('xcolumn')
     read (line(i+1:), *, iostat=ios) mongo_xcol
  case ('ycolumn')
     read (line(i+1:), *, iostat=ios) mongo_ycol
  case ('xlimits')
     call mongo_parse_limits(line, wxmin, wxmax)
     if (index(line, ' ') <= 0) call mongo_auto_limits(mongo_xcol, npts, ncol, data, wxmin, wxmax)
  case ('ylimits')
     call mongo_parse_limits(line, wymin, wymax)
     if (index(line, ' ') <= 0) call mongo_auto_limits(mongo_ycol, npts, ncol, data, wymin, wymax)
  case ('ylogarithm')
     ylog = .true.
  case ('xlogarithm')
     xlog = .true.
  case ('ticksize')
     read (line(i+1:), *, iostat=ios) asmallx, abigx, asmally, abigy
     if (ios == 0) then
        tick_xmaj = asmallx
        tick_xmin = abigx
        tick_ymaj = asmally
        tick_ymin = abigy
        ylog = asmally < 0.d0
        xlog = asmallx < 0.d0
     end if
  case ('erase')
     call mongo_ensure_device
     call giza_draw_background
  case ('box')
     read (line(i+1:), *, iostat=ios) ix, iy, inx, iny
     if (ios == 0) call mongo_draw_box(ix, iy, inx, iny)
  case ('color')
     read (line(i+1:), *, iostat=ios) mongo_colour
     if (ios == 0) call giza_set_colour_index(mongo_colour)
  case ('lweight')
     read (line(i+1:), *, iostat=ios) mongo_lweight
     if (ios == 0) call giza_set_line_width(real(mongo_lweight, kind=dp))
  case ('connect')
     ilow = 1
     ihigh = npts
     if (mongo_line_end >= mongo_line_start .and. mongo_line_start > 0) then
        ilow = max(1, mongo_line_start)
        ihigh = min(npts, mongo_line_end)
     else if (mongo_nlines > 0) then
        ihigh = min(npts, mongo_nlines)
     end if
     n = ihigh - ilow + 1
     if (n < 2) return
     call mongo_ensure_device
     call mongo_apply_window
     call giza_set_colour_index(mongo_colour)
     call giza_set_line_width(real(mongo_lweight, kind=dp))
     call giza_line(n, data(ilow:ihigh, mongo_xcol), data(ilow:ihigh, mongo_ycol))
  case ('xlabel')
     label_x = adjustl(line(i+1:))
  case ('ylabel')
     label_y = adjustl(line(i+1:))
  case ('rlabel')
     label_r = adjustl(line(i+1:))
  case ('tlabel')
     label_t = adjustl(line(i+1:))
  case ('hard')
     call giza_flush_device
     call giza_change_page
  case ('play')
     call giza_flush_device
  case ('end')
     if (device_open) call giza_close
     device_open = .false.
  case ('psland', 'psport')
     if (trim(verb) == 'psland') ps_landscape = .true.
     if (trim(verb) == 'psport') ps_landscape = .false.
  case ('location')
     ! retained for batch compatibility; geometry is handled by setloc/window.
  case default
     call mongo_warn('unimplemented mongo command: '//trim(verb))
  end select
 end subroutine mongo_exec_command

end module gizamongo

!-----------------------------------------------------------------------
! Kepler libmgo-compatible external subroutines
!-----------------------------------------------------------------------

! Status: IMPLEMENTED
! Routine: device
subroutine device(idev)
 use gizamongo
 implicit none
 integer, intent(in) :: idev
 mongo_device = idev
 call mongo_ensure_device
end subroutine device

! Status: IMPLEMENTED
! Routine: tsetup
subroutine tsetup
 use gizamongo
 implicit none
 call mongo_ensure_device
 call mongo_apply_viewport
end subroutine tsetup

! Status: IMPLEMENTED
! Routine: window
subroutine window(nx, ny, ipanel)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: nx, ny, ipanel
 real(dp) :: deltax, deltay, lm, rm, bm, tm
 integer :: indx, indy, px, py
 call mongo_ensure_device
 if (nx <= 0 .or. ny <= 0 .or. ipanel <= 0) return
 panel_nx = max(nx, 1)
 panel_ny = max(ny, 1)
 if (panel_nx == 1 .and. panel_ny == 1) then
    gx1 = old_gx1
    gx2 = old_gx2
    gy1 = old_gy1
    gy2 = old_gy2
    expand = old_expand
    call mongo_apply_viewport
    call giza_subpanel(1, 1)
    call giza_set_panel(1, 1)
    return
 end if
 if (ipanel == 1 .and. (panel_nx > 1 .or. panel_ny > 1)) then
    old_gx1 = gx1
    old_gx2 = gx2
    old_gy1 = gy1
    old_gy2 = gy2
    old_expand = expand
 end if
 indx = mod(ipanel - 1, panel_nx)
 indy = (ipanel - 1) / panel_nx
 deltax = (old_gx2 - old_gx1) / real(panel_nx, kind=dp)
 deltay = (old_gy2 - old_gy1) / real(panel_ny, kind=dp)
 lm = submarg_x * expand * (6.d0 * cwidth + 2.d0 * cheight)
 rm = submarg_x * expand * (0.d0 * cwidth + 1.d0 * cheight)
 bm = submarg_y * expand * (0.d0 * cwidth + 3.d0 * cheight)
 tm = submarg_y * expand * (0.d0 * cwidth + 1.d0 * cheight)
 gx1 = old_gx1 + indx * deltax + lm
 gx2 = old_gx1 + (indx + 1) * deltax - rm
 gy1 = old_gy1 + indy * deltay + bm
 gy2 = old_gy1 + (indy + 1) * deltay - tm
 call mongo_apply_viewport
 call giza_subpanel(panel_nx, panel_ny)
 px = mod(ipanel - 1, panel_nx) + 1
 py = (ipanel - 1) / panel_nx + 1
 call giza_set_panel(px, py)
end subroutine window

! Status: IMPLEMENTED
! Routine: setlim
subroutine setlim(xmin, ymin, xmax, ymax)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: xmin, ymin, xmax, ymax
 wxmin = xmin
 wymin = ymin
 wxmax = xmax
 wymax = ymax
 call mongo_ensure_device
 call mongo_apply_window
end subroutine setlim

! Status: IMPLEMENTED
! Routine: getlim
subroutine getlim(xmin, ymin, xmax, ymax)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: xmin, ymin, xmax, ymax
 xmin = wxmin
 ymin = wymin
 xmax = wxmax
 ymax = wymax
end subroutine getlim

! Status: IMPLEMENTED
! Routine: setexp
subroutine setexp(xlo, xhi, ylo, yhi)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: xlo, xhi, ylo, yhi
 xlexp = xlo
 xhexp = xhi
 ylexp = ylo
 yhexp = yhi
end subroutine setexp

! Status: IMPLEMENTED
! Routine: ticksize
subroutine ticksize(xmaj, xmin, ymaj, ymin)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: xmaj, xmin, ymaj, ymin
 asmallx = xmaj
 abigx = xmin
 asmally = ymaj
 abigy = ymin
 tick_xmaj = xmaj
 tick_xmin = xmin
 tick_ymaj = ymaj
 tick_ymin = ymin
 if (ymaj < 0.d0) ylog = .true.
 if (xmaj < 0.d0) xlog = .true.
end subroutine ticksize

! Status: IMPLEMENTED
! Routine: abox
subroutine abox(ixlab, iylab, ixnum, iynum)
 use gizamongo
 implicit none
 integer, intent(in) :: ixlab, iylab, ixnum, iynum
 call mongo_draw_box(ixlab, iylab, ixnum, iynum)
end subroutine abox

! Status: IMPLEMENTED
! Routine: connect
subroutine connect(x, y, n)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: n
 real(kind=8), intent(in) :: x(n), y(n)
 integer :: j
 call mongo_ensure_device
 call mongo_apply_window
 if (n < 2) return
 call relocate(x(1), y(1))
 do j = 2, n
    call draw(x(j), y(j))
 end do
end subroutine connect

! Status: IMPLEMENTED
! Routine: setcolor
subroutine setcolor(ic)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: ic
 mongo_colour = ic
 call giza_set_colour_index(ic)
end subroutine setcolor

! Status: IMPLEMENTED
! Routine: setexpand
subroutine setexpand(fac)
 use gizamongo
 use giza
 implicit none
 real(kind=8), intent(in) :: fac
 expand = fac
 if (fac > 0.d0) call giza_set_character_height(fac)
end subroutine setexpand

! Status: IMPLEMENTED
! Routine: setltype
subroutine setltype(i)
 use giza
 use gizamongo
 implicit none
 integer, intent(in) :: i
 integer :: ls
 mongo_ltype = i
 select case (i)
 case (1)
    ls = giza_ls_solid
 case (2)
    ls = giza_ls_long_dash
 case (3)
    ls = giza_ls_short_dash
 case (4)
    ls = giza_ls_dot
 case default
    ls = giza_ls_solid
 end select
 call giza_set_line_style(ls)
end subroutine setltype

! Status: IMPLEMENTED
! Routine: setlweight
subroutine setlweight(iw)
 use giza
 use gizamongo
 implicit none
 integer, intent(in) :: iw
 mongo_lweight = iw
 call giza_set_line_width(real(iw, kind=8))
end subroutine setlweight

! Status: IMPLEMENTED
! Routine: erase
subroutine erase
 use gizamongo
 use giza
 call mongo_ensure_device
 call giza_draw_background
end subroutine erase

! Status: IMPLEMENTED
! Routine: xlabel
subroutine xlabel(n, str)
 use gizamongo
 implicit none
 integer, intent(in) :: n
 character(len=*), intent(in) :: str
 label_x = str(1:min(n, len(str)))
end subroutine xlabel

! Status: IMPLEMENTED
! Routine: ylabel
subroutine ylabel(n, str)
 use gizamongo
 implicit none
 integer, intent(in) :: n
 character(len=*), intent(in) :: str
 label_y = str(1:min(n, len(str)))
end subroutine ylabel

! Status: IMPLEMENTED
! Routine: rlabel
subroutine rlabel(n, str)
 use gizamongo
 implicit none
 integer, intent(in) :: n
 character(len=*), intent(in) :: str
 label_r = str(1:min(n, len(str)))
end subroutine rlabel

! Status: IMPLEMENTED
! Routine: relocate
subroutine relocate(x, y)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: x, y
 pen_x = x
 pen_y = y
 call mongo_world_to_dev(x, y, xp, yp)
end subroutine relocate

! Status: IMPLEMENTED
! Routine: putlabel
subroutine putlabel(n, str, orient)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: n, orient
 character(len=*), intent(in) :: str
 real(kind=8) :: angle, just, tx, ty, tw, th, xsign, ysign, dx, dy
 character(len=256) :: text
 if (expand <= 0.d0) return
 text = str(1:min(n, len(str)))
 angle = 0.d0
 just = 0.5d0
 call mongo_ensure_device
 call mongo_apply_window
 call gstrlen(n, str, tw, th)
 xsign = real(mod(orient - 1, 3) - 1, kind=8)
 ysign = real((orient - 1) / 3 - 1, kind=8)
 if (orient /= 6) then
    dx = (xsign - 1.d0) * 0.5d0 * tw
    dy = ysign * 0.5d0 * th
    tx = pen_x + dx
    ty = pen_y + dy
 else
    tx = pen_x
    ty = pen_y
 end if
 if (orient == 1 .or. orient == 4 .or. orient == 7) just = 0.d0
 if (orient == 3 .or. orient == 6 .or. orient == 9) just = 1.d0
 call giza_ptext(tx, ty, angle, just, trim(text))
end subroutine putlabel

! Status: IMPLEMENTED
! Routine: getphysical
subroutine getphysical(x1, y1, x2, y2)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: x1, y1, x2, y2
 call mongo_ensure_device
 x1 = lx1
 y1 = ly1
 x2 = lx2
 y2 = ly2
end subroutine getphysical

! Status: IMPLEMENTED
! Routine: margins
subroutine margins(bot, left, top, right)
 use gizamongo
 use giza
 implicit none
 real(kind=8), intent(in) :: bot, left, top, right
 real(dp) :: mgx1, mgy1, mgx2, mgy2
 real(dp) :: bw, bh, lw, lh, tw, th, rw, rh
 call mongo_ensure_device
 bw = 0.d0
 bh = 3.d0
 lw = 6.d0
 lh = 2.d0
 tw = 0.d0
 th = 1.d0
 rw = 0.d0
 rh = 1.d0
 call window(1, 1, 1)
 mgy1 = ly1 + bot * ypin + expand * (bw * cwidth + bh * cheight)
 mgx1 = lx1 + left * xpin + expand * (lw * cwidth + lh * cheight)
 mgy2 = ly2 - top * ypin - expand * (tw * cwidth + th * cheight)
 mgx2 = lx2 - right * xpin - expand * (rw * cwidth + rh * cheight)
 call setloc(mgx1, mgy1, mgx2, mgy2)
end subroutine margins

! Status: IMPLEMENTED
! Routine: submargins
subroutine submargins(xfac, yfac)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: xfac, yfac
 submarg_x = xfac
 submarg_y = yfac
end subroutine submargins

! Status: IMPLEMENTED
! Routine: makecolor
subroutine makecolor(i, r, g, b)
 use giza
 implicit none
 integer, intent(in) :: i
 real(kind=8), intent(in) :: r, g, b
 call giza_set_colour_representation(i, real(r, kind=8), real(g, kind=8), &
      real(b, kind=8))
end subroutine makecolor

! Status: IMPLEMENTED
! Routine: mongo
subroutine mongo(numcmd, cmd, npts, ncol, data)
 use gizamongo
 implicit none
 integer, intent(in) :: numcmd, npts, ncol
 character(len=*), intent(in) :: cmd(*)
 real(kind=8), intent(in) :: data(npts, ncol)
 integer :: i
 do i = 1, numcmd
    call mongo_exec_command(cmd(i), npts, ncol, data)
 end do
end subroutine mongo

! Status: IMPLEMENTED
! Routine: bgnpol
subroutine bgnpol
 use gizamongo
 npol = 0
 pol_current_color = mongo_colour
end subroutine bgnpol

! Status: IMPLEMENTED
! Routine: v2f
subroutine v2f(v)
 use gizamongo
 implicit none
 real(kind=4), intent(in) :: v(*)
 if (npol >= maxpol) return
 npol = npol + 1
 pol_x(npol) = real(v(1), kind=dp)
 pol_y(npol) = real(v(2), kind=dp)
 pol_c(npol) = pol_current_color
end subroutine v2f

! Status: IMPLEMENTED
! Routine: color
subroutine color(i)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: i
 pol_current_color = i
 mongo_colour = i
 call giza_set_colour_index(i)
end subroutine color

! Status: IMPLEMENTED
! Routine: endpol
subroutine endpol
 use gizamongo
 use giza
 implicit none
 integer :: icol
 real(dp) :: rsum
 if (npol >= 3) then
    call mongo_ensure_device
    call mongo_apply_window
    rsum = 0.d0
    icol = pol_c(1)
    if (npol > 0) then
       rsum = sum(real(pol_c(1:npol), kind=dp))
       icol = max(0, nint(rsum / real(npol, kind=dp)))
    end if
    call setcolor(icol)
    call giza_set_fill(giza_fill_solid)
    call giza_polygon(npol, pol_x, pol_y)
    call giza_set_fill(giza_fill_hollow)
 end if
 npol = 0
end subroutine endpol

! Status: IMPLEMENTED
! Routine: tuvmx11open
subroutine tuvmx11open(loc)
 use gizamongo
 use giza
 implicit none
 character(len=*), intent(in) :: loc
 integer :: iwidth, iheight
 character(len=256) :: prefix
 mongo_device = 11
 if (device_open) call mgoclose
 if (len_trim(mongo_window_title) > 0) then
    prefix = trim(mongo_window_title)
 else
    prefix = 'giza'
 end if
 call mongo_parse_x11_geometry(loc, iwidth, iheight)
 if (iwidth > 0 .and. iheight > 0) then
    call giza_open(dev='/xw', prefix=trim(prefix), &
         width=real(iwidth), height=real(iheight), units=giza_units_pixels)
    device_open = .true.
    physical_init = .false.
    call mongo_init_physical
    call mongo_apply_viewport
 else
    call mongo_ensure_device
 end if
 mongo_movie_mode = .false.
end subroutine tuvmx11open

! Status: IMPLEMENTED
! Routine: tuvmx11movie
subroutine tuvmx11movie
 use gizamongo
 use giza
 call mongo_ensure_device
 mongo_movie_mode = .true.
 call giza_begin_buffer
end subroutine tuvmx11movie

! Status: IMPLEMENTED
! Routine: tuvmx11direct
subroutine tuvmx11direct
 use gizamongo
 use giza
 if (mongo_movie_mode) call giza_end_buffer
 mongo_movie_mode = .false.
end subroutine tuvmx11direct

! Status: IMPLEMENTED
! Routine: tuvmx11mapimage
subroutine tuvmx11mapimage
 use gizamongo
 use giza
 if (.not. device_open) return
 if (mongo_movie_mode) then
    call giza_flush_buffer
 else
    call giza_flush_device
 end if
end subroutine tuvmx11mapimage

! Status: IMPLEMENTED
! Routine: tidle
subroutine tidle
 use gizamongo
 use giza
 if (device_open .and. .not. mongo_movie_mode) call giza_flush_device
end subroutine tidle

! Status: IMPLEMENTED
! Routine: mgoclose
subroutine mgoclose
 use gizamongo
 use giza
 if (device_open) call giza_close
 device_open = .false.
 physical_init = .false.
end subroutine mgoclose

! Status: PARTIAL
! Routine: psplot
subroutine psplot(flag, name, opt)
 use gizamongo
 use giza
 implicit none
 logical, intent(in) :: flag
 character(len=*), intent(in) :: name, opt
 character(len=512) :: dev
 if (device_open) call giza_close
 device_open = .false.
 physical_init = .false.
 mongo_device = 2
 if (len_trim(name) > 0) then
    dev = trim(name)//'/ps'
 else
    dev = '/ps'
 end if
 call giza_open(dev=trim(dev))
 device_open = .true.
 call mongo_init_physical
 call mongo_apply_viewport
end subroutine psplot

! Status: STUB
! Routine: setpsfmode
subroutine setpsfmode(i)
 use gizamongo
 implicit none
 integer, intent(in) :: i
 psfmode = i
end subroutine setpsfmode

! Status: IMPLEMENTED
! Routine: softphysical
subroutine softphysical(x1, y1, x2, y2, m1, m2, m3, m4)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: x1, y1, x2, y2, m1, m2, m3, m4
 real(dp) :: rm, lm, bm, tm
 call mongo_ensure_device
 rm = m1
 lm = m2
 bm = m3
 tm = m4
 if (rm < 0.d0) rm = 0.10d0
 if (lm < 0.d0) lm = 0.05d0
 if (bm < 0.d0) bm = 0.10d0
 if (tm < 0.d0) tm = 0.05d0
 lx1 = x1
 ly1 = y1
 lx2 = x2
 ly2 = y2
 gx1 = lx1 + lm * (lx2 - lx1 + 1.d0)
 gx2 = lx2 - rm * (lx2 - lx1 + 1.d0)
 gy1 = ly1 + bm * (ly2 - ly1 + 1.d0)
 gy2 = ly2 - tm * (ly2 - ly1 + 1.d0)
 old_gx1 = gx1
 old_gx2 = gx2
 old_gy1 = gy1
 old_gy2 = gy2
 mongo_custom_viewport = .true.
 physical_init = .true.
 call mongo_apply_viewport
end subroutine softphysical

! Status: IMPLEMENTED
! Routine: setloc
subroutine setloc(x1, y1, x2, y2)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: x1, y1, x2, y2
 gx1 = x1
 gy1 = y1
 gx2 = x2
 gy2 = y2
 mongo_custom_viewport = .true.
 call mongo_ensure_device
 call mongo_apply_viewport
end subroutine setloc

! Status: IMPLEMENTED
! Routine: winname
subroutine winname(a, b)
 use gizamongo
 use giza
 implicit none
 character(len=*), intent(in) :: a, b
 character(len=256) :: title
 if (len_trim(a) > 0) then
    title = trim(a)
 else
    title = trim(b)
 end if
 if (len_trim(title) == 0) return
 mongo_window_title = title
 if (device_open) call giza_set_window_title(title)
end subroutine winname

! Status: IMPLEMENTED
! Routine: invertpic
subroutine invertpic(flag)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: flag
 call mongo_ensure_device
 if (flag <= 0) then
    ! Black background, light foreground (Kepler follows with setcolor(1)).
    call giza_set_colour_representation(0, 0.d0, 0.d0, 0.d0)
    call giza_set_colour_representation(1, 1.d0, 1.d0, 1.d0)
 else
    ! White background, dark foreground.
    call giza_set_colour_representation(0, 1.d0, 1.d0, 1.d0)
    call giza_set_colour_representation(1, 0.d0, 0.d0, 0.d0)
 end if
 mongo_colour = 1
 call giza_set_colour_index(1)
end subroutine invertpic

! Status: IMPLEMENTED
! Routine: fileplot
integer function fileplot()
 use gizamongo
 use giza
 implicit none
 call window(1, 1, 1)
 if (.not. device_open) then
    fileplot = 0
    return
 end if
 call giza_flush_device
 if (mongo_device == 2) then
    call giza_close
    device_open = .false.
    physical_init = .false.
    fileplot = -17
 else
    fileplot = 0
 end if
end function fileplot

! Status: IMPLEMENTED
! Routine: gdraw
subroutine gdraw(x, y)
 use gizamongo
 use giza
 implicit none
 real(kind=8), intent(in) :: x, y
 real(dp) :: wx1, wy1, wx2, wy2
 call mongo_ensure_device
 call mongo_apply_window
 call mongo_dev_to_world(xp, yp, wx1, wy1)
 call mongo_dev_to_world(x, y, wx2, wy2)
 call giza_line(2, (/wx1, wx2/), (/wy1, wy2/))
 xp = x
 yp = y
 call mongo_dev_to_world(x, y, pen_x, pen_y)
end subroutine gdraw

! Status: IMPLEMENTED
! Routine: grelocate
subroutine grelocate(x, y)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: x, y
 xp = x
 yp = y
 call mongo_dev_to_world(x, y, pen_x, pen_y)
end subroutine grelocate

! Status: PARTIAL
! Routine: gstrlen
subroutine gstrlen(n, str, length, height)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: n
 character(len=*), intent(in) :: str
 real(kind=8), intent(out) :: length, height
 character(len=256) :: text
 text = str(1:min(n, len(str)))
 call giza_qtextlen(giza_units_world, trim(text), length, height)
 length = length * expand
 height = height * expand
end subroutine gstrlen

! Status: IMPLEMENTED
! Routine: xlogarithm
subroutine xlogarithm
 use gizamongo
 xlog = .true.
end subroutine xlogarithm

! Status: IMPLEMENTED
! Routine: ylogarithm
subroutine ylogarithm
 use gizamongo
 ylog = .true.
end subroutine ylogarithm

! Status: IMPLEMENTED
! Routine: draw
subroutine draw(x, y)
 use gizamongo
 use giza
 implicit none
 real(kind=8), intent(in) :: x, y
 call mongo_ensure_device
 call mongo_apply_window
 call giza_move(pen_x, pen_y)
 call giza_draw(x, y)
 pen_x = x
 pen_y = y
 call mongo_world_to_dev(x, y, xp, yp)
end subroutine draw

! Status: IMPLEMENTED
! Routine: line
subroutine line(x1, y1, x2, y2)
 implicit none
 real(kind=8), intent(in) :: x1, y1, x2, y2
 call relocate(x1, y1)
 call draw(x2, y2)
end subroutine line

! Status: IMPLEMENTED
! Routine: reset
subroutine reset
 use gizamongo
 use giza
 implicit none
 call window(1, 1, 1)
 expand = 1.d0
 call ticksize(0.d0, 0.d0, 0.d0, 0.d0)
 call setexp(1.d-4, 1.d5, 1.d-4, 1.d5)
 call setlweight(1)
 call setcolor(1)
end subroutine reset

! Status: IMPLEMENTED
! Routine: getloc
subroutine getloc(x1, y1, x2, y2)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: x1, y1, x2, y2
 x1 = gx1
 y1 = gy1
 x2 = gx2
 y2 = gy2
end subroutine getloc

! Status: IMPLEMENTED
! Routine: getgxy
subroutine getgxy(x, y)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: x, y
 x = xp
 y = yp
end subroutine getgxy

! Status: IMPLEMENTED
! Routine: getcolor
subroutine getcolor(ic)
 use gizamongo
 implicit none
 integer, intent(out) :: ic
 ic = mongo_colour
end subroutine getcolor

! Status: IMPLEMENTED
! Routine: getexpand
subroutine getexpand(fac)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: fac
 fac = expand
end subroutine getexpand

! Status: IMPLEMENTED
! Routine: getltype
subroutine getltype(i)
 use gizamongo
 implicit none
 integer, intent(out) :: i
 i = mongo_ltype
end subroutine getltype

! Status: IMPLEMENTED
! Routine: getlweight
subroutine getlweight(iw)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: iw
 iw = real(mongo_lweight, kind=8)
end subroutine getlweight

! Status: IMPLEMENTED
! Routine: getpsfmode
subroutine getpsfmode(i)
 use gizamongo
 implicit none
 integer, intent(out) :: i
 i = psfmode
end subroutine getpsfmode

! Status: IMPLEMENTED
! Routine: getlvis
subroutine getlvis(i)
 implicit none
 integer, intent(out) :: i
 i = 0
end subroutine getlvis

! Status: IMPLEMENTED
! Routine: getangle
subroutine getangle(ang)
 use gizamongo
 implicit none
 real(kind=8), intent(out) :: ang
 ang = mongo_angle
end subroutine getangle

! Status: IMPLEMENTED
! Routine: setangle
subroutine setangle(ang)
 use gizamongo
 implicit none
 real(kind=8), intent(in) :: ang
 mongo_angle = ang
end subroutine setangle

! Status: IMPLEMENTED
! Routine: label
subroutine label(n, str)
 implicit none
 integer, intent(in) :: n
 character(len=*), intent(in) :: str
 call putlabel(n, str, 6)
end subroutine label

! Status: IMPLEMENTED
! Routine: points
subroutine points(style, nstyle, x, y, n)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: nstyle, n
 real(kind=8), intent(in) :: style(*), x(n), y(n)
 integer :: j, sym
 call mongo_ensure_device
 call mongo_apply_window
 do j = 1, n
    sym = 2
    if (nstyle >= 1) sym = max(1, mod(int(abs(style(min(j, nstyle))) + 0.001d0), 10))
    call giza_single_point(x(j), y(j), sym)
 end do
end subroutine points

! Status: IMPLEMENTED
! Routine: histogram
subroutine histogram(x, y, n)
 use gizamongo
 implicit none
 integer, intent(in) :: n
 real(kind=8), intent(in) :: x(n), y(n)
 integer :: j
 if (n < 2) return
 call mongo_ensure_device
 call mongo_apply_window
 call relocate(x(1), y(1))
 do j = 2, n
    call draw(x(j), y(j-1))
    call draw(x(j), y(j))
 end do
end subroutine histogram

! Status: IMPLEMENTED
! Routine: errorbar
subroutine errorbar(location, x, y, err, n)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: location, n
 real(kind=8), intent(in) :: x(n), y(n), err(n)
 real(kind=8) :: x1, y1, x2, y2
 integer :: j
 if (n < 1) return
 call mongo_ensure_device
 call mongo_apply_window
 do j = 1, n
    if (location == 1) then
       x1 = x(j)
       y1 = y(j)
       x2 = x(j) + err(j)
       y2 = y(j)
    else if (location == 2) then
       x1 = x(j)
       y1 = y(j)
       x2 = x(j)
       y2 = y(j) + err(j)
    else if (location == 3) then
       x1 = x(j)
       y1 = y(j)
       x2 = x(j) - err(j)
       y2 = y(j)
    else
       x1 = x(j)
       y1 = y(j)
       x2 = x(j)
       y2 = y(j) - err(j)
    end if
    call giza_line(2, (/x1, x2/), (/y1, y2/))
 end do
end subroutine errorbar

! Status: IMPLEMENTED
! Routine: mpolygon
subroutine mpolygon(x, y, n, icfill, x11p)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: n, icfill
 real(kind=8), intent(in) :: x(n), y(n)
 integer*2, intent(in) :: x11p(2, *)
 integer :: saved
 if (n < 3) return
 call mongo_ensure_device
 call mongo_apply_window
 saved = mongo_colour
 if (icfill > 0) call setcolor(icfill)
 call giza_set_fill(giza_fill_solid)
 call giza_polygon(n, x, y)
 call giza_set_fill(giza_fill_hollow)
 if (icfill > 0) call setcolor(saved)
end subroutine mpolygon

! Status: STUB
! Routine: contour
subroutine contour(z, m, n, level, nlevel)
 use gizamongo
 implicit none
 integer, intent(in) :: m, n, nlevel
 real(kind=8), intent(in) :: z(m, n), level(nlevel)
 call mongo_warn('contour not yet implemented')
end subroutine contour

! Status: IMPLEMENTED
! Routine: point
subroutine point(n, istyle)
 use gizamongo
 use giza
 implicit none
 integer, intent(in) :: n, istyle
 integer :: j, nsides
 real(dp) :: dtheta, theta, xa, ya, xb, yb, xmid, ymid
 real(dp) :: cx, cy, rx, ry, scalex, scaley
 real(dp), allocatable :: px(:), py(:)
 character(len=5) :: strdot
 if (expand <= 0.d0) return
 call mongo_ensure_device
 call mongo_apply_window
 if (n < 0) then
    if (xp < gx1 .or. xp > gx2 .or. yp < gy1 .or. yp > gy2) return
    if (istyle == 0) then
       strdot = char(-n)//'    '
    else
       strdot = char(92)//char(ichar('A') + mod(abs(istyle) - 1, 26)) &
            //char(-n)//'  '
    end if
    call putlabel(len_trim(strdot), strdot, 5)
    return
 end if
 if (n < 2) then
    call grelocate(xp, yp)
    call gdraw(xp, yp)
    return
 end if
 nsides = n
 cx = pen_x
 cy = pen_y
 if (gx2 > gx1) then
    scalex = (wxmax - wxmin) / (gx2 - gx1)
 else
    scalex = 1.d0
 end if
 if (gy2 > gy1) then
    scaley = (wymax - wymin) / (gy2 - gy1)
 else
    scaley = 1.d0
 end if
 rx = expand * pdef * scalex
 ry = expand * pdef * scaley
 dtheta = 2.d0 * mongo_pi / nsides
 theta = (3.d0 * mongo_pi + dtheta) / 2.d0 + mongo_angle * mongo_pi / 180.d0
 if (istyle == point_open .or. nsides == 2) then
    xa = cx + rx * cos(theta)
    ya = cy + ry * sin(theta)
    call relocate(xa, ya)
    do j = 1, nsides
       theta = theta + dtheta
       xb = cx + rx * cos(theta)
       yb = cy + ry * sin(theta)
       call draw(xb, yb)
    end do
    call relocate(cx, cy)
 else if (istyle == point_skeletal) then
    xa = cx + rx * cos(theta)
    ya = cy + ry * sin(theta)
    do j = 1, nsides
       call relocate(xa, ya)
       if (2 * (nsides / 2) == nsides) then
          if (j > nsides / 2) exit
          call draw(2.d0 * cx - xa, 2.d0 * cy - ya)
       else
          call draw(cx, cy)
       end if
       theta = theta + dtheta
       xa = cx + rx * cos(theta)
       ya = cy + ry * sin(theta)
    end do
 else if (istyle == point_stellated) then
    xa = cx + rx * cos(theta)
    ya = cy + ry * sin(theta)
    call relocate(xa, ya)
    do j = 1, nsides
       theta = theta + dtheta
       xb = cx + rx * cos(theta)
       yb = cy + ry * sin(theta)
       xmid = 0.25d0 * (xa + xb - 2.d0 * cx) + cx
       ymid = 0.25d0 * (ya + yb - 2.d0 * cy) + cy
       call draw(xmid, ymid)
       call draw(xb, yb)
       xa = xb
       ya = yb
    end do
 else if (istyle == point_filled) then
    allocate(px(nsides), py(nsides))
    do j = 1, nsides
       px(j) = cx + rx * cos(theta + (j - 1) * dtheta)
       py(j) = cy + ry * sin(theta + (j - 1) * dtheta)
    end do
    call giza_set_fill(giza_fill_solid)
    call giza_polygon(nsides, px, py)
    call giza_set_fill(giza_fill_hollow)
    deallocate(px, py)
 end if
end subroutine point
