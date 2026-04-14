program test_pgconf
! Test PGCONF (filled contours) with PGCONT overlay
 implicit none
 integer, parameter :: NX = 100, NY = 100
 real :: data(NX, NY), tr(6)
 real :: x, y, levels(5), cont(5)
 integer :: i, j, colors(4)

 tr(1) = -1.5 - 3.0/NX
 tr(2) = 3.0/NX
 tr(3) = 0.0
 tr(4) = -1.5 - 3.0/NY
 tr(5) = 0.0
 tr(6) = 3.0/NY

 do j = 1, NY
    do i = 1, NX
       x = tr(1) + tr(2)*real(i)
       y = tr(4) + tr(6)*real(j)
       data(i,j) = x * cos(3.0*x*y)
    enddo
 enddo

 levels = (/ -1.0, -0.5, 0.0, 0.5, 1.0 /)
 colors = (/ 2, 3, 5, 7 /)

 call pgbeg(0, 'test-pgconf.png/png', 1, 1)
 call pgenv(-1.5, 1.5, -1.5, 1.5, 1, 0)
 call pglab('X', 'Y', 'PGCONF: filled contours')

 ! Fill bands
 do i = 1, 4
    call pgsci(colors(i))
    call pgconf(data, NX, NY, 1, NX, 1, NY, levels(i), levels(i+1), tr)
 enddo

 ! Overlay line contours
 call pgsci(1)
 cont = levels
 call pgcont(data, NX, NY, 1, NX, 1, NY, cont, 5, tr)

 call pgend

end program test_pgconf
