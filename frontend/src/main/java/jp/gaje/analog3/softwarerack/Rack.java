package jp.gaje.analog3.softwarerack;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import javafx.geometry.HPos;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import jp.gaje.analog3.module.KnobComponent;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SelectorComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.ModuleComponent;

public class Rack extends GridPane {

    final Group opScreen;

    protected final String rackName;
    protected final SynthComponent rack;

    protected final GridPane panelsGrid;
    
    protected final Map<String, Jack> jackResolver;

    public Rack(String rackName, SynthComponent rack, Group opScreen) {
        super();
        this.opScreen = opScreen;

        this.rackName = rackName;
        this.rack = rack;

        panelsGrid = new GridPane();
        panelsGrid.setHgap(2);
        
        jackResolver = new HashMap<String, Jack>();

        makeView();
    }

    protected void makeView() {
        // make the rack layout
        this.setAlignment(Pos.CENTER_LEFT);
        this.setStyle("-fx-background-color: #CCC;");
        this.setVgap(3);
        
        Label title = new Label(SoftwareRack.makeTitle(rackName));
        title.setTextFill(Color.BLACK);
        this.add(title, 0, 0);

        this.add(panelsGrid, 0, 1);

        // add panels
        int x = 0;
        for (Entry<String, SynthComponent> entry : rack.getSubComponents().entrySet()) {
            String moduleName = entry.getKey();
            SynthComponent module = entry.getValue();
            if (module instanceof ModuleComponent) {
                makeModulePanel(panelsGrid, moduleName, module, x++);
            }                
        }
    }

    protected void makeModulePanel(GridPane panelsGrid, String moduleName,
            SynthComponent module, int panelColumn) {
        // make label grid
        GridPane labelGrid = new GridPane();
        labelGrid.setStyle("-fx-background-color: black;");
        labelGrid.setAlignment(Pos.CENTER);
        labelGrid.setPadding(new Insets(3, 3, 0, 3));
        Label label = new Label(moduleName);
        label.setTextFill(Color.web("white", 1.0));
        label.setFont(Font.font("Arial", FontWeight.NORMAL, 15));
        GridPane.setHalignment(label, HPos.CENTER);
        labelGrid.add(label, 0, 0);
        panelsGrid.add(labelGrid, panelColumn, 0);

        // make knobs grid
        GridPane knobsGrid = new GridPane();
        knobsGrid.setAlignment(Pos.CENTER);
        knobsGrid.setStyle("-fx-background-color: black;");
        knobsGrid.setPadding(new Insets(7, 3, 7, 3));
        knobsGrid.setHgap(2);
        knobsGrid.setVgap(10);

        int knobRow = 0;

        panelsGrid.add(knobsGrid, panelColumn, 1);

        // make jacks grid
        GridPane jacksGrid = new GridPane();
        jacksGrid.setAlignment(Pos.BOTTOM_CENTER);
        jacksGrid.setPadding(new Insets(0, 3, 3, 3));
        jacksGrid.setHgap(10);
        jacksGrid.setVgap(10);
        jacksGrid.setStyle("-fx-background-color: black;");
        panelsGrid.add(jacksGrid, panelColumn, 2);

        int jackX = 0;
        int jackY = 0;
        boolean wasInput = true;

        for (Entry<String, SynthComponent> entry : module.getSubComponents().entrySet()) {
            SynthComponent subComponent = entry.getValue();
            if (subComponent instanceof PortComponent) {
                PortComponent port = (PortComponent) subComponent;
                boolean isOutput = false;
                String direction = subComponent.getAttribute("direction");
                Jack jack = new Jack(port);
                System.out.println("jack path = " + port.getPath());
                jackResolver.put(port.getPath(), jack);
                if (direction != null) {
                    isOutput = direction.equals("OUTPUT");
                }
                if (isOutput) {
                    if (wasInput) {
                        ++jackX;
                        jackY = 0;
                        wasInput = false;
                    }
                    jacksGrid.add(jack, jackX, jackY++);
                } else {
                    jacksGrid.add(jack, jackX, jackY++);
                }
                if (jackY == 2) {
                    jackY = 0;
                    ++jackX;
                }
            } else if (subComponent instanceof SelectorComponent) {
                SelectorComponent selector = (SelectorComponent) subComponent;
                knobsGrid.add(new Selector(selector), 0, knobRow++);
            } else if (subComponent instanceof KnobComponent) {
                KnobComponent knobComponent = (KnobComponent) subComponent;
                knobsGrid.add(new Knob(knobComponent, opScreen), 0, knobRow++);
            }
        }


    }
}
