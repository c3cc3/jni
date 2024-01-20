import com.clang.fq.*;

public class Test {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	Test () {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/jni.log", 1, "/home/ums/fq/enmq", "TST");
		FileQueueJNI test1 = new FileQueueJNI( 1, "/tmp/jni.log", 1, "/home/ums/fq/enmq", "TST1");

		rc = test.open();
		if( rc < 0 ) {
			System.out.println("open failed: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);

		rc = test1.open();
		if( rc < 0 ) {
			System.out.println("open failed: " + test1.logname + "," + test1.path + "," + test1.qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test1.logname + "," + test1.path + "," + test1.qname + "," + " rc: " + rc);

		for(;;) { // polling file queue.

			rc = test.readXA();
			if( rc < 0 ) {
				System.out.println("readXA failed: " + test.path + "," + test.qname + "," + " rc: " + rc);
				break;
			}
			else if( rc == 0 ) {
				System.out.println("empty: " + test.path + "," + test.qname + "," + " rc: " + rc);

				try {
                    Thread.sleep(100); // Pause for 1 second
                }
                catch(InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }
                continue;
			}
			else {
				String data = test.get_out_msg();
				long out_seq = test.get_out_seq();
				String out_unlink_filename = test.get_out_unlink_filename();
				long out_run_time = test.get_out_run_time();

				System.out.println("read success: " +  test.path + "," + test.qname + "," + " rc: " + rc + " msg: " + data + " seq: " + out_seq + " unlink_filename: " + out_unlink_filename + " run_timme(micro seconds): " + out_run_time);

				////////////////////////////////////////////////////////////////////////////////////
				// Type your job code in here
				//
				///////////////////////////////////////////////////////////////////////////////////

				if( rc > 10) { // normal data
					rc = test.commitXA();
					System.out.println("commit success: rc: " + rc);
				}
				else { // abnormal data
					rc = test.cancelXA();
					System.out.println("cancel success: rc: " + rc);
					break;
				}
				continue;
			}
		}
		test.close();
	} 
} // class block end.
