#
# build matlab functions using matlab's mex tool
#
# issues:
#	* MEX automatically appends the arch dependent suffix (.mexglx, .mexa64, etc.)
#	  How can we make the Makefile aware of the correct suffix? (Hardcoded for now)
#
#	* The libGDF binaries are put where the user chooses to build the library with cmake.
#	  Putting the binaries into a specific location inside the source tree would probably
#	  be the easiest solution, but is not pretty. (For now the search path is hardcoded to
#	  ../build)
#
#	* Location of matlab installation: How can this be determined?
#


SOURCEDIR = .
OUTDIR = .
OBJDIR = build

MEXSUFFIX = mexa64

GDF_INCLUDE_DIR = -I../libgdf/include
GDF_LIB_DIR = -L../build/libgdf
GDF_LIBS = -lGDF

MATLABROOT = /opt/matlab/R2010b
MATLAB_INCLUDE_DIR = -I$(MATLABROOT)/extern/include

MEXTOOL = $(MATLABROOT)/bin/mex

CXXFLAGS = -c -cxx -O -DNDEBUG
LDFLAGS = 

all: $(OUTDIR)/gdf_reader.$(MEXSUFFIX) $(OUTDIR)/gdf_writer.$(MEXSUFFIX)

clean:
	rm -rf $(OBJDIR) $(OUTDIR)/gdf_reader.$(MEXSUFFIX) $(OUTDIR)/gdf_writer.$(MEXSUFFIX)
	
buildclean:
	rm -rf $(OBJDIR)
	
$(OUTDIR)/gdf_reader.$(MEXSUFFIX): $(OBJDIR)/gdf_reader.o
	$(MEXTOOL) -cxx $(LDFLAGS) $(GDF_LIB_DIR) $(GDF_LIBS) -outdir $(OUTDIR) -output $@ $^
	
$(OUTDIR)/gdf_writer.$(MEXSUFFIX): $(OBJDIR)/gdf_writer.o
	$(MEXTOOL) -cxx $(LDFLAGS) $(GDF_LIB_DIR) $(GDF_LIBS) -outdir $(OUTDIR) -output $@ $^
	
$(OBJDIR)/gdf_reader.o: $(SOURCEDIR)/gdf_reader.cpp
	$(MEXTOOL) $(CXXFLAGS) $(MATLAB_INCLUDE_DIR) $(GDF_INCLUDE_DIR) -outdir $(OBJDIR) $^

$(OBJDIR)/gdf_writer.o: $(SOURCEDIR)/gdf_writer.cpp
	$(MEXTOOL) $(CXXFLAGS) $(MATLAB_INCLUDE_DIR) $(GDF_INCLUDE_DIR) -outdir $(OBJDIR) $^
