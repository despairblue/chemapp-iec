
# Sample Makefile for the compilation of C programs using ChemApp
 
#LD= g77
LD= g++
EXTRACFLAGS= -DUNIX
EXTRACLIBS= -lf2c -lm
EXTRALDFLAGS = -lg2c
EXTRALDLIBS =

LIBS = lib/libChemAppC.a lib/libLChemApp.a

# This examples Makefile uses the STATIC version of ChemApp. If your
# distribution also contains the SHARED version of the ChemApp
# libraries, you can also link them dynamically. In this case,
# uncomment the "LIBS"-line below instead. 
# Make sure though that the environment variable LD_LIBRARY_PATH
# contains the directory where you choose to put the ChemApp shared
# libs, otherwise your application program won't be able to find them!
# For instance, for you to be able to run the example program cademo1
# from this directory, LD_LIBRARY_PATH needs to contain the path ".."!

#LIBS = ../libChemAppCS.so ../libLChemAppS.so


TARGET	= iec
OBJECTS = $(TARGET).o iter.o helpers.o

default: $(TARGET)

all: $(TARGET) test clean

%.o: %.c 
	@echo "Erstelle $@"
	$(CC) -I. $(CFLAGS) $(EXTRACFLAGS) -c $<

$(TARGET): $(OBJECTS)
	$(LD) $(EXTRALDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

test: ./$(TARGET)
	clear
	./$(TARGET)

clean:
	-rm *.o $(TARGET)



