package jp.gaje.analog3.module;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

public class WireComponent extends SynthComponent {

    public final static String WIREID = "wireId";
    public final static String SOURCE = "source";
    public final static String LISTENERS = "listeners";
    
    final protected String wireId;
    final protected Set<String> listeners;

    /**
     * Naming convention: WireComponent name is "w" + wireId.
     * ex) w2, w10
     * 
     * @param wireId
     */
    WireComponent(String wireId) {
        super(TYPE_WIRE, makeWireName(wireId));

        this.wireId = wireId;
        addParameter(WIREID, wireId);
        addReadOnly(WIREID);
        
        attributes.put(SOURCE, null);
        
        listeners = new LinkedHashSet<String>();
        attributes.put(LISTENERS, listeners);
        addReadOnly(LISTENERS);
    }

    /**
     * @return wireId
     */
    public String getWireId() {
        return wireId;
    }

    /**
     * Set the given component path to source  
     * 
     * @param path
     * @throws SynthComponentException
     * 
     * TODO: throw SourceConflictException instead of SynthComponentException
     */
    public void setSourcePath(String path) throws SynthComponentException {
        String source = getParameter(SOURCE);
        if (source != null) {
            throw new SynthComponentException(this,
                    "source is already set. src=" + source);
        }

        addParameter(SOURCE, path);
    }

    /**
     * Get source component path.
     * 
     * @return The path if it's already set. If it's not set yet, returns null.
     */
    public String getSourcePath() {
        return getParameter(SOURCE);
    }
    
    public PortComponent getSourceComponent() {
        PortComponent component = (PortComponent) resolve(getSourcePath());
        return component;
    }

    /**
     * Remove source from the 
     */
    public void removeSource() {
        addParameter(SOURCE, null);
    }

    /**
     * Add a listener
     * 
     * @param listenerPath
     */
    public void addListener(String listenerPath) {
        listeners.add(listenerPath);
        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifyParameterAdded(LISTENERS, listenerPath);
        }
    }

    /**
     * Remove a listenerPath from this wire. 
     * 
     * @param listenerPath Listener path to be removed.
     */
    public void removeListener(String listenerPath) {
        listeners.remove(listenerPath);
        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifyParameterRemoved(LISTENERS, listenerPath);
        }
    }

    /**
     * Get a set of listeners
     * 
     * @return listeners
     */
    public Set<String> getListenerPaths() {
        return listeners;
    }
    
    /**
     * Get list of listener components. 
     *  
     * @return listener components.
     */
    public List<PortComponent> getListenerComponents() {
        List<PortComponent> ports = new ArrayList<PortComponent>(listeners.size());
        for (String listenerPath : listeners) {
            PortComponent component = (PortComponent) resolve(listenerPath);
            if (component != null) {
                ports.add(component);
            }
        }
        return ports;
    }

    /**
     * Utility to make a WireComponent name from wire ID.
     * 
     * @param wireId
     * @return wire component name.
     */
    public static String makeWireName(String wireId) {
        return "w" + wireId;
    }
}
