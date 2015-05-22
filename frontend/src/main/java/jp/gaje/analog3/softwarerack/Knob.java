package jp.gaje.analog3.softwarerack;

import java.text.DecimalFormat;
import java.util.Map.Entry;

import javafx.application.Platform;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.geometry.Pos;
import javafx.scene.Cursor;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Arc;
import javafx.scene.shape.Circle;
import javafx.scene.shape.StrokeType;
import javafx.scene.text.Font;
import javafx.scene.text.TextAlignment;
import javafx.scene.transform.Transform;
import jp.gaje.analog3.module.KnobComponent;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.UserInterfaceComponent;

public class Knob extends GridPane implements Pluggable, UserInterfaceComponent {

    // The operation screen
    final Group opScreen;

    // synth property
    protected final String title;
    final protected double scale;

    // Screen components
    Arc arc;
    Arc point;
    Circle circle;

    // constants
    final double radiusForCircle = 15.0;
    final double radiusForArc = 20.0;
    final double radiusForPoint = 12.0;
    final double pointWidth = 5.0f;
    final double minAngle = 215.0f;
    final double maxAngle = -35.0f;
    Font originalFont;
    double originalFontSize;

    protected final KnobComponent knobComponent;

    public Knob(KnobComponent knobComponent, Group opScreen) {
        super();
        this.opScreen = opScreen;
        this.title = knobComponent.getName();

        this.scale = knobComponent.getScale();

        knobComponent.attachUserInterfaceComponent(this);
        this.knobComponent = knobComponent;

        // Attach to port if the knobComponent has some.
        for (Entry<String, SynthComponent> entry : knobComponent.getSubComponents().entrySet()) {
            SynthComponent subComponent = entry.getValue();
            if (subComponent instanceof PortComponent) {
                subComponent.attachUserInterfaceComponent(this);
            }
        }

        makeView();
    }

    protected double orgSceneX, orgSceneY;
    protected Circle testCircle = new Circle(50, Color.RED);
    protected double originalValue;

    static private DecimalFormat df = new DecimalFormat("#");

    Label valueLabel = null;

    Label getValueLabel() {
        return valueLabel;
    }

    void setOrgScene(double x, double y) {
        orgSceneX = x;
        orgSceneY = y;
    }

    double getOrgSceneX() {
        return orgSceneX;
    }

    double getOrgSceneY() {
        return orgSceneY;
    }

    void setOriginalValue(double value) {
        originalValue = value;
    }

    double getOriginalValue() {
        return originalValue;
    }

    void checkpointValue() {
        originalValue = knobComponent.getValue();
    }

    Group getOpScreen() {
        return opScreen;
    }

    protected int eventCount = 0;

    long timePreviousClick = 0;

    long getClickInterval() {
        long now = System.currentTimeMillis();
        long interval = now - timePreviousClick;
        timePreviousClick = now;
        return interval;
    }

    protected void makeView() {
        this.setAlignment(Pos.CENTER);
        this.setVgap(1);

        Label label = new Label(SoftwareRack.makeTitle(title));
        label.setTextFill(Color.web("white", 1.0));
        originalFont = label.getFont();
        originalFontSize = originalFont.getSize();
        GridPane.setHalignment(label, HPos.CENTER);
        this.add(label, 0, 0);

        Group knob = new Group();
        arc = new Arc(0.0f, 0.0f, radiusForArc, radiusForArc, maxAngle,
                minAngle - maxAngle);
        arc.setStroke(Color.web("white", 0.5));
        arc.setStrokeWidth(3);

        circle = new Circle(radiusForCircle, Color.web("black", 0.3));
        circle.setStrokeType(StrokeType.INSIDE);
        circle.setStroke(Color.web("white", 1.0));
        circle.setStrokeWidth(2);

        double pointAngle = minAngle + (maxAngle - minAngle) * (getValue() / scale)
                - pointWidth / 2;
        point = new Arc(0.0f, 0.0f, radiusForPoint, radiusForPoint, pointAngle,
                pointWidth);
        point.setStroke(Color.web("red"));
        point.setStrokeWidth(4);

        valueLabel = new Label(df.format(getValue()));
        valueLabel.setTextFill(Color.WHITE);
        valueLabel.setTextAlignment(TextAlignment.RIGHT);
        valueLabel.setTranslateX(-13);
        valueLabel.setTranslateY(-7);
        valueLabel.setVisible(false);

        knob.getChildren().add(arc);
        knob.getChildren().add(circle);
        knob.getChildren().add(point);
        knob.getChildren().add(valueLabel);
        GridPane.setHalignment(knob, HPos.CENTER);
        this.add(knob, 0, 1);

        circle.setCursor(Cursor.HAND);

        // Set event handlers
        circle.setOnMousePressed(circleOnMousePressedEventHandler);
        circle.setOnMouseDragged(circleOnMouseDraggedEventHandler);
        circle.setOnMouseReleased(circleOnMouseReleasedEventHandler);

        valueLabel.setOnMousePressed(circleOnMousePressedEventHandler);
        valueLabel.setOnMouseDragged(circleOnMouseDraggedEventHandler);
        valueLabel.setOnMouseReleased(circleOnMouseReleasedEventHandler);

        circle.setOnMouseEntered(focusEventHandler);
        circle.setOnMouseExited(unfocusEventHandler);
        circle.setOnScroll(circleOnScrollEventHandler);

        valueLabel.setOnMouseEntered(focusEventHandler);
        valueLabel.setOnMouseExited(unfocusEventHandler);
        valueLabel.setOnScroll(circleOnScrollEventHandler);

    }

    static Knob getKnobFromEvent(Event event) {
        return (Knob) ((Group) ((Node) event.getSource()).getParent())
                .getParent();
    }

    static EventHandler<MouseEvent> circleOnMousePressedEventHandler = new EventHandler<MouseEvent>() {

        @Override
        public void handle(MouseEvent event) {

            Knob knob = Knob.getKnobFromEvent(event);

            double sceneX = event.getSceneX();
            double sceneY = event.getSceneY();
            knob.setOrgScene(sceneX, sceneY);

            knob.checkpointValue();

            Label valueLabel = knob.getValueLabel();
            valueLabel.setVisible(true);
            // knob.getOpScreen().getChildren().add(valueLabel);
        }
    };

    static EventHandler<MouseEvent> circleOnMouseDraggedEventHandler = new EventHandler<MouseEvent>() {

        @Override
        public void handle(MouseEvent event) {
            Knob knob = Knob.getKnobFromEvent(event);

            double offsetX = event.getSceneX() - knob.getOrgSceneX();
            double offsetY = event.getSceneY() - knob.getOrgSceneY();

            final double sensitivity = 5.0;

            double newValue = knob.getOriginalValue() + offsetX * sensitivity;

            knob.updateValue(newValue, true);
            knob.focus();

        }
    };

    static EventHandler<MouseEvent> circleOnMouseReleasedEventHandler = new EventHandler<MouseEvent>() {

        @Override
        public void handle(MouseEvent event) {

            Knob knob = Knob.getKnobFromEvent(event);

            double sceneX = event.getSceneX();
            double sceneY = event.getSceneY();
            knob.setOrgScene(sceneX, sceneY);

            knob.checkpointValue();

            Label valueLabel = knob.getValueLabel();
            // knob.getOpScreen().getChildren().remove(valueLabel);

            knob.unfocus();
        }
    };

    static EventHandler<ScrollEvent> circleOnScrollEventHandler = new EventHandler<ScrollEvent>() {

        @Override
        public void handle(ScrollEvent event) {
            Knob knob = Knob.getKnobFromEvent(event);

            // calculate sensitivity
            long interval = knob.getClickInterval();
            final double perClick = 1.0 / 40.0;
            double sensitivity = 200.0 / interval;
            if (sensitivity <= 1.0) {
                sensitivity = 1.0;
            }

            // double newValue = knob.getValue() + sensitivity *
            // event.getDeltaY();
            double newValue = knob.getValue() + perClick * sensitivity * event.getDeltaY();
            knob.updateValue(newValue, true);
            knob.focus();
        }
    };

    static EventHandler<MouseEvent> focusEventHandler = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent event) {
            Knob knob = getKnobFromEvent(event);
            knob.focus();
        }
    };

    static EventHandler<MouseEvent> unfocusEventHandler = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent event) {
            Knob knob = getKnobFromEvent(event);
            knob.unfocus();
        }
    };

    protected void focus() {
        arc.setStroke(Color.ORANGE);
        valueLabel.setVisible(true);
    }

    protected void unfocus() {
        arc.setStroke(Color.web("white", 0.5));
        valueLabel.setVisible(false);
    }

    public double getValue() {
        return knobComponent.getValue();
    }

    public void setValue(double value) {
        knobComponent.setValue(value);
    }

    void updateValue(double newValue, boolean updateSynthComponent) {
        if (newValue > scale) {
            newValue = scale;
        }
        if (newValue < 0.0) {
            newValue = 0.0;
        }
        double pointAngle = minAngle + (maxAngle - minAngle) * newValue / scale
                - pointWidth / 2;
        point.setStartAngle(pointAngle);

        if (valueLabel != null) {
            valueLabel.setText(df.format(newValue));
        }

        if (updateSynthComponent) {
            setValue(newValue);
        }
    }

    @Override
    public double getPlugPositionX() {
        Transform transform = circle.getLocalToSceneTransform();
        return transform.getTx();
    }

    @Override
    public double getPlugPositionY() {
        Transform transform = circle.getLocalToSceneTransform();
        return transform.getTy();
    }

    @Override
    public void notifyParameterAdded(final String attributeName, final String newValue) {
        if (!Platform.isFxApplicationThread() && "value".equals(attributeName)) {
            Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    arc.setStroke(Color.ORANGE);
                    updateValue(Double.parseDouble(newValue), false);
                    arc.setStroke(Color.web("white", 0.5));
                }
            });
        }
    }

    @Override
    public void notifySubComponentAdded(SynthComponent subComponent) {
        if (!Platform.isFxApplicationThread() && subComponent instanceof PortComponent) {
            subComponent.attachUserInterfaceComponent(this);
        }
    }

    @Override
    public void notifySubComponentRemoved(SynthComponent subComponent) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void notifyParameterRemoved(String attributeName, final Object value) {
        // TODO Auto-generated method stub
        
    }
}
