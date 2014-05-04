%module AX12

%{
#include "../../AX12Base.h"
#include "../AX12Linux.h"
%}

%include "../../AX12Base.h"
%include "../AX12Linux.h"
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
