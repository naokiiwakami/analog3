package jp.gaje.analog3.softwarerack;

import java.util.Set;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import jp.gaje.analog3.module.SelectorComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class Selector extends GridPane {

    String title;
    String initialValue;

    final Set<String> choices;
    
    final SelectorComponent selectorComponent;
    
    ComboBox<String> optionsBox;

    public Selector(SelectorComponent selector) {
        this.title = selector.getName();
        this.initialValue = selector.getValue();

        this.choices = selector.getChoices();
        
        this.selectorComponent = selector;
        
        makeView();
    }
    
    static Selector getFromEvent(Event event) {
        return (Selector) ((Node) event.getSource()).getParent();
    }

    protected void makeView() {
        this.setAlignment(Pos.CENTER);
        this.setVgap(3);

        Label label = new Label(SoftwareRack.makeTitle(title) + ":");
        label.setTextFill(Color.web("white", 1.0));
        GridPane.setHalignment(label, HPos.LEFT);
        this.add(label, 0, 0);

        ObservableList<String> data = FXCollections
                .observableArrayList(choices);
        optionsBox = new ComboBox<String>(data);
        optionsBox.setValue(initialValue);
        optionsBox.setStyle(".list-cell" + "{" + "-fx-background: transparent;"
                + "-fx-background-color: #ccc;" + "-fx-text-fill: #fff;}");
        this.add(optionsBox, 0, 1);
        optionsBox.setOnAction(optionsActionHandler);

    }
    
    void commit() throws SynthComponentException {
        String value = optionsBox.getValue();
        selectorComponent.setValue(value);
    }

    static EventHandler<ActionEvent> optionsActionHandler = new EventHandler<ActionEvent>() {

        @Override
        public void handle(ActionEvent event) {
            Selector selector = Selector.getFromEvent(event);
            try {
                selector.commit();
            } catch (SynthComponentException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    };

    protected void focus() {
    }

    protected void unfocus() {
    }

}
