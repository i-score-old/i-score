#give your OS on the command line :
#make linux
#make macos

#OS by default :
OS = LINUX

GCC = g++
MAC_CFLAGS = -dynamiclib -undefined dynamic_lookup
MAC64_CFLAGS = -dynamiclib -undefined dynamic_lookup -DX86_64
LINUX_CFLAGS = -c -fPIC
LINUX64_CFLAGS = -c -fPIC -DX86_64
LINUX_CCFLAGS = -shared

OBJ_FILES =  Minuit/*.o NetworkCommunication/osc/*.o NetworkCommunication/ip/*.o ../../xmlParser.o ../../Device.o ../../DeviceManager.o ../../PluginFactories.o

OSC_LIB_NAME = OSC
MINUIT_LIB_NAME = Minuit
MAC_LIB_TYPE = dylib
LINUX_LIB_TYPE = so

.PHONY: linux
linux:
	make all OS=LINUX

.PHONY: linux64
linux64:
	make all OS=LINUX64

.PHONY: mac
mac:
	make all OS=MAC

all: 
ifeq ($(OS), MAC)
	$(GCC) $(MAC_CFLAGS) Minuit.cpp $(OBJ_FILES) -o $(MINUIT_LIB_NAME).$(MAC_LIB_TYPE)
	$(GCC) $(MAC_CFLAGS) OSC.cpp $(OBJ_FILES) -o $(OSC_LIB_NAME).$(MAC_LIB_TYPE)
	test -d ./Plugins || mkdir ./Plugins
	mv *.$(MAC_LIB_TYPE) ./Plugins
	rm -f *~ *.o *.$(MAC_LIB_TYPE)
endif
ifeq ($(OS), LINUX)
	$(GCC) $(LINUX_CFLAGS) Minuit.cpp
	$(GCC) $(LINUX_CCFLAGS) Minuit.o $(OBJ_FILES) -o $(MINUIT_LIB_NAME).$(LINUX_LIB_TYPE)
	$(GCC) $(LINUX_CFLAGS) OSC.cpp
	$(GCC) $(LINUX_CCFLAGS) OSC.o $(OBJ_FILES) -o $(OSC_LIB_NAME).$(LINUX_LIB_TYPE)
	test -d ./Plugins || mkdir ./Plugins
	mv *.$(LINUX_LIB_TYPE) ./Plugins
	rm -f *~ *.o *.$(LINUX_LIB_TYPE)
endif

ifeq ($(OS), LINUX64)
	$(GCC) $(LINUX64_CFLAGS) Minuit.cpp
	$(GCC) $(LINUX64_CCFLAGS) Minuit.o $(OBJ_FILES) -o $(MINUIT_LIB_NAME).$(LINUX_LIB_TYPE)
	$(GCC) $(LINUX64_CFLAGS) OSC.cpp
	$(GCC) $(LINUX64_CCFLAGS) OSC.o $(OBJ_FILES) -o $(OSC_LIB_NAME).$(LINUX_LIB_TYPE)
	test -d ./Plugins || mkdir ./Plugins
	mv *.$(LINUX_LIB_TYPE) ./Plugins
	rm -f *~ *.o *.$(LINUX_LIB_TYPE)
endif

clean:
	rm -f *~ *.o *.$(MAC_LIB_TYPE) *.$(LINUX_LIB_TYPE) Plugins/*.$(MAC_LIB_TYPE) Plugins/*.$(LINUX_LIB_TYPE)

