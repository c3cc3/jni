import com.clang.fq.*;
import java.nio.CharBuffer;

public class TestEnQ_big_msg {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	TestEnQ_big_msg () {

	}
	public String spaces( int spaces ) {
	  return CharBuffer.allocate( spaces ).toString().replace( '\0', ' ' );
	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/jni.log", 1, "/home/ums/fq/enmq", "TST");

		rc = test.open();
		if( rc < 0 ) {
			System.out.println("open failed: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);


		float use;
		use = test.usage();
		System.out.println("Current Usage: " + use);

		// Make a big size sending message.

		// Warning : Current deQ buffer size is 65536
		rc = test.write("This is big Testing msg." + " ".repeat(60000) + "end");
		if( rc < 0 ) {
			System.out.println("Write failed: " + test.path + "," + test.qname + "," + " rc: " + rc);
			test.close();
			return;
		}
		else if( rc == 0 ) {
			System.out.println("full: " + test.path + "," + test.qname + "," + " rc: " + rc);
			test.close();
			return;
		}
		else {
			long out_seq = test.get_out_seq();
			long out_run_time = test.get_out_run_time();

			System.out.println("Write success: " +  test.path + "," + test.qname + "," + " rc: " + rc + "," + " seq: " + out_seq + " run_time(micro seconds): " + out_run_time);
			test.close();
			return;
		}
	} 
} // class block end.
