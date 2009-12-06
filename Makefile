CWD = $(shell pwd)

MAKE = make -s

DIR_BIN = $(CWD)/bin
DIR_CONTENT = $(DIR_BIN)/content/kernel4
DIR_OBJECT = $(CWD)/obj
DIR_RESOURCES = $(CWD)/res
DIR_SOURCE = $(CWD)/src

OBJECTS_ROCKET = $(addprefix $(DIR_OBJECT)/,$(patsubst %.asm,%_32_asm.o,$(patsubst %.c,%_32_c.o,$(patsubst $(DIR_SOURCE)/%,%,$(shell find $(DIR_SOURCE)/rocket -iregex ".*\.c" -or -iregex ".*\.asm")))))
OBJECTS_KERNEL4 = $(addprefix $(DIR_OBJECT)/,$(patsubst %.asm,%_asm.o,$(patsubst %.c,%_c.o,$(patsubst $(DIR_SOURCE)/%,%,$(shell find $(DIR_SOURCE)/kernel4 -iregex ".*\.c" -or -iregex ".*\.asm")))))
RESOURCES_FUEL = $(shell find $(DIR_RESOURCES))

.PHONY: all rocket kernel fuel libnuke image clean clean-little do-the-real-cleaning do-the-real-cleaning-little

CC = gcc
CPP = g++
ASM = nasm
LD = ld -n
AR = ar -rcs
CFLAGS = -g -c -Wall -Wextra -Werror -pedantic -std=gnu99 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-leading-underscore -O3
CPPFLAGS = -g -c -Wall -Wextra -Werror -pedantic -std=gnu++89 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-exceptions -O3 -m64

all: $(DIR_CONTENT) clean-little rocket libnuke kernel image

$(DIR_CONTENT):
	@mkdir -p $(DIR_CONTENT)

$(DIR_CONTENT)/rocket.nbl: $(OBJECTS_ROCKET)
	@echo 'LD       $(patsubst $(DIR_BIN)/%,%,$@)'
	@$(LD) -T src/rocket/link.ld $(OBJECTS_ROCKET) -o $@

$(DIR_CONTENT)/nuke.nke: $(DIR_BIN)/libnuke.a $(OBJECTS_KERNEL4)
	@echo 'LD       $(patsubst $(DIR_BIN)/%,%,$@)'
	@$(LD) -T src/kernel4/link.ld $(OBJECTS_KERNEL4) $(CWD)/bin/libnuke.a -o $@

$(DIR_CONTENT)/fuel.nsi: $(DIR_BIN)/libnuke.a $(RESOURCES_FUEL)
	@echo '-------- Creating startup image'
	@tar cvf $@ $(RESOURCES_FUEL)

$(DIR_BIN)/libnuke.a: $(OBJECTS_LIBNUKE)
	@echo 'AR       $(patsubst $(DIR_BIN)/%,%,$@)'
	@$(AR) $@ $(OBJECTS_LIBNUKE)

$(DIR_BIN)/fd.img: $(DIR_CONTENT) $(DIR_CONTENT)/rocket.nbl $(DIR_BIN)/libnuke.a $(DIR_CONTENT)/nuke.nke
	@echo "-------- Creating image"
	@$(DIR_BIN)/scripts/make_floppy.sh $(DIR_BIN) amd64

$(DIR_OBJECT)/%_c.o: $(DIR_SOURCE)/%.c
	@mkdir -p $(dir $@)
	@echo 'CC       $<'
	@$(CC) $(CFLAGS) -m64 $< -o $@

$(DIR_OBJECT)/%_32_c.o: $(DIR_SOURCE)/%.c
	@mkdir -pv $(dir $@)
	@echo 'CC 32    $<'
	@$(CC) $(CFLAGS) -m32 $< -o $@

$(DIR_OBJECT)/%_32_asm.o: $(DIR_SOURCE)/%.asm
	@mkdir -p $(dir $@)
	@echo 'ASM 32   $<'
	@$(ASM) -f elf32 $< -o $@

$(DIR_OBJECT)/%_asm.o: $(DIR_SOURCE)/%.asm
	@mkdir -p $(dir $@)
	@echo 'ASM      $<'
	@$(ASM) -f elf64 $< -o $@

$(DIR_OBJECT)/%_cpp.o: $(DIR_SOURCE)/%.cpp
	@mkdir -p $(dir $@)
	@echo 'CPP      $<'
	@$(CPP) $(CPPFLAGS) $< -o $@

rocket: $(DIR_CONTENT)
	@echo '-------- $@'
	@$(MAKE) $(DIR_CONTENT)/rocket.nbl

kernel: $(DIR_CONTENT)
	@echo '-------- $@'
	@$(MAKE) $(DIR_CONTENT)/nuke.nke

fuel: $(DIR_CONTENT)
	@echo '-------- $@'
	@$(MAKE) $(DIR_CONTENT)/fuel.nsi

libnuke:
	@echo '-------- $@'
	@$(MAKE) $(DIR_BIN)/libnuke.a

image: $(DIR_CONTENT) rocket libnuke kernel
	@$(MAKE) $(DIR_BIN)/fd.img

clean:
	@echo '-------- Cleaning'
	@$(MAKE) do-the-real-cleaning-little do-the-real-cleaning

do-the-real-cleaning:
	@-rm -R $(DIR_OBJECT)/* 2> /dev/null

clean-little:
	@$(MAKE) do-the-real-cleaning-little

do-the-real-cleaning-little:
	@-rm -R $(DIR_CONTENT)/* $(DIR_BIN)/libnuke.a $(DIR_BIN)/fd.img 2> /dev/null
