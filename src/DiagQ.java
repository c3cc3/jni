import com.clang.fq.*;

public class DiagQ {
	static { 
		System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  	System.loadLibrary("fq");
   	}
	DiagQ() {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/diag.log", 1, "/home/ums/fq/enmq", "TST");

		rc = test.diagQ("/home/ums/fq/enmq", "TST");	
		System.out.println("result rc = " + rc);


		test.close();

		return;
	} 
} // class block end.
