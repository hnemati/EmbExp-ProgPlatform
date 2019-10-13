# cross compiler
# ---------------------------------
-include Makefile.local
ifndef EMBEXP_CROSS
  EMBEXP_CROSS	= aarch64-linux-gnu-
endif
ifndef EMBEXP_GDB
  EMBEXP_GDB    = ${EMBEXP_CROSS}gdb
endif

CROSS = ${EMBEXP_CROSS}
GDB   = ${EMBEXP_GDB}

include Makefile.config

# settings
# ---------------------------------
NAME	= output/example-program.elf
NAME_DA	= output/example-program.da
SFLAGS  = -Iinc
CFLAGS	= -ggdb3 -std=gnu99 -Wall -fno-builtin -Iinc
LDFLAGS = -Bstatic -nostartfiles -nostdlib

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))



# source definition
# ---------------------------------
SOURCES_C     = $(call rwildcard, src/, *.c)
SOURCES_S     = $(call rwildcard, src/, *.S)
INCLUDE_FILES = $(call rwildcard, inc/, *.h) inc/config_input.h

OBJECTS     = $(SOURCES_C:.c=.o) $(SOURCES_S:.S=.o)




# compilation and linking
# ---------------------------------
all: $(NAME)

inc/config_input.h: Makefile.config
	./scripts/gen_config_input.py

%.o: %.S ${INCLUDE_FILES}
	${CROSS}as ${SFLAGS} -o $@ $<

%.o: %.c ${INCLUDE_FILES}
	${CROSS}gcc ${CFLAGS} -c -o $@ -fno-stack-protector $<

$(NAME): ${OBJECTS} ${SOURCES_C} ${SOURCES_S} ${INCLUDE_FILES}
	mkdir -p ./output
	${CROSS}ld $(LDFLAGS) -o $@ -T linkerscripts/$(PROGPLAT_BOARD).ld ${OBJECTS}
	${CROSS}objdump -t -h -D $@ > "$@_da"

clean:
	rm -rf output
	rm -f $(call rwildcard, src/, *.o) inc/config_input.h




# targets for running and debugging
# ---------------------------------
ifndef EMBEXP_INSTANCE_IDX
  export EMBEXP_INSTANCE_IDX=0
endif
export EMBEXP_UART_PORT=$(shell bash -c "echo $$(( 20000 + ($(EMBEXP_INSTANCE_IDX) * 100) ))")
export EMBEXP_GDBS_PORT=$(shell bash -c "echo $$(( 20013 + ($(EMBEXP_INSTANCE_IDX) * 100) ))")
connect:
	../EmbExp-Box/interface/remote.py $(PROGPLAT_BOARD) -idx $(EMBEXP_INSTANCE_IDX)

checkready:
	./scripts/check_ready.sh

checkclosed:
	./scripts/check_closed.sh

# Ctrl+] mode character
uart:
	telnet localhost $(EMBEXP_UART_PORT)

log:
	@nc localhost $(EMBEXP_UART_PORT)

run: $(NAME)
	${GDB} --eval-command="target remote localhost:$(EMBEXP_GDBS_PORT)" -x scripts/run.gdb $(NAME)

runlog: $(NAME)
	./scripts/run_only.py ${GDB} "localhost:$(EMBEXP_GDBS_PORT)" $(NAME)
	@echo "======================="
	@echo "======================="
	@cat temp/uart.log

runlog_reset: $(NAME)
	./scripts/connect_and_run.py

runlog_try: $(NAME)
	./scripts/try_run_only.py

cleanuart:
	make clean && make runlog && cat temp/uart.log


.PHONY: all clean
.PHONY: connect checkready uart run log runlog runlog_reset cleanuart

