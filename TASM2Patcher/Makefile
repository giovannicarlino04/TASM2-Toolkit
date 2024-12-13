CC := gcc
CXX := g++

OUTPUT := tasm2patcher.dll
OUTPUT_COPY := D:/Games/The\ Amazing\ Spider-Man\ 2/

OBJS := main.o
CFLAGS := -Wall -I. -I./ -std=gnu99 -mms-bitfields -s -O2 -masm=intel -shared -Wl,--subsystem,windows,--kill-at,--enable-stdcall-fixup -DNO_ZLIB -fpermissive
CPPFLAGS := -Wall -O2 -mno-ms-bitfields -std=c++17 -DTIXML_USE_STL -DNO_ZLIB
CXXFLAGS := -Wall -Wno-strict-aliasing -I./ -O2 -std=c++17 -mms-bitfields -DTIXML_USE_STL -fpermissive
LDFLAGS := -static 
LIBS := -lxinput1_3 -lstdc++ -lversion -lpthread -lMinHook

.PHONY: all clean

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) *.o

$(OUTPUT): $(OBJS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	cp $(OUTPUT) $(OUTPUT_COPY)