target = Alloc.o Handle_grid.o Calc_TF.o Calc_potential.o
compileOption = 

build: $(target)
	ar rv libaecdft.a $(target)

.cpp.o:
	$(CXX) $(compileOption) -c $<