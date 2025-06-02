package jp.gaje.analog3.softwarerack;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javafx.application.Platform;
import javafx.event.Event;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.paint.Color;
import javafx.scene.shape.CubicCurve;
import javafx.scene.shape.StrokeLineCap;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.UserInterfaceComponent;
import jp.gaje.analog3.module.WireComponent;

public class Wire extends Group implements UserInterfaceComponent
{

    // synth property
    protected final WireComponent wireComponent;

    // Screen components
    // private static Color[] colors = { Color.web("F5F"), Color.web("#5F5"),
    // Color.web("#99F"), Color.web("#FF5") };
    // private static Color[] colors = { Color.BROWN, Color.RED, Color.ORANGE,
    // Color.YELLOW, Color.LIGHTGREEN, Color.LIGHTBLUE, Color.PURPLE };
    private static Color[] colors = { Color.RED, Color.web("#3F3"), Color.TURQUOISE,
            Color.web("FD3") };
    static int colorIndex = 0;

    protected double startX;
    protected double startY;

    // Map of listener path name and connection. Used for removing connections.
    private final Map<String, CubicCurve> connections;

    Wire(WireComponent wireComponent)
    {
        super();
        wireComponent.attachUserInterfaceComponent(this);
        this.wireComponent = wireComponent;
        // colorIndex = 0;

        connections = new HashMap<String, CubicCurve>();

        makeView();
    }

    protected void makeView()
    {
        System.out.println("wire = " + wireComponent);

        PortComponent sourcePort = wireComponent.getSourceComponent();
        List<PortComponent> listenerPorts = wireComponent.getListenerComponents();

        // Do sanity check first
        if (sourcePort == null || listenerPorts.isEmpty()) {
            Alert alert = new Alert(AlertType.WARNING);
            alert.setTitle("Dangling Wire");
            alert.setHeaderText("Dangling wire is found");
            StringBuilder sb = new StringBuilder("wireId: ").append(wireComponent.getWireId());
            if (sourcePort != null) {
                sb.append("\nsource: ").append(sourcePort.getPath());
            }
            for (PortComponent port : listenerPorts) {
                sb.append("\nlistener: ").append(port.getPath());
            }
            alert.setContentText(sb.toString());
            alert.showAndWait();
            // TODO: remove the wire
            return;
        }
        
        Pluggable source = sourcePort != null ? (Pluggable) sourcePort
                .getUserInterfaceComponent() : null;
        System.out.println("source = " + sourcePort.getPath() + ", position = "
                + source.getPlugPositionX() + ", " + source.getPlugPositionY());

        startX = source.getPlugPositionX();
        startY = source.getPlugPositionY();

        for (PortComponent port : listenerPorts) {
            Pluggable listener = (Pluggable) port.getUserInterfaceComponent();
            if (listener == null) {
                System.err.println("port=" + port.getPath()
                        + ": user interface component is not attached!");
                continue;
            }
            System.out.println("listener = " + port.getPath() + ", position = "
                    + listener.getPlugPositionX() + ", "
                    + listener.getPlugPositionY());
            double endX = listener.getPlugPositionX();
            double endY = listener.getPlugPositionY();
            addConnection(port.getPath(), startX, startY, endX, endY);
        }
    }

    void addConnection(String listenerPath, double startX, double startY, double endX,
            double endY)
    {
        CubicCurve curve = createCurve(startX, startY, endX, endY);
        connections.put(listenerPath, curve);
        getChildren().add(curve);
    }

    void removeConnection(String listenerPath)
    {
        CubicCurve curve = connections.get(listenerPath);
        if (curve != null) {
            getChildren().remove(curve);
            connections.remove(listenerPath);
        }
    }

    private CubicCurve createCurve(double startX, double startY, double endX, double endY)
    {
        CubicCurve curve = new CubicCurve();

        curve.setStartX(startX);
        curve.setStartY(startY);
        curve.setControlX1(startX + (endX - startX) / 3);
        curve.setControlY1(startY + Math.abs(endY - startY) / 4 + Math.abs(endX - startX) / 4);
        curve.setControlX2(endX - (endX - startX) / 3);
        curve.setControlY2(endY + Math.abs(endY - startY) / 4 + Math.abs(endX - startX) / 4);
        curve.setEndX(endX);
        curve.setEndY(endY);
        curve.setStroke(colors[colorIndex].deriveColor(1.0, 1.0, 1.0, 0.5));
        colorIndex = (colorIndex + 1) % colors.length;
        curve.setStrokeWidth(4);
        curve.setStrokeLineCap(StrokeLineCap.ROUND);
        curve.setFill(Color.TRANSPARENT);
        return curve;
    }

    static Wire getWireFromEvent(Event event)
    {
        return (Wire) ((Node) event.getSource()).getParent().getParent();
    }

    @Override
    public void notifyParameterAdded(final String attributeName, final String newValue)
    {
        if (!Platform.isFxApplicationThread()
                && attributeName.equals(WireComponent.LISTENERS)) {
            final SynthComponent component = wireComponent.resolve(newValue);
            if (component != null) {
                UserInterfaceComponent uic = component.getUserInterfaceComponent();
                if (uic instanceof Pluggable) {
                    Pluggable listener = (Pluggable) uic;
                    final double endX = listener.getPlugPositionX();
                    final double endY = listener.getPlugPositionY();

                    Platform.runLater(new Runnable() {
                        @Override
                        public void run()
                        {
                            addConnection(newValue, startX, startY, endX, endY);
                        }
                    });
                }

            }
        }
    }

    @Override
    public void notifyParameterRemoved(String attributeName, final Object value)
    {
        if (!Platform.isFxApplicationThread()
                && attributeName.equals(WireComponent.LISTENERS)) {
            Platform.runLater(new Runnable() {
                @Override
                public void run()
                {
                    removeConnection((String) value);
                }
            });
        }
    }

    @Override
    public void notifySubComponentAdded(SynthComponent subComponent)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public void notifySubComponentRemoved(SynthComponent subComponent)
    {
        // TODO Auto-generated method stub

    }
}
