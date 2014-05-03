%module AX12

%{
#include "../../AX12Base.h"
#include "../AX12Linux.h"
%}

%include "../../AX12Base.h"
%include "../AX12Linux.h"

%pragma(java) modulecode=%{
    static {
        try {
            NativeUtils.loadLibraryFromJar("/libs/libAX12Java.so");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
%}
