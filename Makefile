CXX = clang++
HEADERS = $(shell find include tests -name *.h)
INCLUDE = include/


TESTS_SRC = $(shell find tests -name *.cc)
TESTS_OBJ = $(TESTS_SRC:%.cc=obj/%.o) 
GTEST=gtest-1.7.0

K2TREE_SRC = $(shell find src/libk2tree -name *.cc)
K2TREE_OBJ = $(K2TREE_SRC:%.cc=obj/%.o)

//FLAGS = -std=c++11 -O3 -Wall -Wextra -Winline -Wpedantic
FLAGS = -std=c++11 -O0 -g -Wall -Wextra -Winline -Wpedantic
LIBRARIES = -lcds -lboost_filesystem -lboost_system




.PHONY: clean style test all

all: test build_k2tree time qry_gen


# TEST
test: bin/test

bin/test: $(GTEST)/libgtest.a $(K2TREE_OBJ) $(TESTS_OBJ) 
	@echo " [LNK] Linking test"
	@$(CXX) -isystem $(GTEST)/include -lpthread  $(TESTS_OBJ) $(K2TREE_OBJ) $(LIBRARIES)\
					$(GTEST)/libgtest.a -o bin/test

$(GTEST)/libgtest.a:
	@echo " [BLD] Building libgtest"
	@$(CXX) -isystem ${GTEST}/include -I${GTEST} \
      -pthread -c ${GTEST}/src/gtest-all.cc -o $(GTEST)/gtest-all.o
	@ar -rv $(GTEST)/libgtest.a $(GTEST)/gtest-all.o

obj/tests/%.o: tests/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -isystem $(GTEST)/include -I$(INCLUDE) $(FLAGS) -c $< -o $@
# END TEST



# STYLE
style:
	@echo " [CHK] Cheking Code Style"
	@./cpplint.py --filter=-legal,-runtime/threadsafe_fn $(TESTS_SRC) $(K2TREE_SRC) $(HEADERS)
# END STYLE



# K2TREE

build_k2tree: bin/build_k2tree
time: bin/time
qry_gen: bin/qry_gen

bin/qry_gen: $(K2TREE_OBJ) obj/src/qry_gen.o
	@echo " [LNK] Linking time"
	@$(CXX) -lcds $(K2TREE_OBJ) obj/src/qry_gen.o $(LIBRARIES) -o bin/qry_gen

bin/time: $(K2TREE_OBJ) obj/src/time.o
	@echo " [LNK] Linking time"
	@$(CXX) -lcds $(K2TREE_OBJ) obj/src/time.o $(LIBRARIES) -o bin/time

bin/build_k2tree: $(K2TREE_OBJ) obj/src/build_k2tree.o
	@echo " [LNK] Linking build_k2tree"
	@$(CXX) -lcds $(K2TREE_OBJ) obj/src/build_k2tree.o $(LIBRARIES) -o bin/build_k2tree

obj/src/build_k2tree.o: src/build_k2tree.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -I$(INCLUDE) $(FLAGS) -c $< -o $@

obj/src/time.o: src/time.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -I$(INCLUDE) $(FLAGS) -c $< -o $@

obj/src/qry_gen.o: src/queries_generator.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -I$(INCLUDE) $(FLAGS) -c $< -o $@
obj/src/%.o: src/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -I$(INCLUDE) $(FLAGS) -c $< -o $@
# END K2TREE


# CLEAN
clean : clean_k2tree clean_test

clean_test:
	@echo " [CLN] Cleaning test"
	@touch .dummy
	@rm $(shell find obj/tests -name *.o) .dummy
	@rm bin/test -f

clean_k2tree:
	@echo " [CLN] Cleaning k2tree"
	@touch .dummy
	@rm $(shell find obj/src -name *.o) .dummy
# END CLEAN
