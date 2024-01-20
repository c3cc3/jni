set -x
export JAVA_HOME=/opt/jdk-17
export FQ_DATA_HOME=/umsdata1/enmq
export FQ_HASH_HOME=/umsdata1/hash
export FQ_WORK_HOME=/ums/fq
export FQ_LIB_HOME=/ums/fq/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${FQ_LIB_HOME}:$JAVA_HOME/lib:
export PATH=$PATH:${FQ_WORK_HOME}/bin:$JAVA_HOME
