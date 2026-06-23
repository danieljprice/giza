! giza test program — see COPYING.LESSER in the top-level directory.
! This library is free software; redistribute under the terms of the
! GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
!
program test_pgconb
! Test PGCONB (blanked contours) and PGCONS (for comparison)
 implicit none
 integer, parameter :: NX = 50, NY = 50
 real :: data(NX, NY), tr(6), contours(9)
 real :: x, y, r, blank, pi
 integer :: i, j

 pi = 4.0 * atan(1.0)

 ! tr maps (I,J) -> world: X = tr(1) + tr(2)*I + tr(3)*J
 tr(1) = -pi - 2.0*pi/NX
 tr(2) = 2.0*pi/NX
 tr(3) = 0.0
 tr(4) = -pi - 2.0*pi/NY
 tr(5) = 0.0
 tr(6) = 2.0*pi/NY

 ! Fill data: sin(x)*cos(y)
 do j = 1, NY
    do i = 1, NX
       x = tr(1) + tr(2)*real(i)
       y = tr(4) + tr(6)*real(j)
       data(i,j) = sin(x) * cos(y)
    enddo
 enddo

 ! Blank a ring where 0.8 < r < 1.2
 blank = -999.0
 do j = 1, NY
    do i = 1, NX
       x = tr(1) + tr(2)*real(i)
       y = tr(4) + tr(6)*real(j)
       r = sqrt(x*x + y*y)
       if (r > 0.8 .and. r < 1.2) data(i,j) = blank
    enddo
 enddo

 contours = (/ -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8 /)

 call pgbeg(0, 'test-pgconb.png/png', 1, 1)

 ! Page 1: blanked contours
 call pgenv(-pi, pi, -pi, pi, 1, 0)
 call pglab('X', 'Y', 'PGCONB: blanked ring (r=0.8..1.2)')
 call pgsci(1)
 call pgconb(data, NX, NY, 1, NX, 1, NY, contours, 9, tr, blank)

 ! Page 2: unblanked for comparison
 call pgpage
 call pgenv(-pi, pi, -pi, pi, 1, 0)
 call pglab('X', 'Y', 'PGCONS: same data, no blanking')
 call pgcons(data, NX, NY, 1, NX, 1, NY, contours, 9, tr)

 call pgend

end program test_pgconb
