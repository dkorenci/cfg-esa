COMPILER = g++
CF ?= -O3

# make NOSHELL=1 to run code other than shell command code in main()
ifdef NOSHELL
SHELL_FLAG = 
else 
SHELL_FLAG = -DSHELL
endif

# to compile with debug use make CF="-O2 -g", effect is: CF = -O2 -g
FLAGS = $(CF) $(SHELL_FLAG) -I src/ -Wall -Wno-parentheses -Wno-char-subscripts -Wno-sign-compare
#FLAGS = -O2 -I src/ -Wall -Wno-parentheses -Wno-char-subscripts 

LDFLAGS = $(CF)
LDLIBS =

EXEC = main
OBJD = obj
SRCD = src
OBJS =  $(OBJD)/main.o $(OBJD)/suffix.o $(OBJD)/lcptree.o $(OBJD)/lfirstcomp.o \
	$(OBJD)/radix.o $(OBJD)/grammar.o $(OBJD)/test.o $(OBJD)/etimer.o \
	$(OBJD)/fsort.o


release: $(OBJD) $(OBJS)
	$(COMPILER) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LDLIBS)
	rm -f cfg_esa
	mv main cfg_esa

clean:
	rm -rf obj
	rm -f cfg_esa
	
$(OBJD):
	mkdir -p $@
	
$(OBJD)/main.o : $(SRCD)/main.cpp
	$(COMPILER) $(FLAGS) -o $(OBJD)/main.o -c $(SRCD)/main.cpp
	
$(OBJD)/suffix.o : $(SRCD)/suffix/SuffixStructCreator.cpp \
	$(SRCD)/suffix/SuffixStructCreator.h $(SRCD)/suffix/SaIsCreator.hpp
	$(COMPILER) $(FLAGS) -o $(OBJD)/suffix.o -c $(SRCD)/suffix/SuffixStructCreator.cpp

$(OBJD)/lcptree.o : $(SRCD)/suffix/LcpTreeCreator.cpp \
	$(SRCD)/suffix/LcpTreeCreator.h
	$(COMPILER) $(FLAGS) -o $(OBJD)/lcptree.o -c $(SRCD)/suffix/LcpTreeCreator.cpp			
	
$(OBJD)/lfirstcomp.o : $(SRCD)/compress/LongestFirstSaCompressor.cpp \
	$(SRCD)/compress/LongestFirstSaCompressor.h $(SRCD)/compress/radix_sort.cpp \
	$(SRCD)/compress/radix_sort.h $(SRCD)/compress/CfGrammar.cpp $(SRCD)/compress/CfGrammar.h \
	$(SRCD)/suffix/LcpTreeCreator.cpp $(SRCD)/suffix/LcpTreeCreator.h \
	$(SRCD)/suffix/SuffixStructCreator.cpp $(SRCD)/suffix/SuffixStructCreator.h \
	$(SRCD)/suffix/SaIsCreator.hpp
	$(COMPILER) $(FLAGS) -o $(OBJD)/lfirstcomp.o -c $(SRCD)/compress/LongestFirstSaCompressor.cpp			

$(OBJD)/radix.o : $(SRCD)/compress/radix_sort.cpp
	$(COMPILER) $(FLAGS) -o $(OBJD)/radix.o -c $(SRCD)/compress/radix_sort.cpp			
	
$(OBJD)/grammar.o : $(SRCD)/compress/CfGrammar.cpp $(SRCD)/compress/CfGrammar.h
	$(COMPILER) $(FLAGS) -o $(OBJD)/grammar.o -c $(SRCD)/compress/CfGrammar.cpp	
	
$(OBJD)/test.o : $(SRCD)/test/Tests.cpp $(SRCD)/test/Tests.h $(OBJD)/lfirstcomp.o
	$(COMPILER) $(FLAGS) -o $(OBJD)/test.o -c $(SRCD)/test/Tests.cpp	
	
$(OBJD)/etimer.o : $(SRCD)/test/etimer.cpp $(SRCD)/test/etimer.h
	$(COMPILER) $(FLAGS) -o $(OBJD)/etimer.o -c $(SRCD)/test/etimer.cpp
	
$(OBJD)/fsort.o : $(SRCD)/compress/FastSort.cpp $(SRCD)/compress/FastSort.h
	$(COMPILER) $(FLAGS) -o $(OBJD)/fsort.o -c $(SRCD)/compress/FastSort.cpp
