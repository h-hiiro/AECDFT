target = Alloc.o Fill_grid.o
compileOption = 

build: $(target)
	ar rv libaecdft.a $(target)

.cpp.o:
	$(CXX) $(compileOption) -c $<