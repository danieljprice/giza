! Batch MONGO() driver test (Kepler time-plot command strings).
program test_mongo_batch
 implicit none
 integer, parameter :: npts = 20
 integer :: numcmd, i
 character(len=120), dimension(32) :: cmd
 real(kind=8), dimension(npts, 8) :: data

 do i = 1, npts
    data(i, 1) = real(i, kind=8)
    data(i, 2) = sin(real(i, kind=8) * 0.3d0)
    data(i, 3) = cos(real(i, kind=8) * 0.3d0)
 end do

 numcmd = 13
 cmd(1) = 'terminal 5'
 cmd(2) = 'reset'
 cmd(3) = 'lines 1 20'
 cmd(4) = 'xcolumn 1'
 cmd(5) = 'ycolumn 2'
 cmd(6) = 'xlimits 1 20'
 cmd(7) = 'ylimits -1.2 1.2'
 cmd(8) = 'box 1 2 0 -1'
 cmd(9) = 'xlabel time'
 cmd(10) = 'ylabel value'
 cmd(11) = 'color 2'
 cmd(12) = 'lweight 2'
 cmd(13) = 'connect'

 call mongo(numcmd, cmd, npts, 8, data)
 call mgoclose

end program test_mongo_batch
