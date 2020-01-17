package sample.logic;

import sample.ui.Controller;
import sample.ui.JavafxThread;

import java.util.ArrayList;
import java.util.List;

public class SocketThread extends Thread{

    private static List<String> connectedClients = new ArrayList<>();
    private static boolean initialized = false;

    @Override
    public void run() {
        for(;;) {
            try {
//                System.out.println(initialized);
//                if(initialized) {
                    System.out.println("Reading");
                    String clientId = SocketConnector.getInstance().readEncodedString();
                    Base64Converter.decodeBase64String(clientId);
                    if (!connectedClients.contains(clientId)) {
                        connectedClients.add(clientId);
                        Controller.getInstance().showConnected();
                    }
//                }
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

    public static boolean isInitialized() {
        return initialized;
    }

    public static void setInitialized(boolean initialized) {
        SocketThread.initialized = initialized;
    }
}
