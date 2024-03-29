import java.net.*;
import java.io.*;
 
/**
 * This program demonstrates a client socket application that connects
 * to a Whois server to get information about a domain name.
 * @author www.codejava.net
 */
public class WhoisClient {
 
    public static void main(String[] args) {
        if (args.length < 1) return;
 
        String domainName = args[0];
 
        String hostname = "172.25.28.34";
        int port = 6666;
 
        try (Socket socket = new Socket(hostname, port)) {
 
            OutputStream output = socket.getOutputStream();
            PrintWriter writer = new PrintWriter(output, true);
            writer.println(domainName);
 
            InputStream input = socket.getInputStream();
 
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
 
            String line;
 
            while ((line = reader.readLine()) != null) {
                System.out.println(line);
            }
        } catch (UnknownHostException ex) {
 
            System.out.println("Server not found: " + ex.getMessage());
 
        } catch (IOException ex) {
 
            System.out.println("I/O error: " + ex.getMessage());
        }
    }
}
