.DEFAULT_GOAL := all

MPICC = mpiCC
MPICFLAGS = -std=c++11
MPICOPTFLAGS = -O3 -g -lpng
MPILDFLAGS =

TARGETS = mandelbrot_serial$(EXEEXT) mandelbrot_mpi$(EXEEXT)

all: $(TARGETS)

SRCS_COMMON = render.cc 

DISTFILES += $(SRCS_COMMON) $(DEPS_COMMON)

mandelbrot_mpi_block$(EXEEXT): mandelbrot_mpi_block.cc $(SRCS_COMMON) $(DEPS_COMMON)
	$(MPICC) $(MPICFLAGS) $(MPICOPTFLAGS) -I/data/apps/boost/1.57/include \
	    -o $@ mandelbrot_mpi_block.cc $(SRCS_COMMON) $(MPILDFLAGS)

mandelbrot_mpi_cyclic$(EXEEXT): mandelbrot_mpi_cyclic.cc $(SRCS_COMMON) $(DEPS_COMMON)
	$(MPICC) $(MPICFLAGS) $(MPICOPTFLAGS) -I/data/apps/boost/1.57/include \
	    -o $@ mandelbrot_mpi_cyclic.cc $(SRCS_COMMON) $(MPILDFLAGS)


mandelbrot_mpi_master_slave$(EXEEXT): mandelbrot_mpi_master_slave.cc $(SRCS_COMMON) $(DEPS_COMMON)
	     $(MPICC) $(MPICFLAGS) $(MPICOPTFLAGS) -I/data/apps/boost/1.57/include \
		 -o $@ mandelbrot_mpi_master_slave.cc $(SRCS_COMMON) $(MPILDFLAGS)
								   
clean:
	rm -f $(TARGETS) 

# eof
