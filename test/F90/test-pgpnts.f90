program test_pgpnts
! Test PGPNTS (per-point markers), PGSCRN (color by name),
! PGQNDT/PGQDT (device queries)
 implicit none
 integer :: n, ndt, i, ier, tlen, dlen, inter
 real :: x(10), y(10)
 integer :: symbols(10)
 character(len=64) :: dtype, ddescr

 ! Query device types
 call pgqndt(ndt)
 write(*,*) 'Number of device types:', ndt
 do i = 1, ndt
    call pgqdt(i, dtype, tlen, ddescr, dlen, inter)
    write(*,'(A,I2,A,A,A,A,A,I1)') '  Device ', i, ': type=', &
         trim(dtype), ' descr=', trim(ddescr), ' inter=', inter
 enddo

 call pgbeg(0, 'test-pgpnts.png/png', 1, 1)
 call pgenv(0.0, 11.0, 0.0, 4.0, 0, 0)
 call pglab('X', 'Y', 'PGPNTS + PGSCRN test')

 n = 10
 do i = 1, n
    x(i) = real(i)
    y(i) = 2.0
    symbols(i) = i
 enddo

 ! Test PGSCRN with known color
 call pgscrn(1, 'red', ier)
 write(*,*) "PGSCRN('red'): ier=", ier, ' (expect 0)'
 call pgsci(1)
 call pgpnts(n, x, y, symbols, n)

 ! Blue row
 call pgscrn(1, 'blue', ier)
 write(*,*) "PGSCRN('blue'): ier=", ier, ' (expect 0)'
 do i = 1, n
    y(i) = 3.0
 enddo
 call pgpnts(n, x, y, symbols, n)

 ! Green row
 call pgscrn(1, 'green', ier)
 write(*,*) "PGSCRN('green'): ier=", ier, ' (expect 0)'
 do i = 1, n
    y(i) = 1.0
 enddo
 call pgpnts(n, x, y, symbols, n)

 ! Unknown color
 call pgscrn(1, 'nosuchcolor', ier)
 write(*,*) "PGSCRN('nosuchcolor'): ier=", ier, ' (expect 1)'

 call pgend

end program test_pgpnts
