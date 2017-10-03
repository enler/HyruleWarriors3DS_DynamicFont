ifeq ($(strip $(DEVKITARM)),)
$(error "请将DEVKITARM添加到环境变量")
endif

PREFIX = $(DEVKITARM)/bin/arm-none-eabi-

CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
CTR_INCLUDE = $(DEVKITPRO)/libctru/include

# define options for compilation
CFLAGS = -Wall -O2 -fno-builtin -march=armv6k -fshort-wchar -I$(CTR_INCLUDE)
CFLAGS += -Ilibctru

# define options for linkage. Prevent the inclusion of standard start
# code and libraries.
LDFLAGS =  -nostartfiles -nodefaultlibs -Wl,--use-blx -T ld.S

# define options for the objdump
DUMPFLAGS = -xdsS

# use variables to refer to init code in case it changes
OBJS = DrawTexture.o head.o main.o

#
# define build targets
#
all: payload_10.bin

VER11: payload_11.bin

VER16: payload_16.bin

clean:
	rm -f *.o *.elf *.bin *.dump


# build s-record with init code and c files linked together
payload_10.bin: $(OBJS) 
	$(LD) $(LDFLAGS) -T symbols/symbol.S -o payload_10.elf $(OBJS) $(LIBS)
	$(OBJDUMP) $(DUMPFLAGS) payload_10.elf > payload_10.dump
	$(OBJCOPY) -O binary payload_10.elf $@

payload_11.bin: $(OBJS)
	$(LD) $(LDFLAGS) -T symbols/symbol_11.S -o payload_11.elf $(OBJS) $(LIBS)
	$(OBJDUMP) $(DUMPFLAGS) payload_11.elf > payload_11.dump
	$(OBJCOPY) -O binary payload_11.elf $@

payload_16.bin: $(OBJS)
	$(LD) $(LDFLAGS) -T symbols/symbol_16.S -o payload_16.elf $(OBJS) $(LIBS)
	$(OBJDUMP) $(DUMPFLAGS) payload_16.elf > payload_16.dump
	$(OBJCOPY) -O binary payload_16.elf $@

payload_10.bin : CFLAGS+= -D=VER10
payload_11.bin : CFLAGS+= -D=VER11
payload_16.bin : CFLAGS+= -D=VER16

# handle compilation of C files

main.o : CFLAGS += -mthumb

%.o:%.S
	$(CC) -D__ASSEMBLY__ $(CFLAGS) -c $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


