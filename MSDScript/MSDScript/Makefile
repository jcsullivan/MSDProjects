INCS = parse.hpp expression.hpp value.hpp environment.hpp step.hpp cont.hpp pointer.hpp catch.hpp
LIBOBJS = stub.o parse.o expression.o value.o environment.o step.o cont.o
OBJS = main.o $(LIBOBJS)

CXX = c++
CXXFLAGS = --std=c++17 -O2

msdscript: $(OBJS)
	$(CXX) $(CXXFLAGS) -o MSDScript $(OBJS)
	
libmsdscript.a: $(LIBOBJS)
	$(AR) $(ARFLAGS) libmsdscript.a $(LIBOBJS)
	
main.o: $(INCS)

stub.o: $(INCS)

parse.o: $(INCS)

expression.o: $(INCS)

value.o: $(INCS)

environment.o: $(INCS)

step.o: $(INCS)

cont.o: $(INCS)
