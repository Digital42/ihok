#include <core/logger.h>
#include <core/asserts.h>

int main(void){
    KFATAL("A test msg: %f", 6.66f);
    KERROR("A test msg: %f", 6.66f);
    KWARN("A test msg: %f", 6.66f);
    KINFO("A test msg: %f", 6.66f);
    KDEBUG("A test msg: %f", 6.66f);
    KTRACE("A test msg: %f", 6.66f);

    KASSERT(1==0);

    return 0;
}