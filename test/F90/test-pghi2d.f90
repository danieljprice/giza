! giza test program — see COPYING.LESSER in the top-level directory.
! This library is free software; redistribute under the terms of the
! GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
!
program test_pghi2d
! Test PGHI2D (waterfall / cross-section plot)
 implicit none
 integer, parameter :: NXV = 40, NYV = 20
 real :: data(NXV, NYV), x(NXV), ylims(NXV)
 integer :: i, j

 ! Data: decaying sine waves
 do j = 1, NYV
    do i = 1, NXV
       data(i,j) = sin(real(i)/5.0) * exp(-real(j)/10.0)
    enddo
 enddo

 do i = 1, NXV
    x(i) = real(i)
 enddo

 call pgbeg(0, 'test-pghi2d.png/png', 1, 1)

 ! Page 1: basic waterfall
 call pgenv(0.0, 50.0, -1.5, 5.0, 0, 0)
 call pglab('X', 'Y', 'PGHI2D: waterfall plot')
 call pgsci(1)
 call pghi2d(data, NXV, NYV, 1, NXV, 1, NYV, x, 0, 0.2, .false., ylims)

 ! Page 2: with horizontal offset
 call pgpage
 call pgenv(0.0, 60.0, -1.5, 5.0, 0, 0)
 call pglab('X', 'Y', 'PGHI2D: waterfall with ioff=1')
 call pgsci(2)
 call pghi2d(data, NXV, NYV, 1, NXV, 1, NYV, x, 1, 0.2, .false., ylims)

 call pgend

end program test_pghi2d
