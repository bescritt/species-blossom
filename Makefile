# Makefile for Species Blossom

CC = gcc
CPP = g++
LD = g++

DEBUG = -g3 -DBLOSSOM_DEBUG=1 # -DDEBUG=3
OPTIMISATIONS = -O2

# default flags
CFLAGS = ${DEBUG} ${OPTIMISATIONS} 
CPPFLAGS = ${CFLAGS}
LDFLAGS = ${DEBUG}
# (wxWidgets is NOT used by this build; the wx-config lines were dead
#  and have been removed. The evolver only needs gcc/g++/make/gnuplot.)

# Martin Ankerl's highly tuned settings for exMars
#CFLAGS = -O3 -fomit-frame-pointer -fforce-addr -funroll-all-loops -fstrict-aliasing -malign-double -fprefetch-loop-arrays -maccumulate-outgoing-args -minline-all-stringops -finline-functions -finline-limit=800 -ffast-math -mno-align-stringops
#CFLAGS += -mfpmath=sse,387 -mmmx -msse -march=pentium3

# Martin Ankerl's develomental flags for exMars
#CFLAGS += -fprofile-arcs -ftest-coverage
#CFLAGS = -O0 -ansi -pedantic -Wall -W -Wno-div-by-zero -Wsystem-headers -Wfloat-equal -Wundef -Wendif-labels -Wshadow -Wbad-function-cast -Wcast-qual -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Wunreachable-code -Winline -Wlong-long

#extensions
EXE_EXT =
DLL_EXT = .so

#Exhaust files
EXHAUST_PATH=exhaust-1.9.2/
EXHAUST = \
	${EXHAUST_PATH}pspace.o \
	${EXHAUST_PATH}sim.o \
	${EXHAUST_PATH}asm.o

#blossom files
BLOSSOM = \
	warrior.opp \
	blossom_utils.opp \
	world.opp \
	benchmark.opp \
	reproduction.opp \
	insn_markov.opp \
	rand.opp \
	mersenne.opp \
	exhaust.opp \
	battle.opp \
	population.opp \
	blossom.opp

#files borrowed from other places
OTHER = \
	ui_gnuplot.o

all: blossom${EXE_EXT}

TESTS = test/test_mersenne

.PHONY: unit-tests

unit-tests: ${TESTS}
	@for t in $(TESTS); do \
		printf "Running $$t... "; \
		./$$t || exit 1; \
	done



blossom${EXE_EXT}:	${EXHAUST} ${BLOSSOM} ${OTHER}
	${LD} ${CPPFLAGS} -o $@ ${EXHAUST} ${BLOSSOM} ${OTHER} ${LDFLAGS}

# compile c files
	
%.o:	%.c
	${CC} ${CFLAGS} -c $< -MD -MF $(<:%.c=%.dep) -o $@
		
# compile c++ files
	
%.opp:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -MD -MF $(<:%.cpp=%.dep) -o $@

# test binaries

test/test_mersenne: test/test_mersenne.cpp mersenne.opp
	${CPP} ${CPPFLAGS} -o $@ test/test_mersenne.cpp mersenne.opp

#misc

debug:	blossom${EXE_EXT}
	gdb ./blossom${EXE_EXT}
	
run:	blossom${EXE_EXT}
	#clear
	./blossom${EXE_EXT}

clean:
	rm -f blossom${EXE_EXT}
	rm -f *.[hc]pp~ Makefile~ ${EXHAUST} ${BLOSSOM} ${OTHER} core
	rm -f $(EXHAUST:%.o=%.dep) $(BLOSSOM:%.opp=%.dep) $(OTHER:%.o=%.dep)
	rm -f Pond.dat *.dat
	
-include $(EXHAUST:%.o=%.dep) $(BLOSSOM:%.opp=%.dep) $(OTHER:%.o=%.dep)

.PHONY: test test-integration

test: unit-tests test-integration

coverage: COVERAGE_DIR=coverage
coverage:
	@echo "Building with coverage flags..."
	$(MAKE) clean
	CFLAGS="-g -O0 -fprofile-arcs -ftest-coverage" CPPFLAGS="-g -O0 -fprofile-arcs -ftest-coverage" LDFLAGS="-g -O0 -fprofile-arcs -ftest-coverage" make -j2 blossom${EXE_EXT}
	# build mersenne test with coverage
	# compile mersenne with coverage
	${CPP} -g -O0 -fprofile-arcs -ftest-coverage -c mersenne.cpp -o mersenne_cov.o || true
	${CPP} -g -O0 -fprofile-arcs -ftest-coverage -o test/test_mersenne_cov test/test_mersenne.cpp mersenne_cov.o || true
	./test/test_mersenne_cov || true
	mkdir -p $(COVERAGE_DIR)
	gcov -o . mersenne.cpp > $(COVERAGE_DIR)/mersenne.gcov || true
	@echo "Coverage artifacts in $(COVERAGE_DIR)"


# Integration: run blossom headless briefly and verify it starts
test-integration:
	chmod +x test/run_integration.sh
	./test/run_integration.sh

