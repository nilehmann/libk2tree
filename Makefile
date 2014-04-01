HEADERS = $(shell find include tests -name *.h)
INCLUDE = include/


TESTS_SRC = $(shell find tests -name *.cc)
TESTS_OBJ = $(TESTS_SRC:%.cc=obj/%.o) 
GTEST=gtest-1.7.0
TESTSFLAGS = -g -Wall -Wextra

K2TREE_SRC = $(shell find src -name *.cc)
K2TREE_OBJ = $(K2TREE_SRC:%.cc=obj/%.o)

FLAGS = -std=c++11


.PHONY: clean style test all

all: test

# TEST
test: bin/test

bin/test: $(GTEST)/libgtest.a $(K2TREE_OBJ) $(TESTS_OBJ) 
	@echo " [LNK] Linking test"
	@$(CXX) -isystem $(GTEST)/include -lpthread -lcds $(TESTS_OBJ) $(K2TREE_OBJ) \
					$(GTEST)/libgtest.a -o bin/test

$(GTEST)/libgtest.a:
	@echo " [BLD] Building libgtest"
	@$(CXX) -isystem ${GTEST}/include -I${GTEST} \
      -pthread -c ${GTEST}/src/gtest-all.cc -o $(GTEST)/gtest-all.o
	@ar -rv $(GTEST)/libgtest.a $(GTEST)/gtest-all.o

obj/tests/%.o: tests/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -isystem $(GTEST)/include -I$(INCLUDE) $(FLAGS) $(TESTSFLAGS) -c $< -o $@
# END TEST



# STYLE
style:
	@echo " [CHK] Cheking Code Style"
	@./cpplint.py --filter=-legal,-runtime/threadsafe_fn $(TESTS_SRC) $(K2TREE_SRC) $(HEADERS)
# END STYLE



# K2TREE
obj/src/%.o: src/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -I$(INCLUDE) $(FLAGS) -c $< -o $@
# END K2TREE


# CLEAN
clean : clean_test clean_k2tree

clean_test:
	@echo " [CLN] Cleaning test"
	@rm bin/test
	@rm $(shell find obj/tests -name *.o)

clean_k2tree:
	@echo " [CLN] Cleaning k2tree"
	@rm $(shell find obj/src -name *.o)
# END CLEAN
