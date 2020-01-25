package vision.logic;

import vision.ui.Controller;

import java.util.ArrayList;
import java.util.List;

public class SocketThread extends Thread{

    private static List<String> connectedClients = new ArrayList<>();

    //run infinite loop with reading from socket
    @Override
    public void run() {
        for(;;) {
            try {
                System.out.println("Reading");
                String clientId = SocketConnector.getInstance().readEncodedString();
                Base64Converter.decodeBase64String(clientId);
                if (!connectedClients.contains(clientId)) {
                    connectedClients.add(clientId);
                    Controller.getInstance().showConnected();
                }
            } catch (Exception e){
                e.printStackTrace();
            }
        }
    }

    public static List<String> getConnectedClients() {
        return connectedClients;
    }

    public static void setConnectedClients(List<String> connectedClients) {
        SocketThread.connectedClients = connectedClients;
    }
}
