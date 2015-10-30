program testgizaplot
  use giza
  implicit none
  integer, parameter :: n = 100
  real, dimension(n) :: xplot,yplot
  real, dimension(n,n) :: arr
  integer :: i,j
  real :: r

  do i=1,n
     xplot(i)=(i-1)/real(n-1)*20.-10.
     yplot(i)=xplot(i)
  enddo
  do i=1,n
     do j=1,n
        r=sqrt(xplot(i)*xplot(i)+yplot(j)*yplot(j))
        if (r.ne.0.d0) then
           arr(i,j)=sin(r)/r
        else
           arr(i,j)=1.
        endif
     enddo
  enddo

  call giza_plot(img=arr,dev='giza-test-2D.png',just=1,&
!                width,height,units,just=1,axis=1,&
                 xmin=-10.,xmax=10.,ymin=-10.,ymax=10., &
!                imgmin=,imgmax,&
!                affine=(/1./real(n),0.,0.,1./real(n),0.,0./),&
!                vptxmin,vptxmax,vptymin,vptymax,&
                 xlabel='x',ylabel='y',&
                 title='sin(r)/r',extend=giza_extend_pad)
!                font,&
!                 ls=giza_ls_solid,lw=2.,ci=2,ch=1.)
!                symbol=3,
!                extend,printid=.true.)

end program testgizaplot
