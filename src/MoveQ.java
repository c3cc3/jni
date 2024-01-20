import com.clang.fq.*;

public class MoveQ {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	MoveQ () {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/move.log", 1, "/home/ums/fq/enmq", "TST");

		rc = test.moveQ("/home/ums/fq/enmq", "LG_SM", "/home/ums/fq/enmq", "KT");	
		System.out.println("result rc = " + rc);


		test.close();
		return;
	} 
} // class block end.
