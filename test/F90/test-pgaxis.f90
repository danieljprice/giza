program test_pgaxis
 implicit none
 real :: x1,y1,x2,y2,v1,v2,step,nsub,dmajl,dmajr,fmin,disp,orient

 call pgbeg(0,'/xw',1,1)
 call pgenv(0.,1.,0.,1.,0,0)
 x1 = 0.1
 y1 = 0.1
 x2 = 0.9
 y2 = 0.9
 v1 = 0.
 v2 = 11.
 step = 0.
 nsub = 10
 dmajl = 1.
 dmajr = 1.
 fmin = 0.5
 disp = 0.
 orient = 90.
 call pgaxis("N",x1,y1,x2,y2,v1,v2,step,nsub,dmajl,dmajr,fmin,disp,orient)
 call pgend

end program test_pgaxis
