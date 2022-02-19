#include <stdio.h>
#include <dlfcn.h>



int main(int argc, char* argv[]){
    
    //__libc_dlopen_mode("/home/ap/pocs/c/inject/payloads/libpayload.so", 2);
    void* addr = dlopen("/home/ap/pocs/c/inject/payloads/libpayload.so", 1);
    if(addr)
        printf("loaded\n");
    return 0;
}