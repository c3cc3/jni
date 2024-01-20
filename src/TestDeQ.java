import com.clang.fq.*;

/*
** Warning: max buffer size is 65536
*/

public class TestDeQ {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	TestDeQ () {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		// make a FileQueueJNI instance with naming test.
		// 3-th argument is loglevel. (0: trace, 1: debug, 2: info, 3: Warning, 4: error, 5: emerg, 6: request)
		// Use 1 in dev and 4 real.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/jni_TestDeQ.log", 4, "/home/ums/fq/enmq", "TST");

		rc = test.open();
		if( rc < 0 ) {
			System.out.println("open failed: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);

		for(;;) { // polling file queue.

			rc = test.read();
			if( rc < 0 ) {
				if( rc == -99 ) {
					System.out.println("Manager Stop: " + test.path + "," + test.qname);
					try {
						Thread.sleep(1000); // Pause for 1 second
					}
					catch(InterruptedException ex) {
						Thread.currentThread().interrupt();
					}
					continue;
				}
				else {
					System.out.println("readXA failed: " + test.path + "," + test.qname + "," + " rc: " + rc);
					break;
				}
			}
			else if( rc == 0 ) {
				System.out.println("empty: " + test.path + "," + test.qname + "," + " rc: " + rc);

				try {
                    Thread.sleep(1000); // Pause for 1 second
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

				System.out.println( "rc=" + rc);
				System.out.println("read success: " +  test.path + "," + test.qname + "," + " rc: " + rc + " msg: " + data + " seq: " + out_seq + " unlink_filename: " + out_unlink_filename + " run_timme(micro seconds): " + out_run_time);

				// input your jobs in here
				//
				//

/*
				if( rc > 10) { // normal data
					rc = test.commitXA();
					System.out.println("commit success: rc: " + rc);
				}
				else { // abnormal data
					rc = test.cancelXA();
					System.out.println("cancel success: rc: " + rc);
					break;
				}
*/
				continue;
			}
		}
		test.close();
	} 
} // class block end.
