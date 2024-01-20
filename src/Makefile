# Define a variable for classpath
CLASS_PATH = ./com/clang/fq

all : libjfq.so

# Last(4-th), Make a shared library with objects
# $@ matches the target, $< matches the first dependency
libjfq.so : FileQueueJNI.o
#	gcc -Wl,-fPIC -I"/usr/lib/jvm/java-7-openjdk-armhf/include" -I"/usr/fq/include" -shared -o $@ $<
# Raspi-2
# gcc -Wall -I"/usr/lib/jvm/java-8-openjdk-armhf/include" -I"/usr/fq/include" -L"/usr/fq/lib" -shared -o $@ $< -lfq
	gcc -Wall,-fPIC -I"/usr/lib/jvm/java-7-openjdk-armhf/include" -I"/usr/fq/include" -L"/usr/fq/lib" -shared -o $@ $< -lfq

# 3rd, Make a object file with Native C and header.
# $@ matches the target, $< matches the first dependency
FileQueueJNI.o : FileQueueJNI.c FileQueueJNI.h
# Raspi-2
# gcc -I"/usr/lib/jvm/java-8-openjdk-armhf/include" -I"/usr/lib/jvm/java-8-openjdk-armhf/include/linux" -I"/usr/fq/include" -c $< -o $@
	gcc -I"/usr/lib/jvm/java-7-openjdk-armhf/include" -I"/usr/lib/jvm/java-7-openjdk-armhf/include/linux" -I"/usr/fq/include" -c $< -o $@


# Second, Make a header file  with java classs
# $* matches the target filename without the extension
FileQueueJNI.h : FileQueueJNI.class
	javah com.clang.fq.FileQueueJNI
	jar cvf FileQueueJNI.jar com

# First, Make a class with java source
FileQueueJNI.class: FileQueueJNI.java
	javac -d . $<

clean :
	rm -f FileQueueJNI.class FileQueueJNI.h FileQueueJNI.o libjfq.so FileQueueJNI.jar
	rm -rf com
