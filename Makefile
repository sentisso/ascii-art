#V Makefile smí být použity pouze tyto programy: rm, gcc, g++, mkdir, doxygen, cp, mv, cd, ar, make.

#Detailní pokyny pro vytvoření souboru Makefile, tedy při napsání:

# - make all nebo make vytvoří veškeré výstupy, generované ze zdrojového kódu,
# - make compile vytvoří binární podobu semestrální práce, výsledek bude uložen v adresáři <login> do souboru <login> (tedy ve stejném adresáři jako se nachází Makefile),
# - make run spustí binární podobu semestrální práce,
# - make clean smaže všechny vygenerované soubory a uvede adresář <login> do původního stavu,
# - make doc vygeneruje dokumentaci do adresáře <login>/doc. Dokumentace může být vytvořená staticky ve formátu HTML (pak make doc nebude nic dělat) nebo dynamicky generovaná programem doxygen (generována pouze v HTML).

LOGIN = ascii

SRC_PATH = src
BUILD_PATH = build
ZIP = Makefile Doxyfile zadani.txt prohlaseni.txt $(wildcard src/*) $(wildcard assets/*) $(wildcard examples/*) DOCUMENTATION.md

DEV_FLAGS = -std=c++17 -Wall -pedantic -g -fsanitize=address
PROD_FLAGS = -std=c++17 -O2 -Wall -pedantic
LIBS = -lasan -lstdc++fs -lncurses -ljpeg -lpng

# exclude all files that contain main()
CPP_SOURCES = $(shell find $(SRC_PATH) -type f -name '*.cpp' ! -name 'test.cpp' ! -name 'main.cpp')
CPP_OBJECTS = $(patsubst src/%.cpp, build/%.o, ${CPP_SOURCES})
DEPS = $(patsubst src/%.cpp, build/%.dep, ${CPP_SOURCES})

.PHONY: all compile run doc clean zip dev test

all: compile doc

# compile each file + src/main.cpp with PROD flags
compile: $(CPP_OBJECTS) $(BUILD_PATH)/main.o
	g++ -o $(LOGIN) $^ $(LIBS) $(PROD_FLAGS)

# target for each build/%.o file
build/%.o: src/%.cpp
	@mkdir -p $(shell dirname $@)
	g++ -c $< -o $@ $(LIBS) $(DEV_FLAGS)

run: compile
	./$(LOGIN)

# compile everything with DEV flags and run the dev version of the LOGIN
dev: $(CPP_OBJECTS) $(BUILD_PATH)/main.o
	g++ -o $(LOGIN)_dev $^ $(LIBS) $(DEV_FLAGS)
	@echo ""
	./$(LOGIN)_dev

# clean everything
clean:
	rm -rf build doc
	rm -f test $(LOGIN) $(LOGIN)_dev $(LOGIN).zip
	
# compile each file + test.cpp and run the test
test: $(CPP_OBJECTS) $(BUILD_PATH)/test.o
	g++ -o test $^ $(LIBS) $(DEV_FLAGS)
	@echo ""
	./test
	
# generate the doxygen
doc:
	@rm -rf doc
	@mkdir -p doc
	doxygen


# zip this projects in a way that is required for submission
zip: ${LOGIN}.zip
${LOGIN}.zip: ${ZIP}
	rm -rf tmp/
	rm -f $@
	mkdir -p tmp/${LOGIN}/
	cp --parents -r $^ tmp/${LOGIN}/
	cd tmp/ && zip -r ../$@ ${LOGIN}/
	rm -rf tmp/


# generate all the dependencies of each file
build/%.dep: src/%.cpp src/*
	@mkdir -p $(shell dirname $@)
	${CXX} -MM -MT $(patsubst src/%.cpp, build/%.o, $<) $< > $@
	
include ${DEPS}
