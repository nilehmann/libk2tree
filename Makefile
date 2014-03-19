

TESTS = $(shell find tests -name *.cc)
TESTS_OBJ = $(TESTS:%.cc=obj/%.o) 
GTEST=gtest-1.7.0
TESTSFLAGS = -g -Wall -Wextra -pthread 

K2TREE = $(shell find src -name *.cc)
K2TREE_OBJ = $(K2TREE:%.cc=obj/%.o)

HEADERS = $(shell find src -name *.h)

.PHONY: clean


test: $(K2TREE_OBJ) $(TESTS_OBJ) 
	@echo " [LNK] Linking test"
	@$(CXX) -isystem $(GTEST)/include -lpthread $(TESTS_OBJ) $(K2TREE_OBJ) \
					$(GTEST)/libgtest.a -o bin/test


obj/tests/%.o: tests/%.cc
	@echo " [C++] Compiling $<"
	@$(CXX) -isystem $(GTEST)/include $(TESTSFLAGS) -c $< -o $@


obj/src/%.o: src/%.cc $(HEADERS)
	@echo " [C++] Compiling $<"
	@$(CXX) -c $< -o $@

clean_test:
	@echo " [CLN] Cleaning test"
	@rm bin/test
	@rm $(TESTS_OBJ)

clean_k2tree:
	@echo " [CLN] Cleaning k2tree"
	@rm ${K2TREE_OBJ}
clean: clean_test clean_k2tree
