%module AX12_Module;

%{
#include "../../AX12Base.h"
#include "../AX12Linux.h"
%}


%javaconst(1);

%include "inttypes.i"
%include "arrays_java.i"
%include "enumsimple.swg"

%rename(AX12) AX12Linux;

%typemap(javaclassmodifiers) AX12Base "class";
%pragma(java) jniclassclassmodifiers="class";
%pragma(java) moduleclassmodifiers="class";

%pragma(java) jniclassimports=%{
import cz.adamh.utils.NativeUtils;
import java.io.IOException;
%}

%pragma(java) jniclasscode=%{
    static {
        try {
            NativeUtils.loadLibraryFromJar("/libs/libAX12Java.so");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
%}

%include "../../AX12Base.h"
%include "../AX12Linux.h"
