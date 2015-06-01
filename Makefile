AR=ar
AS=as
CC=gcc
CPP=cpp
CXX=g++
LD=ld
OBJCOPY=objcopy
OBJDUMP=objdump
STRIP=strip

OBJDIR=.
BINDIR=.
APPDIR=./apps

OBJS=$(OBJDIR)/Machine.o \
     $(OBJDIR)/VirtualMachineUtils.o \
     $(OBJDIR)/VirtualMachineMemory.o \
     $(OBJDIR)/VirtualMachineFileSystem.o \
     $(OBJDIR)/VirtualMachine.o \
     $(OBJDIR)/main.o
     
     
DEBUG_MODE=TRUE
UNAME := $(shell uname)

ifdef DEBUG_MODE
DEFINES += -DDEBUG
endif

INCLUDES += -I. 
LIBRARIES = -ldl

CFLAGS += -Wall $(INCLUDES) $(DEFINES)
CPPFLAGS+= -std=c++11 -Wno-unused-variable -Wno-unused-but-set-variable
APPCFLAGS += -Wall -fPIC $(INCLUDES) $(DEFINES)

ifdef DEBUG_MODE
CFLAGS += -g -ggdb
CPPFLAGS += -g -ggdb
APPCFLAGS += -g -ggdb
else
CFLAGS += -O3
APPCFLAGS += -O3
endif

ifeq ($(UNAME), Darwin)
LDFLAGS = $(DEFINES) $(INCLUDES) $(LIBRARIES) 
APPLDFLAGS += $(DEFINES) $(INCLUDES) -shared -rdynamic -flat_namespace -undefined suppress
else
LDFLAGS = $(DEFINES) $(INCLUDES) $(LIBRARIES) -Wl,-E
APPLDFLAGS += $(DEFINES) $(INCLUDES) -shared -rdynamic -Wl,-E
endif

all: $(BINDIR)/vm 
apps: $(BINDIR)/hello.so $(BINDIR)/sleep.so $(BINDIR)/file.so $(BINDIR)/thread.so $(BINDIR)/mutex.so $(BINDIR)/memory.so $(BINDIR)/shell.so 

$(BINDIR)/vm: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINDIR)/vm
	
FORCE: ;

#
# use gmake's implicit rules for now, except this one:
#
$(BINDIR)/%.so: $(APPDIR)/%.o
	$(CC) $(APPLDFLAGS) $< -o $@

$(APPDIR)/%.o : $(APPDIR)/%.c
	$(CC) -c $(APPCFLAGS) $< -o $@

$(APPDIR)/%.o : $(APPDIR)/%.cpp 
	$(CXX) -c $(APPCFLAGS) $(CPPFLAGS) $< -o $@
	
$(OBJDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp 
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@


	
clean::
	-rm $(OBJDIR)/*.o 
	-rm $(APPDIR)/*.o    
	
.PHONY: clean
