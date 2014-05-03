all: ax12.jar

CXX ?= g++
SWIG ?= swig
JAR ?= jar
JAVA_INCLUDE ?= -I/opt/java/include -I/opt/java/include/linux 

libs/libAX12Java.so: ../../AX12Base.cpp
	$(SWIG) -c++ -java -package fr.nancyborg.ax12 -outdir fr/nancyborg/ax12 AX12.i
	$(CXX) $(CXXFLAGS) $(JAVA_INCLUDE)  -fPIC -shared -o $@ -I.. $^ AX12_wrap.cxx 

ax12.jar: libs/libAX12Java.so
	jar -cf $@ fr cz libs

clean:
	rm -rf *.cxx *.java
