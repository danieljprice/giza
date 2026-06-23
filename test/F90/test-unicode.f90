! giza test program — see COPYING.LESSER in the top-level directory.
! This library is free software; redistribute under the terms of the
! GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
!
!---------------------------------------------------------------------------
!
! Regression test for issue #73: Unicode characters in plot strings.
!
!---------------------------------------------------------------------------
program test_unicode
  use giza
  implicit none
  character(len=*), parameter :: text = '© Giza'
  integer :: id
  logical :: exists
  real :: xbox(4), ybox(4), xlen, ylen

  if (.not. text_length_ok(text)) then
     write(*,*) 'Error: Unicode string has zero measured width'
     stop 1
  endif

  id = giza_open_device('/png', 'test-unicode-f90')
  if (id <= 0) then
     write(*,*) 'Error: giza_open_device failed'
     stop 1
  endif

  call giza_start_warnings()
  call giza_set_environment(0., 1., 0., 1., 0, 0)
  call giza_box('BCNT', 0., 0, 'BCNT', 0., 0)
  call giza_qtext(0.5, 0.5, 0., 0.5, text, xbox, ybox)
  call giza_ptext(0.5, 0.5, 0., 0.5, text)
  call giza_label('x', 'y', text)
  call giza_close()

  inquire(file='test-unicode-f90.png', exist=exists)
  if (.not. exists) then
     write(*,*) 'Error: required file test-unicode-f90.png does not exist'
     stop 1
  endif

  write(*,*) 'Unicode text test passed (Fortran)'

contains

  !---------------------------------------------------------------------------
  ! verify giza_qtextlen returns non-zero size for a UTF-8 string
  !---------------------------------------------------------------------------
  logical function text_length_ok(text)
    character(len=*), intent(in) :: text
    real :: xlen, ylen
    integer :: id_len

    id_len = giza_open_device('/null', 'test-unicode-f90-len')
    call giza_qtextlen(giza_units_normalized, text, xlen, ylen)
    call giza_close()
    text_length_ok = (xlen > 0. .and. ylen > 0.)

  end function text_length_ok

end program test_unicode
