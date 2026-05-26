! Basic regression test for giza-mongo (libmgo).
program test_mongo_basic
 implicit none
 real(kind=8) :: x(50), y(50)
 integer :: i

 do i = 1, 50
    x(i) = real(i, kind=8) * 0.1d0
    y(i) = sin(x(i))
 end do

 call device(5)
 call tsetup
 call setlim(0.d0, -1.2d0, 5.d0, 1.2d0)
 call abox(1, 2, 0, 0)
 call xlabel(3, 'x')
 call ylabel(3, 'y')
 call setcolor(2)
 call connect(x, y, 50)
 call mgoclose

end program test_mongo_basic
