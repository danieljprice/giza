! Kepler-shaped libmgo integration slices (headless via PNG/PS devices).
program test_mongo_kepler
 implicit none
 call test_kepler_startup
 call test_kepler_points
 call test_kepler_plotmap
 call test_kepler_mongo_batch
 call test_kepler_ps
end program test_mongo_kepler

subroutine test_kepler_startup
 implicit none
 real(kind=8) :: x1, y1, x2, y2

 call device(5)
 call winname('kepler-test', 'kepler-test')
 call tsetup
 call tuvmx11movie
 call tuvmx11direct
 call invertpic(1)
 call erase
 call setcolor(1)
 call softphysical(0.d0, 0.d0, 800.d0, 600.d0, 0.d0, 0.d0, 0.d0, 0.d0)
 call getphysical(x1, y1, x2, y2)
 call margins(1.d0, 1.2d0, 1.3d0, 1.2d0)
 call window(1, 2, 2)
 call setlim(0.d0, 0.d0, 1.d0, 1.d0)
 call abox(1, 2, 0, -1)
 call tuvmx11mapimage
 call tidle
 call mgoclose
end subroutine test_kepler_startup

subroutine test_kepler_points
 implicit none
 integer, parameter :: n = 8
 integer :: j
 real(kind=8) :: x(n), y(n)

 do j = 1, n
    x(j) = real(j - 1, kind=8) / real(n - 1, kind=8)
    y(j) = 0.5d0 + 0.4d0 * sin(x(j) * 6.283185307179586d0)
 end do

 call device(5)
 call tsetup
 call invertpic(1)
 call erase
 call setcolor(1)
 call softphysical(0.d0, 0.d0, 640.d0, 480.d0, 0.d0, 0.d0, 0.d0, 0.d0)
 call setlim(0.d0, 0.d0, 1.d0, 1.d0)
 call abox(1, 2, 0, 0)
 call setexpand(1.d0)
 call setcolor(2)
 call connect(x, y, n)
 do j = 1, n
    call relocate(x(j), y(j))
    call point(5, 0)
 end do
 do j = 1, n
    call relocate(x(j), y(j))
    call point(5, 3)
 end do
 call mgoclose
end subroutine test_kepler_points

subroutine test_kepler_plotmap
 implicit none
 real(kind=4) :: vertex(2)

 call device(5)
 call tsetup
 call invertpic(1)
 call erase
 call setcolor(1)
 call softphysical(0.d0, 0.d0, 640.d0, 480.d0, 0.d0, 0.d0, 0.d0, 0.d0)
 call setlim(0.d0, 0.d0, 10.d0, 1.d0)
 call abox(1, 2, 0, 0)

 call bgnpol
 call color(2)
 vertex(1) = 1.0
 vertex(2) = 0.2
 call v2f(vertex)
 call color(5)
 vertex(1) = 9.0
 vertex(2) = 0.2
 call v2f(vertex)
 call color(8)
 vertex(1) = 9.0
 vertex(2) = 0.8
 call v2f(vertex)
 call color(11)
 vertex(1) = 1.0
 vertex(2) = 0.8
 call v2f(vertex)
 call endpol

 call mgoclose
end subroutine test_kepler_plotmap

subroutine test_kepler_mongo_batch
 implicit none
 integer, parameter :: npts = 20
 integer :: numcmd
 character(len=120), dimension(32) :: cmd
 real(kind=8), dimension(npts, 8) :: data
 integer :: i

 do i = 1, npts
    data(i, 1) = real(i, kind=8)
    data(i, 2) = sin(real(i, kind=8) * 0.3d0)
    data(i, 3) = cos(real(i, kind=8) * 0.3d0)
 end do

 numcmd = 21
 cmd(1) = 'terminal 5'
 cmd(2) = 'reset'
 cmd(3) = 'lines 1 20'
 cmd(4) = 'xcolumn 1'
 cmd(5) = 'ycolumn 2'
 cmd(6) = 'xlimits 1 20'
 cmd(7) = 'ylimits -1.2 1.2'
 cmd(8) = 'ylogarithm'
 cmd(9) = 'ticksize 0 0 -1 0'
 cmd(10) = 'erase'
 cmd(11) = 'box 1 2 0 -1'
 cmd(12) = 'xlabel time'
 cmd(13) = 'tlabel kepler slice'
 cmd(14) = 'color 2'
 cmd(15) = 'ylabel primary'
 cmd(16) = 'lweight 2'
 cmd(17) = 'connect'
 cmd(18) = 'ycolumn 3'
 cmd(19) = 'ylimits -1.5 1.5'
 cmd(20) = 'box -1 -1 -1 2'
 cmd(21) = 'connect'

 call mongo(numcmd, cmd, npts, 8, data)
 call mgoclose
end subroutine test_kepler_mongo_batch

subroutine test_kepler_ps
 implicit none
 integer, external :: fileplot
 integer :: nplot
 real(kind=8) :: x(2), y(2)

 x = (/1.d0, 2.d0/)
 y = (/0.d0, 1.d0/)

 call psplot(.false., 'giza-mongo-kepler', ' ')
 call invertpic(1)
 call erase
 call setcolor(1)
 call softphysical(0.d0, 0.d0, 576.d0, 828.d0, 0.d0, 0.d0, 0.d0, 0.d0)
 call setlim(0.d0, 0.d0, 2.d0, 1.d0)
 call abox(1, 2, 0, 0)
 call connect(x, y, 2)
 nplot = fileplot()
 if (nplot /= -17) then
    write (*, '(a,i0)') 'test-mongo-kepler: expected fileplot=-17, got ', nplot
    stop 1
 end if
end subroutine test_kepler_ps
