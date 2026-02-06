# Nuke built-in rules and variables.
override MAKEFLAGS += -rR

# This is the name that our final executable will have.
override OUTPUT := test

# Convenience macro to reliably declare user overridable variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# User controllable C compiler command.
override DEFAULT_KCC := cc
$(eval $(call DEFAULT_VAR,KCC,$(DEFAULT_KCC)))

# User controllable linker command.
override DEFAULT_KLD := cc
$(eval $(call DEFAULT_VAR,KLD,$(DEFAULT_KLD)))

# User controllable C flags.
override DEFAULT_KCFLAGS := -Wall -Wextra -g -Wpedantic
$(eval $(call DEFAULT_VAR,KCFLAGS,$(DEFAULT_KCFLAGS)))

# User controllable C preprocessor flags.
override DEFAULT_KCPPFLAGS :=
$(eval $(call DEFAULT_VAR,KCPPFLAGS,$(DEFAULT_KCPPFLAGS)))

# User controllable nasm flags.
override DEFAULT_KNASMFLAGS := -g
$(eval $(call DEFAULT_VAR,KNASMFLAGS,$(DEFAULT_KNASMFLAGS)))

# User controllable linker flags - ADD YOUR LIBRARY HERE
override DEFAULT_KLDFLAGS := -L../bin -ljes -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm
$(eval $(call DEFAULT_VAR,KLDFLAGS,$(DEFAULT_KLDFLAGS)))

# Internal C flags that should not be changed by the user.
override KCFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -m64 \
    -march=native

# Internal C preprocessor flags - ADD LIBRARY HEADER PATH
override KCPPFLAGS := \
    -I examples \
    -I ../src \
    $(KCPPFLAGS)

# Internal nasm flags that should not be changed by the user.
override KNASMFLAGS += \
    -Wall \
    -f elf64

# Internal linker flags that should not be changed by the user.
override KLDFLAGS += \
    -m64 \
    -lgcc

# Use "find" to glob all *.c, *.S, and *.asm files in examples directory
override CFILES := $(shell find src -type f -name '*.c' | LC_ALL=C sort)
override ASFILES := $(shell find src -type f -name '*.S' | LC_ALL=C sort)
override NASMFILES := $(shell find src -type f -name '*.asm' | LC_ALL=C sort)
override OBJ := $(addprefix obj/,$(notdir $(CFILES:.c=.c.o)) $(notdir $(ASFILES:.S=.S.o)) $(notdir $(NASMFILES:.asm=.asm.o)))
override HEADER_DEPS := $(addprefix obj/,$(notdir $(CFILES:.c=.c.d)) $(notdir $(ASFILES:.S=.S.d)))

# Default target.
.PHONY: all
all: bin/$(OUTPUT)

# Link rules for the final executable.
bin/$(OUTPUT): $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(KLD) $(OBJ) $(KLDFLAGS) -o $@

# Include header dependencies.
-include $(HEADER_DEPS)

# Compilation rules for *.c files.
obj/%.c.o: src/%.c
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -MMD -c $< -o $@

# Compilation rules for *.s (gas) files.
obj/%.S.o: src/%.S
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@

# Compilation rules for *.asm (nasm) files.
obj/%.asm.o: src/%.asm
	mkdir -p "$$(dirname $@)"
	nasm $(KNASMFLAGS) $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf bin obj