program test_pgconx
! Test PGCONX (contour with user callback)
 implicit none
 integer, parameter :: NX = 50, NY = 50
 real :: data(NX, NY), contours(5), pi
 integer :: i, j
 external :: myplot

 pi = 4.0 * atan(1.0)

 do j = 1, NY
    do i = 1, NX
       data(i,j) = sin(-pi + real(i-1)*2.0*pi/real(NX)) &
                 * cos(-pi + real(j-1)*2.0*pi/real(NY))
    enddo
 enddo

 contours = (/ -0.8, -0.4, 0.0, 0.4, 0.8 /)

 call pgbeg(0, 'test-pgconx.png/png', 1, 1)
 call pgenv(-pi, pi, -pi, pi, 1, 0)
 call pglab('X', 'Y', 'PGCONX: contour with user callback')

 call pgsci(2)
 call pgconx(data, NX, NY, 1, NX, 1, NY, contours, 5, myplot)

 call pgsci(1)
 call pgbox('BCNTS', 0.0, 0, 'BCNTS', 0.0, 0)

 call pgend

end program test_pgconx

! User callback: transform grid indices to world coords
subroutine myplot(visble, x, y, z)
 implicit none
 integer, intent(in) :: visble
 real, intent(in)    :: x, y, z
 real :: pi, scale, offset, wx, wy

 pi = 4.0 * atan(1.0)
 scale = 2.0 * pi / 50.0
 offset = -pi

 wx = (x - 0.5) * scale + offset
 wy = (y - 0.5) * scale + offset

 if (visble == 0) then
    call pgmove(wx, wy)
 else
    call pgdraw(wx, wy)
 endif

end subroutine myplot
