! giza test program — see COPYING.LESSER in the top-level directory.
! This library is free software; redistribute under the terms of the
! GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
!
program test_pgncur
 implicit none
 integer, parameter :: maxpt = 100
 integer :: npt,i
 real, dimension(maxpt) :: x, y

 npt = 0
 call pgbeg(0,'/xw',1,1)
 call pgask(.false.)
 call pgenv(0.,1.,0.,1.,0,0)
! call pgolin(maxpt,npt,x,y,3)
 call pgncur(maxpt,npt,x,y,3)
 call pgend
 do i=1,npt
    print*,i,x(i),y(i)
 enddo

end program test_pgncur
