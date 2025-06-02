package jp.gaje.analog3.module;

import java.util.Map;
import java.util.SortedMap;
import java.util.TreeMap;

public class PerspectiveComponent extends SynthComponent
{

    protected final static Integer MAX_WIREID = 255;
    protected final SortedMap<Integer, Integer> usedWireIds;

    private SynthComponent wiresRoot = null;

    public PerspectiveComponent(String name)
    {
        super(TYPE_PERSPECTIVE, name);

        usedWireIds = new TreeMap<Integer, Integer>();
    }

    void addUsedWireId(Integer wireId)
    {
        Integer count = usedWireIds.get(wireId);
        if (count == null) {
            count = new Integer(0);
        }
        ++count;
        usedWireIds.put(wireId, count);
    }

    void addUsedWireId(String wireIdString) throws NumberFormatException
    {
        addUsedWireId(Integer.valueOf(wireIdString));
    }

    void removeUsedWireId(Integer wireId)
    {
        Integer count = usedWireIds.get(wireId);
        if (count == null) {
            return;
        }
        if (--count == 0) {
            usedWireIds.remove(wireId);
        } else {
            usedWireIds.put(wireId, count);
        }
    }

    void removeUsedWireId(String wireIdString) throws NumberFormatException
    {
        removeUsedWireId(Integer.valueOf(wireIdString));
    }

    /**
     * Find the smallest free wireId and return it.
     * 
     * @return Smallest free wireId. Returns null if no free wireId is
     *         available.
     */
    Integer getNextWireId()
    {
        Integer wireIdInt = 1;
        for (Integer current : usedWireIds.keySet()) {
            if (wireIdInt < current) {
                return wireIdInt;
            }
            wireIdInt = current + 1;
        }
        return wireIdInt <= MAX_WIREID ? wireIdInt : null;
    }

    public SynthComponent getWiresRoot()
    {
        if (wiresRoot == null) {
            wiresRoot = new SynthComponent(SynthComponent.TYPE_WIRES, "wires");
            addSubComponent(wiresRoot);
        }
        return wiresRoot;
    }

    /**
     * Resolve WireComponent in the perspective by wire name.
     * 
     * @param wireName
     * @return Found wire component. Returns null if the component is not found.
     */
    public WireComponent resolveWire(String wireName)
    {
        SynthComponent component = getWiresRoot().resolve(wireName);
        if (component != null && component instanceof WireComponent) {
            return (WireComponent) component;
        }
        return null;
    }

    /**
     * @return map of wire names and corresponding components.
     */
    public Map<String, SynthComponent> getWires()
    {
        return getWiresRoot().getSubComponents();
    }

    /**
     * Bind a wire to perspective. The method does not thing if the wire is
     * already bound.
     * 
     * @param wireId
     *            Wire ID to bind.
     * @return Bound wire component.
     */
    WireComponent bindWire(Integer wireId)
    {
        String wireName = WireComponent.makeWireName(wireId);
        WireComponent wire = resolveWire(wireName);
        if (wire == null) {
            wire = new WireComponent(wireId);
            getWiresRoot().addSubComponent(wire);
            addUsedWireId(wireId);
        }
        return wire;
    }

    void unbindWire(WireComponent wire) throws SynthComponentException
    {
        Integer wireId = wire.getWireId();
        removeUsedWireId(wireId);
        getWiresRoot().removeSubComponentPersistent(wire.getName());
    }

    /**
     * Connect a source and a listener components by a wire. Source must be a
     * PortComponent. Listener can be either PortComponent or KnobComponent. If
     * the listener is a KnobComponent, the method creates a PortComponent
     * object and add it to the KnobComponent sub-components.
     * 
     * @param sourcePath
     *            Path for the source port component.
     * @param listenerPath
     *            Path for the listener port or knob component.
     * @return The wireId
     * @throws SynthComponentException
     */
    public Integer connect(String sourcePath, String listenerPath) throws SynthComponentException
    {

        PortComponent sourcePort = resolveSourcePort(sourcePath, "Connect");

        // Determine wireId
        Integer wireId = sourcePort.getWireId();
        if (wireId == null) {
            wireId = getNextWireId();
        }
        String wireName = WireComponent.makeWireName(wireId);

        // Find the listener port
        if (listenerPath == null) {
            throw new SynthComponentException(this, "Connect failure: listenerPath is null");
        }
        SynthComponent listener = resolve(listenerPath);
        PortComponent listenerPort = null;
        String listenerSignal;
        if (listener == null) {
            throw new SynthComponentException(this, "Connect failure: " + listenerPath + ": Listener not found.");
        } else if (listener instanceof PortComponent) {
            listenerPort = (PortComponent) listener;
            listenerSignal = listenerPort.getSignal();
        } else if (listener instanceof KnobComponent) {
            SynthComponent component = listener.resolve(wireName);
            if (component != null) {
                listenerPort = (PortComponent) component;
                listenerSignal = listenerPort.getSignal();
            } else {
                listenerSignal = PortComponent.SIGNAL_VALUE;
            }
        } else {
            throw new SynthComponentException(this, "Connect Failure: Listener port is not pluggable.");
        }

        // compare signals
        if (!sourcePort.getSignal().equals(listenerSignal)) {
            throw new SynthComponentException(this, "Connect failure: Signal mismatch. src=" + sourcePort.getSignal()
                    + " lsn=" + listenerSignal);
        }

        // Listener sanity check or create one.
        if (listenerPort != null) {
            if (listenerPort.getDirection() != PortComponent.INPUT) {
                throw new SynthComponentException(this, "Connect failure: " + listenerPath
                        + ": Listener must be an input port.");
            }
            Integer listenerWireId = listenerPort.getWireId();
            if (listenerWireId != null) {
                if (listenerWireId.equals(wireId)) {
                    // already connected. finish silently.
                    return wireId;
                } else {
                    String listenerWireName = WireComponent.makeWireName(listenerWireId);
                    if (resolveWire(listenerWireName) != null) {
                        throw new SynthComponentException(this, "Connect failue: The listener is occupied by wireId "
                                + listenerWireId);
                    }
                    // else it's a dangling wire. just continue
                }
            }
        } else {
            listenerPort = new PortComponent(wireName, PortComponent.INPUT, listenerSignal,
                    ((ModulePart) listener).getRack());
            listener.addSubComponentPersistent(listenerPort);
        }

        // Set wire IDs to the ports.
        sourcePort.setWireId(wireId);
        listenerPort.setWireId(wireId);

        wireBetweenPorts(sourcePort.getPath(), listenerPort.getPath(), wireId);

        return wireId;
    }

    /**
     * Disconnect wired source and listener ports.
     * 
     * @param sourcePath
     *            Path for the source port component.
     * @param listenerPath
     *            Path for the listener port component.
     * @return The wireId that was used for connection.
     * @throws SynthComponentException
     */
    public Integer disconnect(String sourcePath, String listenerPath) throws SynthComponentException
    {
        // Find source port
        PortComponent sourcePort = resolveSourcePort(sourcePath, "Disconnect");

        // Resolve wire ID and wire name
        Integer wireId = sourcePort.getWireId();
        if (wireId == null) {
            throw new SynthComponentException(this, "Disconnect failure: Source is not wired.");
        }
        String wireName = WireComponent.makeWireName(wireId);
        
        // Find listener port
        SynthComponent listener = resolve(listenerPath);
        PortComponent listenerPort;
        if (listener == null) {
            throw new SynthComponentException(this, "Disonncect failure: " + listenerPath + ": Listener not found.");
        } else if (listener instanceof PortComponent) {
            listenerPort = (PortComponent) listener;
        } else if (listener instanceof KnobComponent) {
            SynthComponent port = listener.resolve(wireName);
            if (port instanceof PortComponent) {
                listenerPort = (PortComponent) port;
                listenerPath = listenerPort.getPath();
            } else {
                throw new SynthComponentException(this, "Disconnect failure: specified knob does not have the port for the wireid");
            }
        } else {
            throw new SynthComponentException(this, "Disconnect Failure: Listener port is not pluggable.");
        }

        // Check connectivity
        if (!wireId.equals(listenerPort.getWireId())) {
            throw new SynthComponentException(this, "Disconnect failure: Wire ID mismatch.  src=" + wireId + " lsn="
                    + listenerPort.getWireId());
        }

        // Remove wireId from the listener port
        listenerPort.removeWireId();

        // Remove listener port from wire, and wire itself if necessary
        WireComponent wire = resolveWire(wireName);
        if (wire != null) {
            wire.removeListener(listenerPath);
            if (wire.getListenerPaths().isEmpty()) {
                unbindWire(wire);
                sourcePort.removeWireId();
            }
        }
        
        // Remove the listener port if it belongs to a knob.
        SynthComponent listenerParent = listenerPort.getParent();
        if (listenerParent instanceof KnobComponent) {
            listenerParent.removeSubComponentPersistent(wireName);
        }

        return wireId;
    }

    private PortComponent resolveSourcePort(String sourcePath, String methodName) throws SynthComponentException
    {

        if (sourcePath == null) {
            throw new SynthComponentException(this, methodName + " failure: sourcePath is null");
        }

        // Find the source port and do sanity checks.
        SynthComponent source = resolve(sourcePath);
        if (source == null) {
            throw new SynthComponentException(this, methodName + " failure: " + sourcePath + " not found.");
        } else if (!(source instanceof PortComponent)) {
            throw new SynthComponentException(this, methodName + " failure: Source component " + sourcePath
                    + " is not a PORT object.");
        }
        PortComponent sourcePort = (PortComponent) source;
        if (sourcePort.getDirection() != PortComponent.OUTPUT) {
            throw new SynthComponentException(this, methodName + " failure: " + sourcePath
                    + ": Source must be an output port.");
        }

        return sourcePort;
    }

    /**
     * Connect a source and a listener ports by a wire.
     * 
     * @param sourcePath
     *            The source path.
     * @param listenerPath
     *            The listener path.
     * @param wireId
     *            The wire ID.
     */
    void wireBetweenPorts(String sourcePath, String listenerPath, Integer wireId) throws SynthComponentException
    {

        if (wireId == null) {
            throw new SynthComponentException(this, "Ports cannot be connected without wireId.");
        }

        if (sourcePath == null || sourcePath.isEmpty()) {
            throw new SynthComponentException(this, "Source path must not be empty.");
        }

        if (listenerPath == null || listenerPath.isEmpty()) {
            throw new SynthComponentException(this, "Listener path must not be empty.");
        }

        // Find or make the wire component.
        String wireName = WireComponent.makeWireName(wireId);
        WireComponent wire = resolveWire(wireName);
        boolean isCreated = false;
        if (wire == null) {
            wire = new WireComponent(wireId);
            addUsedWireId(wireId);
            isCreated = true;
        }

        // Add source path to the wire
        String currentSourcePath = wire.getSourcePath();
        if (currentSourcePath != null) {
            if (!currentSourcePath.equals(sourcePath)) {
                throw new SynthComponentException(wire, "Source mismatch. current=" + currentSourcePath
                        + ", new=" + sourcePath);
            }
        } else {
            wire.setSourcePath(sourcePath);
        }

        // Add listener path to the wire component
        wire.addListener(listenerPath);

        if (isCreated) {
            getWiresRoot().addSubComponentPersistent(wire);
        }
    }
}
