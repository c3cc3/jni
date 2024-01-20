import com.clang.fq.*;

public class TestShm_deQ {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	TestShm_deQ () {

	}
 
	// TestShm_deQ Driver
	public static void main(String[] args) {
		int rc;

		System.out.println("args.length=" + args.length);
		for(int i = 0; i< args.length; i++) {
			System.out.println(String.format("Command Line Argument %d is %s", i, args[i]));
		}
		if( args.length != 3) {
			System.out.println("Usage: $ java TestShm_deQ [qpath] [qname] [user_working_time] <enter>");
			System.out.println("Usage: $ java TestShm_deQ /home/ums/fq/enmq SHM_TST 1000 : 1000 -> 1 second <enter>");
			return;
		}
		String qpath = args[0];
		String qname = args[1];
		String user_working_time_str = args[2];

		int user_working_time_int = Integer.parseInt(user_working_time_str);

		// make a FileQueueJNI instance with naming test.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/jni.log", 4, qpath, qname);

		rc = test.open_shm();
		if( rc < 0 ) {
			System.out.println("open failed: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);

		for(;;) { // polling file queue.
			rc = test.read();
			if( rc < 0 ) {
				System.out.println("read failed: " + test.path + "," + test.qname + "," + " rc: " + rc);
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

				// System.out.println("read success: " +  test.path + "," + test.qname + "," + " rc: " + rc + " msg: " + data + " seq: " + out_seq + " unlink_filename: " + out_unlink_filename + " run_timme(micro seconds): " + out_run_time);
				 System.out.println("read success: " + " rc: " + rc);

				////////////////////////////////////////////////////////////////////////////////////
				// Type your job code in here
				//
				///////////////////////////////////////////////////////////////////////////////////

				if( rc > 10) { // normal data
					System.out.println("commit success: rc: " + rc);
				}
				else { // abnormal data
					System.out.println("cancel success: rc: " + rc);
					break;
				}
				continue;
			}
		}
		test.close_shm();
	} 
} // class block end.
