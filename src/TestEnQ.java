import com.clang.fq.*;

public class TestEnQ {
   static {
      System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
	  System.loadLibrary("fq");
   }
	TestEnQ () {

	}
 
	// Test Driver
	public static void main(String[] args) {
		int rc;

		System.out.println("args.length=" + args.length);
		for( int i=0; i<args.length; i++) {
			System.out.println(String.format("Command line argument %d is %s.", i , args[i]));
		}

		if( args.length != 2 ) {
			System.out.println("Usage  : $ java TestFQXA [qpath] [qname] <enter>\n");
			System.out.println("Example: $ java TestFQXA /ums/enmq GW-REAL-REQ <enter>\n");
			return;
		}

		String qpath = args[0];
		String qname = args[1];

		// make a FileQueueJNI instance with naming test.
		// 3-th argument is loglevel. (0: trace, 1: debug, 2: info, 3: Warning, 4: error, 5: emerg, 6: request)
		// Use 1 in dev and 4 real.
		FileQueueJNI test = new FileQueueJNI( 0, "/tmp/jni.log", 1, qpath, qname);

		rc = test.open(); // Open only when the process begins.
		if( rc < 0 ) {
			System.out.println("open failed: " + test.logname + "," + qpath + "," + qname + "," + " rc: " + rc);
			return;
		}
		System.out.println("open success: " + test.logname + "," + test.path + "," + test.qname + "," + " rc: " + rc);


		float usage;
		usage = test.usage();
		System.out.println("Current Usage: " + usage);


		// String kind_media = "LM"; /* kind_media  kakao:KA */
		String kind_media = "SM"; /* kind_media  kakao:KA */
		// String kind_media = "PU"; /* kind_media  Push:PU */
		// String kind_media = "EM"; /* kind_media  Email:EM */
		// String kind_media = "SM"; /* kind_media  SMS:Short Message */
		String phone_no = "01072021516";
		String send_msg = "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567";
		String template = "Hello %var1% ! my name is %var2%________________________________________________________________________________________________";
		String var_data = "Choi|Gwisang";

		String SendData = kind_media+phone_no+send_msg+template+var_data;

		System.out.println( "Send Data: " + SendData );

		while(true) {
			rc = test.write( SendData );
			if( rc < 0 ) {
				System.out.println("Write failed: " + test.path + "," + test.qname + "," + " rc: " + rc);
				test.close(); // Close when the process terminates.
				return;
			}
			else if( rc == 0 ) {
				System.out.println("full: " + test.path + "," + test.qname + "," + " rc: " + rc);

				try {
					Thread.sleep(1); // Pause for 1 second (1000)
				}
				catch(InterruptedException ex) {
				        Thread.currentThread().interrupt();
			    }
				continue; // Retry until sucesses.
			}
			else {
				long out_seq = test.get_out_seq();
				long out_run_time = test.get_out_run_time();

				System.out.println("Write success: " +  test.path + "," + test.qname + "," + " rc: " + rc + "," + " seq: " + out_seq + " run_time(micro seconds): " + out_run_time);
				break;
			}
		}

		test.close(); // Close only when the process terminates.
		return;
	} 
} // class block end.
