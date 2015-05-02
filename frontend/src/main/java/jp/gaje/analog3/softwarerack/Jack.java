package jp.gaje.analog3.softwarerack;

import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Polygon;
import javafx.scene.transform.Transform;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.UserInterfaceComponent;

public class Jack extends GridPane implements Pluggable, UserInterfaceComponent {

    // synth property
    protected final PortComponent portComponent;
    protected final String title;
    protected final String path;
    protected final boolean isOutput;

    // Screen components
    protected Circle hole;
    protected Polygon nut;
    protected final static double size = 12.0;
    protected final static Double[] nutVertices = new Double[] { 
        -size * 1.0,  size * 0.6, 
                0.0,  size * 1.2,
         size * 1.0,  size * 0.6,
         size * 1.0, -size * 0.6,
                0.0, -size * 1.2,
        -size * 1.0, -size * 0.6 };
    
    protected double sceneX;
    protected double sceneY;

    Jack(PortComponent portComponent) {
        super();
        portComponent.attachUserInterfaceComponent(this);
        this.portComponent = portComponent;
        this.title = portComponent.getName();
        this.path = portComponent.getPath();
        this.isOutput = (portComponent.getDirection() == PortComponent.Direction.OUTPUT);
        makeView();
        
    }
    
    public String getTitle() {
        return title;
    }
    
    @Override
    public double getPlugPositionX() {
        Transform transform = hole.getLocalToSceneTransform();
        return transform.getTx();
    }
    
    @Override
    public double getPlugPositionY() {
        Transform transform = hole.getLocalToSceneTransform();
        return transform.getTy();
    }
    
    protected void makeView() {
        this.setAlignment(Pos.CENTER);
        this.setVgap(3);

        Label label = new Label(SoftwareRack.makeTitle(title));
        label.setTextFill(Color.web("white", 1.0));
        GridPane.setHalignment(label, HPos.CENTER);
        this.add(label, 0, 0);

        Group jack = new Group();
        nut = new Polygon();
        nut.getPoints().addAll(nutVertices);
        nut.setStroke(Color.web("white", 0.4));
        nut.setStrokeWidth(5);
        jack.getChildren().add(nut);

        hole = new Circle(10, Color.TRANSPARENT);
        Color color = isOutput ? Color.web("#AFF") : Color.web("#FAA");
        hole.setStroke(color);
        hole.setStrokeWidth(3);
        jack.getChildren().add(hole);
        
        hole.setOnMouseClicked(new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                System.out.println("label=" + title);
                Circle source = (Circle) event.getSource();
                System.out.println("x=" + event.getSceneX() + ", y=" + event.getSceneY());
                Jack jack = Jack.getJackFromEvent(event);
                System.out.println("x=" + jack.getPlugPositionX() + " y=" + jack.getPlugPositionY());
            }
        });
        hole.setOnMouseEntered(new EventHandler<MouseEvent>() {

            @Override
            public void handle(MouseEvent event) {
                focus();
            }

        });
        hole.setOnMouseExited(new EventHandler<MouseEvent>() {

            @Override
            public void handle(MouseEvent event) {
                unfocus();
            }

        });

        GridPane.setHalignment(jack, HPos.CENTER);
        this.add(jack, 0, 1);
    }

    protected void focus() {
        nut.setStroke(Color.web("orange"));
    }

    protected void unfocus() {
        nut.setStroke(Color.web("white", 0.4));
    }

    static Jack getJackFromEvent(Event event) {
        return (Jack) ((Node) event.getSource()).getParent().getParent(); 
    }

    @Override
    public void notifyParameterAdded(final String attributeName, final String newValue) {
        // TODO Auto-generated method stub
 
    }

    @Override
    public void notifySubComponentAdded(SynthComponent subComponent) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void notifySubComponentRemoved(SynthComponent subComponent) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void notifyParameterRemoved(String attributeName, String value) {
        // TODO Auto-generated method stub
        
    }
}
