package jp.gaje.analog3.softwarerack;

import javafx.application.Platform;
import javafx.scene.Group;
import jp.gaje.analog3.module.PerspectiveComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.UserInterfaceComponent;
import jp.gaje.analog3.module.WireComponent;

public class Wires implements UserInterfaceComponent
{
    final PerspectiveComponent perspective;
    final Group screen;
    
    Wires(PerspectiveComponent perspective, Group screen)
    {
        this.perspective = perspective;
        this.screen = screen;
    }

    @Override
    public void notifyParameterAdded(String attributeName, String newValue)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public void notifyParameterRemoved(final String attributeName, final Object value)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public void notifySubComponentAdded(final SynthComponent subComponent)
    {
        if (!Platform.isFxApplicationThread() && subComponent instanceof WireComponent) {
            Platform.runLater(new Runnable() {
                @Override
                public void run() {
                    WireComponent wireComponent = (WireComponent) subComponent;
                    screen.getChildren().add(new Wire(wireComponent));
                }
            });
            
        }
    }

    @Override
    public void notifySubComponentRemoved(final SynthComponent subComponent)
    {
        if (!Platform.isFxApplicationThread() && subComponent instanceof WireComponent) {
            final Wire wire = (Wire) subComponent.getUserInterfaceComponent();
            Platform.runLater(new Runnable() {
                @Override
                public void run()
                {
                    screen.getChildren().remove(wire);
                }
            });
        }
    }

}
