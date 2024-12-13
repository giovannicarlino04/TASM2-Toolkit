CC := gcc                            
CXX := g++                             

OUTPUT := TASM2Launcher                
OUTPUT_COPY := D:/Games/The\ Amazing\ Spider-Man\ 2/  # Path to copy the built executable to

OBJS := main.o                       
CFLAGS := -Wall -I. -I./ -std=gnu99 -mms-bitfields -s -O2 -masm=intel -static -Wl,--subsystem,windows,--kill-at,--enable-stdcall-fixup -DNO_ZLIB -fpermissive 
CPPFLAGS := -Wall -O2 -mno-ms-bitfields -std=c++17 -DTIXML_USE_STL -DNO_ZLIB  
CXXFLAGS := -Wall -Wno-strict-aliasing -I./ -O2 -std=c++17 -mms-bitfields -DTIXML_USE_STL -fpermissive  
LIBS := -lxinput1_3 -lstdc++ -lversion -lpthread -lMinHook  # Libraries to link against

.PHONY: all clean                     

# Default target: build the launcher executable
all: $(OUTPUT)

# Clean up object files and executable
clean:
	rm -f $(OUTPUT) *.o

# Build the final executable
$(OUTPUT): $(OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS)

# Copy the built executable to the game directory
	cp $(OUTPUT) $(OUTPUT_COPY)
