target = Alloc.o Handle_grid.o Calc_TF.o Calc_potential.o Calc_differential.o Fit_potential.o Utils.o
compileOption = 

build: $(target)
	ar rv libaecdft.a $(target)

.cpp.o:
	$(CXX) $(compileOption) -c $<