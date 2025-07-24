module com.pip {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.fazecast.jSerialComm;

    opens com.pip to javafx.fxml;
    exports com.pip;
}
