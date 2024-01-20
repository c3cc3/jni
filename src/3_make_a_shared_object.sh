set -x
gcc -shared -fPIC -L${FQ_LIB_HOME} -o libjfq.so com_clang_fq_FileQueueJNI.o -lc -lfq
