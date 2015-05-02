package jp.gaje.analog3.module;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map.Entry;
import java.util.Properties;

/**
 * This class makes hardware perspective. A hardware perspective consists of -
 * Rack - SynthModule - Knob - Selector - Port - ... - Wire
 * 
 * Wires are built built from Port components initially.
 * 
 * @author Naoki Iwakami
 *
 */
public class PerspectiveBuilder
{

    final String perspectiveName;
    final protected Properties properties;
    final protected List<String> rackNames;
    SynthComponent root;

    public PerspectiveBuilder(final String name) throws ComponentBuildException
    {

        perspectiveName = name;
        properties = new Properties();
        // Read hardware description file
        final String filename = "conf/" + name + ".properties";
        try {
            properties.load(new FileInputStream(filename));
        } catch (IOException e) {
            throw new ComponentBuildException("Unable to read " + filename);
        }

        // Find rack names.
        String racksString = properties.getProperty("racks");
        if (racksString == null) {
            // TODO: throw
        }
        String[] rackNames = racksString.split(",");
        this.rackNames = new ArrayList<String>(Arrays.asList(rackNames));
        root = null;
    }

    public void setRoot(SynthComponent root)
    {
        this.root = root;
    }

    /**
     * Build the perspective. This method first recognizes hardware modules that
     * makes rack(s). Then finds wires.
     * 
     * @return (hardware) perspective
     * @throws SynthComponentException
     * @throws ComponentBuildException
     */
    public PerspectiveComponent build() throws SynthComponentException, ComponentBuildException
    {

        if (root == null) {
            root = new SynthComponent(SynthComponent.TYPE_ROOT, "");
        }

        PerspectiveComponent perspective = new PerspectiveComponent(perspectiveName);
        root.addSubComponent(perspective);

        // build racks
        for (String rackName : rackNames) {
            RackComponent rack = buildRack(rackName);
            perspective.addSubComponent(rack);
        }

        // find wires
        makeWires(perspective, perspective);

        return perspective;
    }

    /**
     * Build a rack from rack name.
     * 
     * @param rackName
     *            The rack name.
     * @return Built rack component.
     * @throws ComponentBuildException
     * @throws SynthComponentException
     */
    protected RackComponent buildRack(String rackName) throws ComponentBuildException,
            SynthComponentException
    {
        final String connectorClassPropertyName = rackName + ".connector";
        String className = properties.getProperty(connectorClassPropertyName);
        if (className == null) {
            throw new ComponentBuildException("Mandatory property " + connectorClassPropertyName + " is not set");
        }
        RackComponent rack = new RackComponent(rackName, className, properties);

        return rack;
    }

    /**
     * This method iterates modules in racks and finds input/output ports. Picks
     * up already-assigned wire IDs and builds wire components.
     *
     * @param perspective
     *            The hardware perspective.
     * @throws SynthComponentException
     */
    protected void makeWires(PerspectiveComponent perspective, SynthComponent current)
            throws SynthComponentException
    {

        // iterate racks
        for (Entry<String, SynthComponent> entry : current.getSubComponents().entrySet()) {

            SynthComponent component = entry.getValue();

            if (component instanceof PortComponent) {
                PortComponent port = (PortComponent) component;

                // Put to a wire if wireID is set.
                String wireId = port.getWireId();
                if (wireId != null) {
                    PortComponent.Direction direction = port.getDirection();
                    String path = port.getPath();

                    WireComponent wire = perspective.bindWire(wireId);

                    // put the path of the port as source or listener
                    if (direction == PortComponent.OUTPUT) {
                        // TODO: catch wire source conflict exception
                        // here, report error to UI and continue
                        wire.setSourcePath(path);
                    } else {
                        wire.addListener(path);
                    }
                }
            } else {
                makeWires(perspective, component);
            }
        }
    }
}
