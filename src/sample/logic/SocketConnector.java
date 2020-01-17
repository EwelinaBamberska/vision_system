package sample.logic;


import java.net.*;
import java.io.*;

public class SocketConnector {
    private static SocketConnector socketConnector;
    private Socket javaSocket;
    private int port = 1234;
    private String ip_address = "192.168.1.23";
    private BufferedReader in;
    private OutputStream out;

    public SocketConnector() throws IOException {
        socketConnector = this;
        javaSocket = new Socket();
    }

    public void sendToSocket(){
        try {
            ((Socket) javaSocket).connect(new InetSocketAddress(ip_address, port), 3000);
            System.out.println("Connected");
            PrintWriter outs =
                    new PrintWriter(javaSocket.getOutputStream(), true);
            outs.println("s");
            outs.flush();

            in = new BufferedReader(
                            new InputStreamReader(javaSocket.getInputStream()));
            SocketThread.setInitialized(true);
            System.out.println("Send");
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public String readEncodedString()  {
        String clientNumber = "";
        System.out.println("start of reading");
        try {
            String stringLength = in.readLine();
            while(!stringLength.contains("%")) {
                stringLength = in.readLine();
            }
            clientNumber = stringLength.trim();
            System.out.println(clientNumber);

            out = new FileOutputStream("c" + clientNumber + ".txt");

            stringLength = in.readLine();
            int len = Integer.valueOf(stringLength.trim());
            System.out.println("Length " + len);


            String line = in.readLine();
            while(true) {
                int index = line.indexOf("/");
                if (index == -1)
                    line = in.readLine();
                else{
                    line = line.substring(index);
                    break;
                }
            }
            int actu = 0;
            actu = line.length() + 2;
            System.out.println("After " + line + " " + actu);

            while (true) {
                out.write(line.getBytes());
                out.write("\n".getBytes());
                if(actu >= len){
                    break;
                }
                line = in.readLine();
                actu += line.length() + 2;
            }
            out.flush();
            out.close();
            System.out.println("read all");

        }
        catch (IOException e){
            e.printStackTrace();
        }
        return clientNumber;
    }

    public static SocketConnector getInstance(){
        return socketConnector;
    }


    public void closeSocket(){
        try {
            in.close();
            out.close();
            javaSocket.close();
        } catch (IOException ex){
            ex.printStackTrace();
        }
    }
}
