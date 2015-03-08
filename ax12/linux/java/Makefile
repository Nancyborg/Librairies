all: ax12.jar ../../../../ia-raspberry/libs/ax12.jar

CXX ?= g++
SWIG ?= swig
JAR ?= jar
JAVA_HOME ?= $(shell readlink -f /usr/bin/javac | sed "s:bin/javac::")
JAVA_INCLUDE ?= -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
ANT ?= ant

-include Makefile.local

../../../../ia-raspberry/libs/ax12.jar: ax12.jar
	cp $< $@

libs/libAX12Java.so: ../../AX12Base.cpp
	$(SWIG) -c++ -java -package fr.nancyborg.ax12 -outdir src/fr/nancyborg/ax12 AX12.i
	$(CXX) $(CXXFLAGS) $(JAVA_INCLUDE) -std=c++11 -fPIC -shared -o $@ -I.. $^ AX12_wrap.cxx 

ax12.jar: libs/libAX12Java.so
	$(ANT)

clean:
	rm -rf *.cxx *.java
