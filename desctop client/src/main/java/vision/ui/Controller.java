package vision.ui;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableRow;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import vision.logic.Device;
import vision.logic.SocketThread;

import java.io.File;

public class Controller {

    @FXML
    private ImageView video;
    @FXML
    private TableView devicesTableView;

    private static Controller instance;


    public void initialize(){
        instance = this;

        JavafxThread javafxThread = new JavafxThread();
        javafxThread.start();

        //initialize table view
        TableColumn<Device, String> deviceId = new TableColumn<>("Device id");
        deviceId.setCellValueFactory(new PropertyValueFactory<Device, String >("id"));
        deviceId.setPrefWidth(500);
        deviceId.setMinWidth(450);
        devicesTableView.getColumns().add(deviceId);


        //set double click listener
        devicesTableView.setRowFactory( tv -> {
            TableRow<Device> row = new TableRow<>();
            row.setOnMouseClicked(event -> {
                if (event.getClickCount() == 2 && (! row.isEmpty()) ) {
                    Device rowData = row.getItem();
                    System.out.println("Choose new showing" + rowData.getId());
                    JavafxThread.setActuallyChooseId(rowData.getId());
                    System.out.println("Choosed " + JavafxThread.getActuallyChooseId());
                }
            });
            return row ;
        });
    }


    //refresh list of connected clients
    public void showConnected() {
        devicesTableView.getItems().clear();
        ObservableList<Device> devices = FXCollections.observableArrayList();
        SocketThread.getConnectedClients().forEach(position -> devices.add(new Device(position)));
        devicesTableView.setItems(devices);
    }


    public void refreshImage(String clientId) {
        if(!clientId.equals(" ")) {
            File file = new File("i" + clientId + ".jpg");
            Image image = new Image(file.toURI().toString());
            video.setImage(image);
            video.setFitWidth(500.0);
            video.setFitHeight(560.0);
        }
    }


    public static Controller getInstance(){
        return instance;
    }
}
