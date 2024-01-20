import com.clang.fq.*;

public class CreateQ {
	static { 
		System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  	System.loadLibrary("fq");
   	}
	CreateQ() {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/skip.log", 1, "/home/ums/fq/enmq", "TST");

		rc = test.createQ("/home/ums/fq/enmq", "TST");	
		System.out.println("result rc = " + rc);

		test.close();
		return;
	} 
} // class block end.
