package vision.app;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import vision.logic.SocketConnector;
import vision.logic.SocketThread;

import java.io.IOException;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
        Parent root = FXMLLoader.load(getClass().getResource("/sample.fxml"));
        primaryStage.setTitle("Hello World");
        primaryStage.setScene(new Scene(root, 1000, 600));
        primaryStage.setOnCloseRequest(new EventHandler<WindowEvent>() {
            @Override
            public void handle(WindowEvent t) {
                System.exit(0);
            }
        });
        primaryStage.show();
    }


    public static void main(String[] args) throws IOException {
        SocketThread socketThread = new SocketThread();

        //connect to server
        SocketConnector socketConnector = new SocketConnector();
        socketConnector.setIp_address(args[0]);
        socketConnector.setPort(Integer.valueOf(args[1]));
        socketConnector.sendToSocket();

        //start thread with reading data from server
        socketThread.start();

        launch(args);

        socketConnector.closeSocket();
    }
}
