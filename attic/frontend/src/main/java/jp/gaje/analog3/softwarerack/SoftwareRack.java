package jp.gaje.analog3.softwarerack;

import java.io.IOException;
import java.util.Map;
import java.util.Map.Entry;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;
import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.PerspectiveBuilder;
import jp.gaje.analog3.module.PerspectiveComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;
import jp.gaje.analog3.module.WireComponent;
import jp.gaje.analog3.textconsole.TextConsole;

public class SoftwareRack extends Application {

    PerspectiveComponent perspective;
    SynthComponent currentComponent;
    
    Thread consoleThread;

    public static void main(String args[]) {
        launch(args);
    }
    
    @Override
    public void init() throws Exception {
        // bind synth
        try {
            perspective = new PerspectiveBuilder("hardware").build();
            currentComponent = perspective;
        } catch (final SynthComponentException ex) {
            SynthComponent component = ex.getComponent();
            if (component != null) {
                System.err.println("Component: " + component.getComponentType() + "." + component.getName());
            }
            System.err.println(ex.getMessage());
            System.exit(1);
        } catch (final ComponentBuildException ex) {
            ex.printStackTrace();
            System.err.println(ex.getMessage());
            System.exit(1);
        }
        
        // launch text console
        try {
            final TextConsole console = new TextConsole(perspective.getRoot(), true);
            consoleThread = new Thread() {
                @Override
                public void run() {
                    try {
                        console.runSession();
                        Platform.exit();
                    } catch (IOException ex) {
                        ex.printStackTrace();
                        System.err.println("Error in the console operation. Terminate the console and continue.");
                    }
                }
            };
            consoleThread.start();
        } catch (IOException ex) {
            ex.printStackTrace();
            System.err.println("Failed to start text console. Giving it up.");            
        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception {

        try {
        // Set perspective name as the title of the stage
        primaryStage.setTitle(perspective.getComponentType() + ":" + perspective.getName());

        // ////////////////
        // Make top layout

        // root group
        Group root = new Group();

        // grid for racks
        GridPane racksGrid = new GridPane();
        racksGrid.setAlignment(Pos.TOP_LEFT);
        racksGrid.setHgap(2);
        racksGrid.setVgap(10);
        racksGrid.setPadding(new Insets(25, 25, 25, 25));
        root.getChildren().add(racksGrid);

        // operation screen
        Group opScreen = new Group();
        root.getChildren().add(opScreen);

        // build the scene
        Scene scene = new Scene(root, 800, 700/* , Color.web("white", 1.0) */);
        primaryStage.setScene(scene);

        // ///////////
        // add racks
        Map<String, SynthComponent> synthComponents = perspective.getSubComponents();

        // Draw rack
        int x = 0;
        int y = 0;
        SynthComponent wiresComponent = null;
        for (Entry<String, SynthComponent> entry : synthComponents.entrySet()) {
            String name = entry.getKey();
            SynthComponent synthComponent = entry.getValue();
            if (synthComponent.getComponentType().equals(SynthComponent.TYPE_RACK)) {
                String type = synthComponent.getComponentType();
                if (type.equals(SynthComponent.TYPE_RACK)) {
                    racksGrid.add(new Rack(name, synthComponent, opScreen), x, y++);
                }
            } else if (synthComponent.getComponentType().equals(SynthComponent.TYPE_WIRES)) {
                wiresComponent = synthComponent;
            }
        }
        
        primaryStage.show();
        
        // Draw wires
        if (wiresComponent != null) {
            synthComponents = wiresComponent.getSubComponents();
            for (Entry<String, SynthComponent> entry : synthComponents.entrySet()) {
                String name = entry.getKey();
                SynthComponent synthComponent = entry.getValue();
                if (synthComponent instanceof WireComponent) {
                    WireComponent wireComponent = (WireComponent) synthComponent;
                    if (wireComponent != null) {
                        opScreen.getChildren().add(new Wire(wireComponent));
                    }
                }
            }
            
            Wires wires = new Wires(perspective, opScreen);
            wiresComponent.attachUserInterfaceComponent(wires);
        }
        }
        catch (Exception ex) {
            ex.printStackTrace();
            throw ex;
        }
    }
    
    @Override
    public void stop() throws Exception
    {
        System.exit(0);
    }
    
    // Utilities
    
    /**
     * Convert variable-name style name to title format.
     * 
     * Example: gateSync -> Gate Sync
     * 
     * @param source variable-name style title
     * @return Formatted title
     */
    static String makeTitle(String source) {
        StringBuilder sb = new StringBuilder();
        char prev = ' ';
        for (int index = 0; index < source.length(); ++index) {
            char ch = source.charAt(index);
            if (prev == ' ') {
                sb.append(Character.toUpperCase(ch));
            } else {
                if (Character.isUpperCase(ch)) {
                    if (prev != ' ' && !Character.isUpperCase(prev)) {
                        sb.append(' ');
                    }
                } else if (Character.isDigit(ch)) {
                    if (prev != ' ' && !Character.isDigit(prev)) {
                        sb.append(' ');
                    }
                }
                sb.append(ch);
            }
            prev = ch;
        }
        return sb.toString();
    }
}
