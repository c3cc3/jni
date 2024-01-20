echo "Make a object from C source"
set -x
gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -I${FQ_WORK_HOME}/include com_clang_fq_FileQueueJNI.c -o com_clang_fq_FileQueueJNI.o
