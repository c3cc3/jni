#include <stdio.h>
#include "com_clang_fq_FileQueueJNI.h"
#include "fq_common.h"
#include "fqueue.h"
#include "fq_manage.h"

#define MAX_FQ_TABLES 100 // This is max-threads

#define DE_CMD	1
#define EN_CMD	2
#define DE_XA_CMD	3
#define DE_XA_COMMIT_CMD	4
#define DE_XA_CANCEL_CMD	5
#define DE_CLOSE_CMD	9

static fqueue_obj_t    *g_obj[MAX_FQ_TABLES];
pthread_mutex_t	rw_lock[MAX_FQ_TABLES];
 
static const char *Bring_Cstr_from_class_member(JNIEnv *env, jobject thisObj, jclass cls, char *cls_member_name )
{
	jfieldID fid_value = (*env)->GetFieldID(env, cls, cls_member_name, "Ljava/lang/String;");
	jstring jStr = (*env)->GetObjectField(env, thisObj, fid_value);
	const char *cStr = (*env)->GetStringUTFChars(env, jStr, NULL);
	return cStr;
} 
static jint Bring_Int_from_class_member( JNIEnv *env, jobject thisObj, jclass cls, char *cls_member_name)
{
	jfieldID fid_value = (*env)->GetFieldID(env, cls, cls_member_name, "I");
	jint value = (*env)->GetIntField(env, thisObj, fid_value);
	return (value);
}

// Implementation of native method AccessFileQueue() of FileQueueJNI class
// 
// JNIEXPORT int JNICALL Java_FileQueueJNI_NativeOpen(JNIEnv *env, jobject thisObj) {
// You should change a method name to package name.
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeOpen(JNIEnv *env, jobject thisObj) 
{
	int rc;
	fqueue_obj_t *obj=NULL;
	fq_logger_t 	*l =  NULL;
	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);

	printf("In C, the int(tid_index)  is %d\n", tid_index);
	if( tid_index < 0 || tid_index > (MAX_FQ_TABLES-1) ) {
		printf("In C, illegal index(%d).\n", tid_index);
		return(-12);
	}

	pthread_mutex_init(&rw_lock[tid_index], NULL);
	obj = g_obj[tid_index];

	/************************************** 
	** 1. Bring a logname(String) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "logname"
	jfieldID fid_logname = (*env)->GetFieldID(env, cls, "logname", "Ljava/lang/String;");
	if (NULL == fid_logname) {
			return(-1);
	}
	// Get the object given the Field ID
	jstring logname = (*env)->GetObjectField(env, thisObj, fid_logname);
		
	// Create a C-string with the JNI String
	const char *cStr_logname = (*env)->GetStringUTFChars(env, logname, NULL);
	if (NULL == cStr_logname) {
		return(-2);
	}
	printf("In C, the string is %s\n", cStr_logname);

	/**************************************
	** 2. Bring a loglevel(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "loglevel"
	jfieldID fid_loglevel  = (*env)->GetFieldID(env, cls, "loglevel", "I");
	if( NULL == fid_loglevel ) return (-3);

	jint loglevel  = (*env)->GetIntField(env, thisObj, fid_loglevel);
	printf("In C, the int  is %d\n", loglevel);


	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}


	/************************************** 
	** 4. Bring a path(String) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "path"
	jfieldID fid_path = (*env)->GetFieldID(env, cls, "path", "Ljava/lang/String;");
	if (NULL == fid_path) {
			log_call_process_info(l);
			return(-5);
	}
	// Get the object given the Field ID
	jstring path = (*env)->GetObjectField(env, thisObj, fid_path);
		
	// Create a C-string with the JNI String
	const char *cStr_path = (*env)->GetStringUTFChars(env, path, NULL);
	if (NULL == cStr_path) {
		log_call_process_info(l);
		return(-6);
	}
	printf("In C, the string is %s\n", cStr_path);

	/************************************** 
	** 5. Bring a qname(String) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "qname"
	jfieldID fid_qname = (*env)->GetFieldID(env, cls, "qname", "Ljava/lang/String;");
	if (NULL == fid_qname) {
			log_call_process_info(l);
			return(-7);
	}
	// Get the object given the Field ID
	jstring qname = (*env)->GetObjectField(env, thisObj, fid_qname);
		
	// Create a C-string with the JNI String
	const char *cStr_qname = (*env)->GetStringUTFChars(env, qname, NULL);
	if (NULL == cStr_qname) {
		log_call_process_info(l);
		return(-8);
	}
	printf("In C, the string is %s\n", cStr_qname);

	fq_log(l, FQ_LOG_DEBUG, "FQ->[%s, %s]: Open index is [%d]", cStr_path, cStr_qname, tid_index);
	

	/***************************************
	** 6. Open FileQueue
	***************************************/
	if( !obj ) {
		rc = OpenFileQueue(l, NULL, NULL, NULL, NULL, (char *)cStr_path, (char *)cStr_qname, &obj);
		if( rc != TRUE ) {
			fq_log(l, FQ_LOG_ERROR, "path=[%s], qname=[%s].", cStr_path, cStr_qname);
			log_call_process_info(l);
			return(-10);
		}
		g_obj[tid_index] = obj;
		fq_log(l, FQ_LOG_DEBUG, "success path=[%s], qname=[%s].", cStr_path, cStr_qname);
	}
	else { 
		fq_log(l, FQ_LOG_ERROR, "Already opened. After closing, retry.");
		log_call_process_info(l);
		return(-11);
	}

	return(0);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeRead(JNIEnv *env, jobject thisObj) 
{
	int rc;
	int buffer_size=0;
	char *ptr_buf = NULL;
	long l_seq=0L;
	long l_run_time=0L;
	char *outCStr=NULL;
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) {
		return (-3);
	}

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);

	pthread_mutex_lock(&rw_lock[tid_index]);

	obj = g_obj[tid_index];
	CHECK(obj);
    size_t obj_msglen = obj->h_obj->h->msglen;
    if( obj_msglen > (65536*10) ) {
        buffer_size = obj_msglen+1;
    }
    else {
        buffer_size = 65536*10;
    }

	ptr_buf = 0x00;
	ptr_buf = calloc(buffer_size, sizeof(unsigned char));

	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);

	rc = obj->on_de(obj->l, obj, ptr_buf, buffer_size, &l_seq, &l_run_time);
	if( rc == DQ_EMPTY_RETURN_VALUE ) {
		fq_log(obj->l, FQ_LOG_DEBUG, "empty.");

		/*
		** set out_empty_flag 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_empty_flag  = (*env)->GetFieldID(env, cls, "out_empty_flag", "I");
		if( NULL == fid_out_empty_flag ) {
			log_call_process_info(obj->l);
			rc = -91;
			goto stop ;
		}

		jint out_empty_flag  = (*env)->GetIntField(env, thisObj, fid_out_empty_flag);
		fq_log(obj->l, FQ_LOG_DEBUG, "In C, the int value flag is %d.", out_empty_flag);

		out_empty_flag = 1;
		(*env)->SetIntField(env, thisObj, fid_out_empty_flag, out_empty_flag);
		goto stop;
	}
	else if( rc < 0 ) {
		fq_log(obj->l, FQ_LOG_ERROR, "fatal error in C module.");
		/*
		** set out_stop_flag 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_stop_flag  = (*env)->GetFieldID(env, cls, "out_stop_flag", "I");
		if( NULL == fid_out_stop_flag ) {
			log_call_process_info(obj->l);
			rc = -92;
			goto stop;
		}

		jint out_stop_flag  = (*env)->GetIntField(env, thisObj, fid_out_stop_flag);
		out_stop_flag = 1;
		(*env)->SetIntField(env, thisObj, fid_out_stop_flag, out_stop_flag);
		goto stop;
	}
	else {
        outCStr = calloc(buffer_size, sizeof(unsigned char));
		memcpy(outCStr, ptr_buf, rc);
		outCStr[rc] = 0x00;

		/*
		** set out_seq 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_seq  = (*env)->GetFieldID(env, cls, "out_seq", "J"); 
		/*
		 For primitives, 
			use "I" for int, 
			"B" for byte, 
			"S" for short, 
			"J" for long, 
			"F" for float, 
			"D" for double, 
			"C" for char, 
			"Z" for boolean. 
			For arrays, include a prefix "[", e.g., "[Ljava/lang/Object;" 
			for an array of Object; "[I" for an array of int.
		*/
		if( NULL == fid_out_seq ) {
			log_call_process_info(obj->l);
			rc = -93;
            SAFE_FREE(outCStr);
			goto stop;
		}

		jlong out_seq  = (*env)->GetLongField(env, thisObj, fid_out_seq);
		fq_log(obj->l, FQ_LOG_DEBUG, "In C, the long value(out_seq) is %ld.", out_seq);

		out_seq = l_seq; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_seq, out_seq);

		/*
		** set out_run_time ( micro seconds )
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_run_time  = (*env)->GetFieldID(env, cls, "out_run_time", "J"); 
		if( NULL == fid_out_run_time ) {
			log_call_process_info(obj->l);
			rc = -95;
            SAFE_FREE(outCStr);
			goto stop;
		}
		jlong out_run_time  = (*env)->GetLongField(env, thisObj, fid_out_run_time);
		fq_log(obj->l, FQ_LOG_DEBUG, "In C, the long value(out_run_time) is %ld.", out_run_time);

		out_run_time = l_run_time; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_run_time, out_run_time);
	}

	// Get the Field ID of the instance String variables "out_msg"
	jfieldID fid_out_msg = (*env)->GetFieldID(env, cls, "out_msg", "Ljava/lang/String;");
	if (NULL == fid_out_msg) {
		log_call_process_info(obj->l);
		rc = -99;
        SAFE_FREE(outCStr);
		goto stop;
	}

	 // Get the object given the Field ID
	jstring out_msg = (*env)->GetObjectField(env, thisObj, fid_out_msg);
	
	// Create a new C-string and assign to the JNI string
	out_msg = (*env)->NewStringUTF(env, outCStr);
	if (NULL == out_msg) {
		log_call_process_info(obj->l);
		rc = -99;
        SAFE_FREE(outCStr);
		goto stop;
	}
	
	// modify the instance variables
	(*env)->SetObjectField(env, thisObj, fid_out_msg, out_msg);

    SAFE_FREE(outCStr);
stop:

	pthread_mutex_unlock(&rw_lock[tid_index]);

	if(ptr_buf) free(ptr_buf);

	return (rc);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeReadXA(JNIEnv *env, jobject thisObj) 
{
	int rc;
	int buffer_size=0;
	char *ptr_buf = NULL;
	long l_seq=0L;
	long l_run_time=0L;
	char *outCStr=NULL;
	char C_unlink_filename[256];
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);
	pthread_mutex_lock(&rw_lock[tid_index]);

	obj = g_obj[tid_index];
	CHECK(obj);

    size_t obj_msglen = obj->h_obj->h->msglen;
    if( obj_msglen > (65536*10) ) {
        buffer_size = obj_msglen+1;
    }
    else {
        buffer_size = 65536*10;
    }

	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);

	ptr_buf = 0x00;
	ptr_buf = calloc(buffer_size, sizeof(unsigned char));


	rc = obj->on_de_XA(obj->l, obj, ptr_buf, buffer_size, &l_seq, &l_run_time, C_unlink_filename);
	if( rc == DQ_EMPTY_RETURN_VALUE ) {

		pthread_mutex_unlock(&rw_lock[tid_index]);

		// fq_log(obj->l, FQ_LOG_DEBUG, "empty.");

		/*
		** set out_empty_flag 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_empty_flag  = (*env)->GetFieldID(env, cls, "out_empty_flag", "I");
		if( NULL == fid_out_empty_flag ) {
			log_call_process_info(obj->l);
			rc = -91;
			goto stop ;
		}

		jint out_empty_flag  = (*env)->GetIntField(env, thisObj, fid_out_empty_flag);
		// fq_log(obj->l, FQ_LOG_DEBUG, "In C, the int value flag is %d.", out_empty_flag);

		out_empty_flag = 1;
		(*env)->SetIntField(env, thisObj, fid_out_empty_flag, out_empty_flag);
		goto stop;
	}
	else if( rc < 0 ) {

		pthread_mutex_unlock(&rw_lock[tid_index]);

		fq_log(obj->l, FQ_LOG_ERROR, "fatal error in C module.");
		/*
		** set out_stop_flag 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_stop_flag  = (*env)->GetFieldID(env, cls, "out_stop_flag", "I");
		if( NULL == fid_out_stop_flag ) {
			log_call_process_info(obj->l);
			rc = -92;
			goto stop;
		}

		jint out_stop_flag  = (*env)->GetIntField(env, thisObj, fid_out_stop_flag);
		out_stop_flag = 1;
		(*env)->SetIntField(env, thisObj, fid_out_stop_flag, out_stop_flag);
		goto stop;
	}
	else {
		fq_log(obj->l, FQ_LOG_DEBUG, "XA rc = [%d]\n", rc);

        outCStr = calloc(buffer_size, sizeof(unsigned char));
		memcpy(outCStr, ptr_buf, rc);
		outCStr[rc] = 0x00;

		/*
		** set out_seq 
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_seq  = (*env)->GetFieldID(env, cls, "out_seq", "J"); 
		/*
		 For primitives, 
			use "I" for int, 
			"B" for byte, 
			"S" for short, 
			"J" for long, 
			"F" for float, 
			"D" for double, 
			"C" for char, 
			"Z" for boolean. 
			For arrays, include a prefix "[", e.g., "[Ljava/lang/Object;" 
			for an array of Object; "[I" for an array of int.
		*/
		if( NULL == fid_out_seq ) {
			log_call_process_info(obj->l);
			rc = -93;
            SAFE_FREE(outCStr);
			goto stop;
		}
		jlong out_seq  = (*env)->GetLongField(env, thisObj, fid_out_seq);

		out_seq = l_seq; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_seq, out_seq);
		fq_log(obj->l, FQ_LOG_DEBUG, "In C, Set the long value(out_seq) is %ld.", out_seq);

		/*
		** set out_run_time ( micro seconds )
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_run_time  = (*env)->GetFieldID(env, cls, "out_run_time", "J"); 
		if( NULL == fid_out_run_time ) {
			log_call_process_info(obj->l);
			rc = -95;
            SAFE_FREE(outCStr);
			goto stop;
		}
		jlong out_run_time  = (*env)->GetLongField(env, thisObj, fid_out_run_time);
		fq_log(obj->l, FQ_LOG_DEBUG, "In C, the long value(out_run_time) is %ld.", out_run_time);

		out_run_time = l_run_time; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_run_time, out_run_time);

		/*
		** set out_unlink_filename
		*/
		// Get the Field ID of the instance variables "out_unlink_filename"
		jfieldID fid_out_unlink_filename = (*env)->GetFieldID(env, cls, "out_unlink_filename", "Ljava/lang/String;");
		if (NULL == fid_out_unlink_filename) {
			log_call_process_info(obj->l);
			rc = -96;
            SAFE_FREE(outCStr);
			goto stop;
        }

        // Get the object given the Field ID
        jstring out_unlink_filename = (*env)->GetObjectField(env, thisObj, fid_out_unlink_filename);

		out_unlink_filename = (*env)->NewStringUTF(env, C_unlink_filename);
        if (NULL == out_unlink_filename) {
			log_call_process_info(obj->l);
			rc = -97;
            SAFE_FREE(outCStr);
			goto stop;
        }

        // modify the instance variables
        (*env)->SetObjectField(env, thisObj, fid_out_unlink_filename, out_unlink_filename);

	}

	// Get the Field ID of the instance String variables "out_msg"
	jfieldID fid_out_msg = (*env)->GetFieldID(env, cls, "out_msg", "Ljava/lang/String;");
	if (NULL == fid_out_msg) {
		log_call_process_info(obj->l);
		rc = -99;
        SAFE_FREE(outCStr);
		goto stop;
	}

	 // Get the object given the Field ID
	jstring out_msg = (*env)->GetObjectField(env, thisObj, fid_out_msg);
	
	// Create a new C-string and assign to the JNI string
	out_msg = (*env)->NewStringUTF(env, outCStr);
	if (NULL == out_msg) {
		log_call_process_info(obj->l);
		rc = -99;
        SAFE_FREE(outCStr);
		goto stop;
	}
	
	// modify the instance variables
	(*env)->SetObjectField(env, thisObj, fid_out_msg, out_msg);

    SAFE_FREE(outCStr);
stop:

	if(ptr_buf) free(ptr_buf);

	return (rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeCommitXA(JNIEnv *env, jobject thisObj) 
{
	int rc;
	long l_run_time = 0L;
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);

	obj = g_obj[tid_index];
	CHECK(obj);
	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);


	// Get the Field ID of the instance variables "unlink_filename"
	jfieldID fid_out_unlink_filename = (*env)->GetFieldID(env, cls, "out_unlink_filename", "Ljava/lang/String;");
	if (NULL == fid_out_unlink_filename) {
		log_call_process_info(obj->l);
		rc = -101;
		goto stop;
	}

	// Get the object given the Field ID
	jstring out_unlink_filename = (*env)->GetObjectField(env, thisObj, fid_out_unlink_filename);
	
	// Create a C-string with the JNI String
	const char *cStr = (*env)->GetStringUTFChars(env, out_unlink_filename, NULL);
	if (NULL == cStr) {
		log_call_process_info(obj->l);
		rc = -102;
		goto stop;
	}

	/*
	**  Get seq from instance fields
	*/
	jfieldID fid_out_seq  = (*env)->GetFieldID(env, cls, "out_seq", "J");
	if( NULL == fid_out_seq ) {
		log_call_process_info(obj->l);
		rc = -103;
		goto stop;
	}

	// jlong seq  = (*env)->GetLongField(env, thisObj, fid_out_seq);
	jlong seq  =  obj->h_obj->h->de_sum;
	// fq_log(obj->l, FQ_LOG_DEBUG, "In C, the long value(seq) is %ld.", seq);

	rc = obj->on_commit_XA(obj->l, obj, seq, &l_run_time, (char  *)cStr);

	fq_log(obj->l, FQ_LOG_DEBUG, "In C, XA commit success!. seq = [%ld]-th.", seq);
stop:

	if(cStr) free((void *)cStr); // this is very important. If you don't have this, you will have a memory (RSS) leak.

	pthread_mutex_unlock(&rw_lock[tid_index]);
	return(rc);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeCancelXA(JNIEnv *env, jobject thisObj) 
{
	int rc;
	long l_run_time=0L;
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);

	obj = g_obj[tid_index];
	CHECK(obj);
	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);

	/*
	**  Get seq from instance fields
	*/
	jfieldID fid_out_seq  = (*env)->GetFieldID(env, cls, "out_seq", "J");
	if( NULL == fid_out_seq ) {
		log_call_process_info(obj->l);
		rc = -103;
		goto stop;
	}

	jlong seq  = (*env)->GetLongField(env, thisObj, fid_out_seq);
	// fq_log(obj->l, FQ_LOG_DEBUG, "In C, the long value(seq) is %ld.", seq);
	
	rc = obj->on_cancel_XA(obj->l, obj, seq, &l_run_time);
	fq_log(obj->l, FQ_LOG_DEBUG, "In C, XA cancel success!. seq = [%ld]-th.", seq);

stop:
	pthread_mutex_unlock(&rw_lock[tid_index]);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeWrite(JNIEnv *env, jobject thisObj, jstring inJNIStr_data) 
{

	int rc;
	long l_seq = 0;
	long l_run_time=0L;
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);
	pthread_mutex_lock(&rw_lock[tid_index]);

	obj = g_obj[tid_index];
	CHECK(obj);
	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);

	const char *inCStr_data = (*env)->GetStringUTFChars(env, inJNIStr_data, NULL);
	if (NULL == inCStr_data) {
		fq_log(obj->l, FQ_LOG_ERROR, "There is no input-data in your arguments.");
		log_call_process_info(obj->l);
		rc = -120;
		goto stop;
	}

	rc =  obj->on_en(obj->l, obj, EN_NORMAL_MODE, inCStr_data, strlen(inCStr_data)+1, strlen(inCStr_data), &l_seq, &l_run_time );
	if( rc == EQ_FULL_RETURN_VALUE )  {
		fq_log(obj->l, FQ_LOG_DEBUG, "data full.");
		/*
        ** set full_flag
        */
        // Read the int static variable and modify its values
        jfieldID fid_out_full_flag  = (*env)->GetFieldID(env, cls, "out_full_flag", "I");
        if( NULL == fid_out_full_flag ) {
			log_call_process_info(obj->l);
			rc = -121;
			goto stop;
		}
        jint out_full_flag  = (*env)->GetIntField(env, thisObj, fid_out_full_flag);
        fq_log(obj->l, FQ_LOG_DEBUG, "In C, the int value flag is %d.", out_full_flag);

        out_full_flag = 1;
        (*env)->SetIntField(env, thisObj, fid_out_full_flag, out_full_flag);
		goto stop;
	}
	else if( rc < 0 ) {
		fq_log(obj->l, FQ_LOG_ERROR, "Fatal error. in C module.");
		log_call_process_info(obj->l);
		goto stop;
	}
	else {
		/*
		** Set out_seq
		*/
#if 0
		jfieldID fid_out_seq  = (*env)->GetFieldID(env, cls, "out_seq", "J"); 
		if( NULL == fid_out_seq ) {
			rc = -122;
			goto stop;
		}
		jlong out_seq  = (*env)->GetLongField(env, thisObj, fid_out_seq);
		out_seq = l_seq; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_seq, out_seq);
#endif

		/*
		** set out_run_time ( micro seconds )
		*/
		// Read the int static variable and modify its values
		jfieldID fid_out_run_time  = (*env)->GetFieldID(env, cls, "out_run_time", "J"); 
		if( NULL == fid_out_run_time ) {
			log_call_process_info(obj->l);
			rc = -123;
			goto stop;
		}
		jlong out_run_time  = (*env)->GetLongField(env, thisObj, fid_out_run_time);
		out_run_time = l_run_time; // Put C value to Java value
		(*env)->SetLongField(env, thisObj, fid_out_run_time, out_run_time);
	}
stop:
	(*env)->ReleaseStringUTFChars(env, inJNIStr_data, inCStr_data); // this is very important. If you don't have this you will have a memory (RSS) leak.

	pthread_mutex_unlock(&rw_lock[tid_index]);
	return(rc);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeClose(JNIEnv *env, jobject thisObj) 
{
	fqueue_obj_t *obj=NULL;
	fq_logger_t *l=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);

	obj = g_obj[tid_index];

	fq_log(obj->l, FQ_LOG_DEBUG, "tid_index=[%d]. queue:[%s][%s]", tid_index, obj->path, obj->qname);

	if( obj ) {
		CloseFileQueue(l, &obj);
		obj = NULL;
	}
	if(l) {
		fq_close_file_logger(&l);
		l = NULL;
	}

	g_obj[tid_index] = NULL;

	return(1);
}

/*
** error
*/
JNIEXPORT jlong JNICALL Java_com_clang_fq_FileQueueJNI_NativeGetMsgLength(JNIEnv *env, jobject thisObj) 
{
	long msglen = 0L;
	fqueue_obj_t *obj=NULL;

	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);
	// printf("In C, the int(tid_index)  is %d\n", tid_index);

	obj = g_obj[tid_index];

	if( obj ) {
		msglen = obj->h_obj->h->msglen;
		// printf("In C, msglen = [%ld].\n", msglen);
	}
	return(msglen);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeMoveFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname, jstring inJNIStr_dst_path, jstring inJNIStr_dst_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l = NULL;

	/************************************** 
	** 1. Bring a logname(String) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "logname"
	jfieldID fid_logname = (*env)->GetFieldID(env, cls, "logname", "Ljava/lang/String;");
	if (NULL == fid_logname) {
			return(-1);
	}
	// Get the object given the Field ID
	jstring logname = (*env)->GetObjectField(env, thisObj, fid_logname);
		
	// Create a C-string with the JNI String
	const char *cStr_logname = (*env)->GetStringUTFChars(env, logname, NULL);
	if (NULL == cStr_logname) {
		return(-2);
	}

	/**************************************
	** 2. Bring a loglevel(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "loglevel"
	jfieldID fid_loglevel  = (*env)->GetFieldID(env, cls, "loglevel", "I");
	if( NULL == fid_loglevel ) return (-3);

	jint loglevel  = (*env)->GetIntField(env, thisObj, fid_loglevel);

	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_dst_path = (*env)->GetStringUTFChars(env, inJNIStr_dst_path, NULL);
    if (NULL == inCStr_dst_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no dst path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_dst_qname = (*env)->GetStringUTFChars(env, inJNIStr_dst_qname, NULL);
    if (NULL == inCStr_dst_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no dst qname.");
		return(-5);
	}

	rc = MoveFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname, (char *)inCStr_dst_path, (char *)inCStr_dst_qname, 999999);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_dst_path, inCStr_dst_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_dst_qname, inCStr_dst_qname);
	
	fq_log(l, FQ_LOG_DEBUG, "Moved count of messages is [%d].", rc);

	return(rc);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeResetFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}

	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");
	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = ResetFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	if( rc == TRUE ) 
		fq_log(l, FQ_LOG_DEBUG, "Reset success. (%s)(%s).", inCStr_src_path, inCStr_src_qname);
	else
		fq_log(l, FQ_LOG_DEBUG, "Reset failed. (%s)(%s).", inCStr_src_path, inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	
	return(rc);
}

JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeFlushFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");
	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = FlushFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	if( rc == TRUE ) 
		fq_log(l, FQ_LOG_DEBUG, "Flush success. (%s)(%s).", inCStr_src_path, inCStr_src_qname);
	else
		fq_log(l, FQ_LOG_DEBUG, "Flush failed. (%s)(%s).", inCStr_src_path, inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeInfoFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");

	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = InfoFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeSkipFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");

	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = ForceSkipFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeDiagFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");

	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = DiagFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeCreateFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");

	/***************************************
	** 3. Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}
	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}
	rc = CreateFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jint JNICALL Java_com_clang_fq_FileQueueJNI_NativeUnlinkFQ(JNIEnv *env, jobject thisObj, jstring inJNIStr_src_path, jstring inJNIStr_src_qname)
{
	jint rc;
	jclass cls = (*env)->GetObjectClass(env, thisObj);
	fq_logger_t *l=NULL;

	const char *cStr_logname =  (char *)Bring_Cstr_from_class_member( env, thisObj, cls, "logname");	
	if (NULL == cStr_logname) {
		return(-2);
	}
	jint loglevel  = Bring_Int_from_class_member( env, thisObj, cls, "loglevel");
	
	/***************************************
	**  Open logfile
	***************************************/
	rc = fq_open_file_logger(&l, cStr_logname, loglevel );
	if( rc <= 0 ) {
		return(-4);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_path = (*env)->GetStringUTFChars(env, inJNIStr_src_path, NULL);
    if (NULL == inCStr_src_path) {
		fq_log(l, FQ_LOG_ERROR, "There is no source path.");
		return(-5);
	}

	//  Convert the JNI String (jstring) into C-String (char*)
    const char *inCStr_src_qname = (*env)->GetStringUTFChars(env, inJNIStr_src_qname, NULL);
    if (NULL == inCStr_src_qname) {
		fq_log(l, FQ_LOG_ERROR, "There is no source qname.");
		return(-6);
	}

	rc = UnlinkFileQueue( l, (char *)inCStr_src_path, (char *)inCStr_src_qname);

	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_path, inCStr_src_path);
	(*env)->ReleaseStringUTFChars(env, inJNIStr_src_qname, inCStr_src_qname);

	return(rc);
}
JNIEXPORT jfloat JNICALL Java_com_clang_fq_FileQueueJNI_NativeUsage (JNIEnv *env , jobject thisObj)
{
	jfloat rc;
	fqueue_obj_t *obj=NULL;
	// Get a reference to this object's class
	jclass cls = (*env)->GetObjectClass(env, thisObj);

	/**************************************
	** 0. Bring a tid_index(int) from class member.
	**************************************/
	// Use fields of class
	// Get the Field ID of the instance variables "tid_index"
	jfieldID fid_tid_index  = (*env)->GetFieldID(env, cls, "tid_index", "I");
	if( NULL == fid_tid_index ) return (-3);

	jint tid_index  = (*env)->GetIntField(env, thisObj, fid_tid_index);

	obj = g_obj[tid_index];

	rc = obj->on_get_usage(obj->l, obj);

	// printf("In C, the float value is %f\n", rc);

	return(rc);

}
