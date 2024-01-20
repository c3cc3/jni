// Java code for thread creation by extending
// the Thread class

import com.clang.fq.*;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Map;
import java.util.HashMap;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import java.io.*; 
import java.util.*; 


class MultithreadingDemo_deQ extends Thread {
	static {
        System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
        System.loadLibrary("fq");
    }
    private FileQueueJNI fq;
	private long old_seq;

    public MultithreadingDemo_deQ (FileQueueJNI fq_obj) {
        this.fq = fq_obj;
    }

	// @Override
    public void run()
    {
		long myID = Thread.currentThread().getId();
        try {
			while(true) {
                int rc;
                rc = fq.readXA();
                if( rc < 0 ) {
                    System.out.println( myID + "->read failed: " + fq.path + "," + fq.qname + "," + " rc: " + rc);
                    break;
                }
                else if( rc == 0 ) {
                    // System.out.println( myID + "->empty: " + fq.path + "," + fq.qname + "," + " rc: " + rc);

                    try {
                        Thread.sleep(1000); // Pause for 1 second
                    }
                    catch(InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    }
                    continue;
                }
                else {
                    String data = fq.get_out_msg();
                    long out_seq = fq.get_out_seq();
                    String out_unlink_filename = fq.get_out_unlink_filename();
                    long out_run_time = fq.get_out_run_time();

					if( this.old_seq >  0 ) {
						if( this.old_seq +1 != out_seq ) {
							System.out.println( "sequence error!. skip" + "old_seq=" + this.old_seq + "out_seq=" + out_seq);
							System.exit(0);	
						}
					}
						
                    System.out.println(myID + "->read success: " +  fq.path + "," + fq.qname + "," + " rc: " + rc + " msg: " + data + " seq: " + out_seq + " unlink_filename: " + out_unlink_filename + " run_timme(micro seconds): " + out_run_time);

                    System.out.println(myID + "->seq=" + out_seq);

					data.trim();
					if( data.length() < 10) {
						System.out.println(myID + "There is no data in buffer." + "rc=" + rc);
						System.exit(0);
					}
					old_seq = out_seq;
					// Thread.sleep(10000);
                    // input your jobs in here
                    //
                    //
                    if( rc > 10) { // normal data
                        rc = fq.commitXA();
						if( rc != 0 ) {
							System.out.println(myID + "commitXA() failed." + "rc=" + rc);
							System.exit(0);
						}
                        System.out.println("commit success: rc: " + rc);
                    }
                    else { // abnormal data
                        rc = fq.cancelXA();
                        System.out.println("cancel success: rc: " + rc);
                        break;
                    }
                    continue;
                }
            }
        }
        catch (Exception e) {
            // Throwing an exception
            System.out.println("Exception is caught");
        }
    }
}
class MultithreadingDemo_enQ extends Thread {
	static {
        System.loadLibrary("jfq"); // Load native library at runtime
                                   // hello.dll (Windows) or libhello.so (Unixes)
        System.loadLibrary("fq");
    }
    private FileQueueJNI fq;

    public MultithreadingDemo_enQ (FileQueueJNI fq_obj) {
        this.fq = fq_obj;
    }

	// @Override
    public void run()
    {
		long myID = Thread.currentThread().getId();
        try {
			while(true) {
                int rc;
				String kind_media = "SM";
				// String kind_media = "LM";
				// String kind_media = "KA"; /* kakao */
				String phone_no = "01072021516";
				String send_msg = "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
				String template = "Hello %var1% ! my name is %var2%________________________________________________________________________________________________";
				String var_data = "Choi|Gwisang";

				String SendData = kind_media+phone_no+send_msg+template+var_data;

				// System.out.println( "Send Data: " + SendData );
				rc = fq.write( SendData );

				if( rc < 0 ) {
					System.out.println("Write failed: " + fq.path + "," + fq.qname + "," + " rc: " + rc);
					fq.close();
					return;
				}
				else if( rc == 0 ) {
					System.out.println("full: " + fq.path + "," + fq.qname + "," + " rc: " + rc);
					return;
				}
				else {
					long out_seq = fq.get_out_seq();
					long out_run_time = fq.get_out_run_time();

					// System.out.println("Write success: " +  fq.path + "," + fq.qname + "," + " rc: " + rc + "," + " seq: " + out_seq + " run_time(micro seconds): " + out_run_time);

					try {
						Thread.sleep(15); // Pause for 1 second (1000)
					}
					catch(InterruptedException ex) {
							Thread.currentThread().interrupt();
					}

					continue;
				}
            }
        }
        catch (Exception e) {
            // Throwing an exception
            System.out.println("Exception is caught");
        }
    }
}
 
// Main Class
public class Multithread_example {
    public static void main(String[] args)
    {
		int rc;
		int myid_1=0;
		int myid_2=1;

		System.out.println("args.length=" + args.length);
		for(int i = 0; i< args.length; i++) {
			System.out.println(String.format("Command Line Argument %d is %s", i, args[i]));
		}

/*
		if( args.length != 1) {
			System.out.println("Usage: $ java TestDeQXA [threads.map filename] <enter>");
			return;
		}

		String threads_map_filename = args[0];
*/

		// make a FileQueueJNI instance with naming test.
        // 3-th argument is loglevel. (0: trace, 1: debug, 2: info, 3: Warning, 4: error, 5: emerg, 6: request)
        // Use 1 in dev and 4 real.
        FileQueueJNI fq_obj = new FileQueueJNI( myid_1, "/tmp/jni_multi_thread_example.log", 4, "/home/ums/fq/enmq", "TST");
        FileQueueJNI fq_obj2 = new FileQueueJNI( myid_2, "/tmp/jni_multi_thread_example2.log", 4, "/home/ums/fq/enmq", "TST1");

		// TST 
        rc = fq_obj.open();
        if( rc < 0 ) {
            System.out.println("open failed: " + fq_obj.logname + "," + fq_obj.path + "," + fq_obj.qname + "," + " rc: " + rc);
            return;
        }
        System.out.println("open success: " + fq_obj.logname + "," + fq_obj.path + "," + fq_obj.qname + "," + " rc: " + rc);


		// TST1 
        rc = fq_obj2.open();
        if( rc < 0 ) {
            System.out.println("open failed: " + fq_obj2.logname + "," + fq_obj2.path + "," + fq_obj2.qname + "," + " rc: " + rc);
            return;
        }
        System.out.println("open success: " + fq_obj2.logname + "," + fq_obj2.path + "," + fq_obj2.qname + "," + " rc: " + rc);



		// TST
        MultithreadingDemo_deQ de_object1 = new MultithreadingDemo_deQ( fq_obj );
        de_object1.start();

        MultithreadingDemo_enQ en_object2 = new MultithreadingDemo_enQ( fq_obj );
        en_object2.start();

        MultithreadingDemo_enQ en_object3 = new MultithreadingDemo_enQ( fq_obj );
        en_object3.start();


		// TST1
        MultithreadingDemo_deQ de_object4 = new MultithreadingDemo_deQ( fq_obj2 );
        de_object4.start();

        MultithreadingDemo_enQ en_object5 = new MultithreadingDemo_enQ( fq_obj2 );
        en_object5.start();

    }
}

/*
public static Map<String, List<String>> aggregateByKeys(String filePath) {

    Map<String, List<String>> map = new HashMap<>();
    try (Stream<String> lines = Files.lines(Paths.get(filePath))) {
        lines.filter(line -> line.contains(":"))
          .forEach(line -> {
              String[] keyValuePair = line.split(":", 2);
              String key = keyValuePair[0];
              String value = keyValuePair[1];
              if (map.containsKey(key)) {
                  map.get(key).add(value);
              } else {
                  map.put(key, Stream.of(value).collect(Collectors.toList()));
              }
          });
    } catch (IOException e) {
        e.printStackTrace();
    }
    return map;
}
*/

/*
public static Map<String, String> byStream(String filePath, DupKeyOption dupKeyOption) {
    Map<String, String> map = new HashMap<>();
    try (Stream<String> lines = Files.lines(Paths.get(filePath))) {
        lines.filter(line -> line.contains(":"))
            .forEach(line -> {
                String[] keyValuePair = line.split(":", 2);
                String key = keyValuePair[0];
                String value = keyValuePair[1];
                if (DupKeyOption.OVERWRITE == dupKeyOption) {
                    map.put(key, value);
                } else if (DupKeyOption.DISCARD == dupKeyOption) {
                    map.putIfAbsent(key, value);
                }
            });
    } catch (IOException e) {
        e.printStackTrace();
    }
    return map;
}
*/

/*
public static Map<String, String> byBufferedReader(String filePath, DupKeyOption dupKeyOption) {
    HashMap<String, String> map = new HashMap<>();
    String line;
    try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
        while ((line = reader.readLine()) != null) {
            String[] keyValuePair = line.split(":", 2);
            if (keyValuePair.length > 1) {
                String key = keyValuePair[0];
                String value = keyValuePair[1];
                if (DupKeyOption.OVERWRITE == dupKeyOption) {
                    map.put(key, value);
                } else if (DupKeyOption.DISCARD == dupKeyOption) {
                    map.putIfAbsent(key, value);
                }
            } else {
                System.out.println("No Key:Value found in line, ignoring: " + line);
            }
        }
    } catch (IOException e) {
        e.printStackTrace();
    }
    return map;
}
*/
