#give your OS on the command line :
#make linux
#make mac
#make windows

#OS by default :
OS = LINUX

ifeq ($(OS), WIN32)
	GCC = cl.exe /c /EHsc
else
	GCC = g++ -c -Wall -fPIC
endif

.PHONY: linux
linux:
	make all OS=LINUX
 
.PHONY: windows
windows:
	make all OS=WIN32
        
.PHONY: mac
mac:
	make all OS=MAC


all: libDeviceManager

libDeviceManager: PluginFactories.obj Device.obj xmlParser.obj DeviceManager.obj
ifeq ($(OS), WIN32) 
	lib.exe /out:DeviceManager.lib PluginFactories.obj Device.obj xmlParser.obj DeviceManager.obj
	#rm -f *~ *.obj
else 
	ar -cr libDeviceManager.a PluginFactories.o Device.o xmlParser.o DeviceManager.o
	#rm -f *~ *.o
endif


PluginFactories.obj: PluginFactories.cpp PluginFactories.h 
ifeq ($(OS), LINUX)
	$(GCC) -fPIC PluginFactories.cpp
else
	$(GCC) PluginFactories.cpp 
endif

Device.obj: Device.cpp Device.h
	$(GCC) Device.cpp

xmlParser.obj: xmlParser.cpp xmlParser.h
	$(GCC) xmlParser.cpp

DeviceManager.obj: DeviceManager.cpp DeviceManager.h
	$(GCC) DeviceManager.cpp


clean:
	rm -f *~ *.o libDeviceManager.a *.obj DeviceManager.lib