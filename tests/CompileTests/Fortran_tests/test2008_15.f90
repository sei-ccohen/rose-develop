module m2
  integer i
  contains
    subroutine ffff(i,pppp)
      integer i
      integer pppp(i)
       integer x
       x = i+2
    end subroutine ffff
end module m2

subroutine u1
 use m2
 integer,parameter::cc=12
 integer,allocatable::a(:)
 allocate(a(cc))
 call ffff(cc,a)
end subroutine u1
