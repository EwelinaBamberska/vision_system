package sample.app;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import sample.logic.Base64Converter;
import sample.logic.SocketConnector;
import sample.logic.SocketThread;

import java.io.IOException;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception{
        Parent root = FXMLLoader.load(getClass().getResource("/sample/ui/sample.fxml"));
        primaryStage.setTitle("Hello World");
        primaryStage.setScene(new Scene(root, 1000, 800));
        primaryStage.show();
    }


    public static void main(String[] args) throws IOException {

        SocketThread socketThread = new SocketThread();

        SocketConnector socketConnector = new SocketConnector();
        socketConnector.sendToSocket();

        socketThread.start();

        launch(args);

        socketConnector.closeSocket();

//        while(true) {
//            socketConnector.readEncodedString();
//        }
//            Base64Converter.decodeBase64String("base64String.txt");
//        }

    }
}
