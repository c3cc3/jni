package com.clang.fq;

// final class can't be inheritanced
// If you want to inheritance, remove final keyword.
public final class FileQueueJNI {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
 
	// static fileds of class
	final static int DE = 1;
	final static int EN = 2;
	final static int DE_XA = 3;
	final static int DE_XA_COMMIT = 4;
	final static int DE_XA_CANCEL = 5;
	final static int CLOSE = 9;

	// instance fileds of class
	public int tid_index; // thread-id index (from 0 to 50)
	public String logname;
	public String path;
	public String qname;
	public int loglevel;

	public int out_stop_flag;
    public int out_empty_flag;
	public int out_full_flag;

	private long out_seq;
	private long out_run_time;
	private String out_unlink_filename;
	private String out_msg; // you can get it through getter.

	///////////////////////////////////////////////////////////
	// Declare a instance native method 
	public native int NativeOpen();
	public native int NativeRead();
	public native int NativeWrite(String data);
	public native int NativeReadXA();
	public native int NativeCommitXA();
	public native int NativeCancelXA();
	public native int NativeClose();
	public native long NativeGetMsgLength();
	public native float NativeUsage();
	public native int NativeMoveFQ(String src_path, String src_qname, String dst_path, String dst_qname);
	public native int NativeResetFQ(String path, String qname);
	public native int NativeFlushFQ(String path, String qname);
	public native int NativeInfoFQ(String path, String qname);
	public native int NativeSkipFQ(String path, String qname);
	public native int NativeDiagFQ(String path, String qname);
	public native int NativeCreateFQ(String path, String qname);
	public native int NativeUnlinkFQ(String path, String qname);

	//////////////////////////////////////////////////////////
	// creater
    public FileQueueJNI( int id, String log_name, int log_level, String q_path, String q_name ) {
		out_stop_flag = 0;
		out_empty_flag = 0;
		out_seq = 0;
		out_run_time = 0;
		out_full_flag = 0;
		out_unlink_filename = null;
		out_msg = null;
		tid_index = id;
		logname = log_name;
		loglevel = log_level;
		path = q_path;
		qname = q_name;
	}

	///////////////////////////////////////////////////////////
	// instance methods
	// final method can't be overriding, If you want to override, remove final keyworkd.
    public final int open() {
		int  rc = NativeOpen();
		return(rc);
	}
	public int read() {
		int  rc = NativeRead();
		return(rc);
	}
	public int write(String data) {
		int  rc = NativeWrite(data);
		return(rc);
	}
	public int readXA() { 
		int  rc = NativeReadXA();
		return(rc);
	}
	public int commitXA() {
		int  rc = NativeCommitXA();
		return(rc);
	}
	public int cancelXA() { 
		int  rc = NativeCancelXA();
		return(rc);
	}
	public int close() { 
		int  rc = NativeClose();
		return(rc);
	}
	public long get_msg_length() {
		long rc = NativeGetMsgLength();
		return(rc);
	}
	public float usage() {
		float rc = NativeUsage();
		return(rc);
	}
	public int moveQ(String src_path, String src_qname, String dst_path, String dst_qname) {
		int rc = NativeMoveFQ(src_path, src_qname, dst_path, dst_qname);
		return(rc);
	}
	public int resetQ(String path, String qname) {
		int rc = NativeResetFQ(path, qname);
		return(rc);
	}
	public int flushQ(String path, String qname) {
		int rc = NativeFlushFQ(path, qname);
		return(rc);
	}
	public int infoQ(String path, String qname) {
		int rc = NativeInfoFQ(path, qname);
		return(rc);
	}
	public int skipQ(String path, String qname) {
		int rc = NativeSkipFQ(path, qname);
		return(rc);
	}
	public int diagQ(String path, String qname) {
		int rc = NativeDiagFQ(path, qname);
		return(rc);
	}
	public int createQ(String path, String qname) {
		int rc = NativeCreateFQ(path, qname);
		return(rc);
	}
	public int unlinkQ(String path, String qname) {
		int rc = NativeUnlinkFQ(path, qname);
		return(rc);
	}

	////////////////////////////////////////////////////
	// functions for getting class field data.
	// Getter
	public String get_out_msg() {
		return(out_msg);
	}
	public long get_out_seq() {
		return(out_seq);
	}
	public String get_out_unlink_filename() {
		return(out_unlink_filename);
	}
	public long get_out_run_time() {
		return(out_run_time);
	}

	// static methodes
	static void init()
	{
		return;
    }
} // class block end.
