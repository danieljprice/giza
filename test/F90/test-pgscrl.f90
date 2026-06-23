! giza test program — see COPYING.LESSER in the top-level directory.
! This library is free software; redistribute under the terms of the
! GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
!
program test_pgscrl
! Test PGSCRL (scroll window)
 implicit none
 integer, parameter :: N = 100
 real :: x(N), y(N), x1, x2, y1, y2
 integer :: i

 do i = 1, N
    x(i) = real(i-1) * 10.0 / real(N-1)
    y(i) = sin(x(i))
 enddo

 call pgbeg(0, 'test-pgscrl.png/png', 1, 1)

 ! Page 1: before scroll
 call pgenv(0.0, 10.0, -1.5, 1.5, 0, 0)
 call pglab('X', 'Y', 'Before PGSCRL')
 call pgsci(2)
 call pgline(N, x, y)

 ! Page 2: draw then scroll
 call pgpage
 call pgenv(0.0, 10.0, -1.5, 1.5, 0, 0)
 call pgsci(2)
 call pgline(N, x, y)

 call pgscrl(2.0, 0.5)

 call pgqwin(x1, x2, y1, y2)
 write(*,'(A,4F8.3)') 'After PGSCRL(2.0,0.5): window =', x1, x2, y1, y2
 write(*,*) 'Expected: 2.000 12.000 -1.000  2.000'

 call pgsci(1)
 call pgbox('BCNTS', 0.0, 0, 'BCNTS', 0.0, 0)
 call pglab('X', 'Y', 'After PGSCRL(2.0, 0.5)')

 call pgend

end program test_pgscrl
