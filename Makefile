.PHONY: all clean
LIBPKGS = cir sat
MAIN = main

LIBS = $(addprefix -l, $(LIBPKGS))
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(LIBPKGS)))

EXEC=cadJYu


all: libs main
	@echo -n "J Yu compile success \n"

libs: 
	@for pkg in $(LIBPKGS); \
	do \
		echo "Checking $$pkg..."; \
		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg; \
		cd ../..; \
	done


main:
	@mkdir -p bin
	@echo "Checking $(MAIN)..."
	@cd src/$(MAIN); \
		make -f make.$(MAIN) --no-print-directory INCLIB="$(LIBS)" EXEC=$(EXEC);
	@ln -fs bin/$(EXEC) .


clean:
	@for pkg in $(LIBPKGS); \
	do \
		echo "Cleaning $$pkg..."; \
		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg clean; \
                cd ../..; \
	done
	@echo "Cleaning $(MAIN)..."
	@cd src/$(MAIN); make -f make.$(MAIN) --no-print-directory clean
	@echo "Removing $(SRCLIBS)..."
	@cd lib; rm -f $(SRCLIBS)
	@echo "Removing $(EXEC)..."
	@rm -f bin/$(EXEC)



#bin/cadJYu: 	$(MAIN).o $(MGR).o $(GATE).o
#	$(CXX) -o bin/$(EXEC) $(MAIN).o $(MGR).o $(GATE).o
#	@ln -fs bin/$(EXEC)

#$(MAIN).o:
#	$(CXX) -c src/$(MAIN).cpp

#$(MGR).o:
#	$(CXX) -c src/$(MGR).cpp

#$(GATE).o:
#	$(CXX) -c src/$(GATE).cpp


#clean:
#	rm -rf *.o bin/$(EXEC)

	
	


