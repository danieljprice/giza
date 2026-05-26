! Label/layout regression for libmgo (plotmongo title path).
program test_mongo_label
 implicit none
 real(kind=8) :: tlen, thgt, tabx, taby

 call device(5)
 call softphysical(0.d0, 0.d0, 800.d0, 600.d0, 0.d0, 0.d0, 0.d0, 0.d0)
 call setlim(0.d0, 0.d0, 1.d0, 1.d0)
 call margins(1.d0, 1.2d0, 1.3d0, 1.2d0)
 call setexpand(1.d0)
 call gstrlen(5, 'title', tlen, thgt)
 tabx = 400.d0
 taby = 500.d0
 call grelocate(tabx, taby)
 call putlabel(5, 'title', 9)
 call abox(1, 2, 0, -1)
 call mgoclose
end program test_mongo_label
