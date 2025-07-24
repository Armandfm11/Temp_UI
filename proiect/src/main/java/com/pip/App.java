package com.pip;

import com.fazecast.jSerialComm.SerialPort;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.io.IOException;
import java.io.InputStream;
import java.util.LinkedList;
import java.util.Queue;

public class App extends Application {
    private static Scene scene;

    private XYChart.Series<Number, Number> series;
    private int xSeriesData = 0;
    private Label averageLabel;
    private final Queue<Double> lastTemps = new LinkedList<>();

    @Override
    public void start(Stage primaryStage) {
        NumberAxis xAxis = new NumberAxis();
        xAxis.setLabel("Timp (s)");

        NumberAxis yAxis = new NumberAxis();
        yAxis.setLabel("Temperatura (°C)");

        LineChart<Number, Number> lineChart = new LineChart<>(xAxis, yAxis);
        lineChart.setTitle("Temperatura");
        lineChart.setAnimated(false);

        series = new XYChart.Series<>();
        series.setName("Temp.");
        lineChart.getData().add(series);

        averageLabel = new Label("Medie: -- °C");
        averageLabel.setStyle("-fx-font-size: 14px; -fx-text-fill: #888; -fx-padding: 0 0 0 6;");

        VBox root = new VBox(0, lineChart, averageLabel); 
        VBox.setVgrow(lineChart, Priority.ALWAYS);

        primaryStage.setScene(new Scene(root, 800, 600));
        primaryStage.setTitle("Health App - Grafic");
        primaryStage.show();

        new Thread(this::startSerialReader, "Serial-Thread").start();
    }

    public static void setRoot(String fxml) throws IOException {
        scene.setRoot(loadFXML(fxml));
    }

    private static Parent loadFXML(String fxml) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource(fxml + ".fxml"));
        return fxmlLoader.load();
    }

    private void startSerialReader() {
        SerialPort comPort = SerialPort.getCommPort("COM5");
        comPort.setBaudRate(9600);
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);

        if (!comPort.openPort()) {
            Platform.runLater(() -> {
                Alert alert = new Alert(Alert.AlertType.ERROR);
                alert.setTitle("Eroare conexiune");
                alert.setHeaderText("Nu s-a putut deschide portul serial!");
                alert.setContentText("Verificați conexiunea la dispozitiv și portul COM.");
                alert.showAndWait();
            });
            return;
        }

        try {
            InputStream in = comPort.getInputStream();
            StringBuilder buffer = new StringBuilder();

            while (true) {
                while (in.available() > 0) {
                    int data = in.read();
                    if (data == -1) continue;

                    char c = (char) data;
                    if (c == '\n') {
                        String line = buffer.toString().trim();
                        buffer.setLength(0); // clear buffer
                        try {
                            double temp = Double.parseDouble(line);
                            Platform.runLater(() -> {
                                series.getData().add(new XYChart.Data<>(xSeriesData++, temp));
                                if (series.getData().size() > 100) {
                                    series.getData().remove(0);
                                }
                                lastTemps.add(temp);
                                if (lastTemps.size() > 100) lastTemps.poll();
                                double avg = lastTemps.stream().mapToDouble(Double::doubleValue).average().orElse(0.0);
                                averageLabel.setText(String.format("Medie: %.2f °C", avg));
                            });
                        } catch (NumberFormatException e) {
                            System.err.println("Invalid line: " + line);
                        }
                    } else {
                        buffer.append(c);
                    }
                }

                Thread.sleep(20); 
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            comPort.closePort();
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
