program test_pgconl
! Test PGCONL (labeled contours)
 implicit none
 integer, parameter :: NX = 80, NY = 80
 real :: data(NX, NY), tr(6)
 real :: x, y, pi
 integer :: i, j

 pi = 4.0 * atan(1.0)

 tr(1) = -pi - 2.0*pi/NX
 tr(2) = 2.0*pi/NX
 tr(3) = 0.0
 tr(4) = -pi - 2.0*pi/NY
 tr(5) = 0.0
 tr(6) = 2.0*pi/NY

 do j = 1, NY
    do i = 1, NX
       x = tr(1) + tr(2)*real(i)
       y = tr(4) + tr(6)*real(j)
       data(i,j) = sin(x) * cos(y)
    enddo
 enddo

 call pgbeg(0, 'test-pgconl.png/png', 1, 1)
 call pgenv(-pi, pi, -pi, pi, 1, 0)
 call pglab('X', 'Y', 'PGCONL: labeled contours of sin(x)cos(y)')

 call pgsci(2)
 call pgconl(data, NX, NY, 1, NX, 1, NY, -0.5, tr, '-0.5', 20, 10)

 call pgsci(1)
 call pgconl(data, NX, NY, 1, NX, 1, NY, 0.0, tr, '0.0', 20, 10)

 call pgsci(4)
 call pgconl(data, NX, NY, 1, NX, 1, NY, 0.5, tr, '+0.5', 20, 10)

 call pgsci(1)
 call pgbox('BCNTS', 0.0, 0, 'BCNTS', 0.0, 0)

 call pgend

end program test_pgconl
