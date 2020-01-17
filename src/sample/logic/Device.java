package sample.logic;

import javafx.beans.property.SimpleStringProperty;

public class Device {
    private final SimpleStringProperty id;

    public Device(String id) {
        this.id = new SimpleStringProperty(id);
    }

    public String getId() {
        return id.get();
    }

    public SimpleStringProperty idProperty() {
        return id;
    }

    public void setId(String id) {
        this.id.set(id);
    }
}
