program testgizaplot
 use giza
 implicit none
 integer, parameter :: n = 100
 real, parameter :: pi = 3.14159236
 real, dimension(n) :: xplot,yplot
 real, dimension(n,n) :: arr
 integer :: i,j
 real :: yi
 
 do i=1,n
    xplot(i) = (i-1)/real(n-1)
    yplot(i) = sin(2.*pi*xplot(i))
    do j=1,n
       yi = (j-1)/real(n-1)
       arr(i,j) = sin(2.*pi*yi)
    enddo
 enddo

! call giza_plot(yplot,ylabel='sin(x)',xlabel='x',&
!                title='test',ls=giza_ls_dot,ci=2,lw=5.,ch=2.,symbol=3,&
!                font='arial',printid=.true.,just=1)
 call giza_plot(img=arr,dev='giza-test.png')

 !call giza_plot(xplot,yplot,dev='/png',xmin=0.5,xmax=1.0)

end program testgizaplot
