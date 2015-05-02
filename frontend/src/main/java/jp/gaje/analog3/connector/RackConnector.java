package jp.gaje.analog3.connector;

import java.util.List;
import java.util.Properties;

import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.RackComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public abstract class RackConnector {

    protected Properties properties = null;
    protected String rackName = null;
    protected RackComponent rack;

    public void setProperties(Properties p, String rackName, RackComponent rack)
    {
        properties = p;
        this.rackName = rackName;
        this.rack = rack;
    }

    public static RackConnector getConnector(final String finderClassName,
            Properties properties, String rackName, RackComponent rack) throws ComponentBuildException
    {
        RackConnector rackConnector = null;
        try {
            Class<?> c = Class.forName(finderClassName);
            rackConnector = (RackConnector) c.newInstance();
            rackConnector.setProperties(properties, rackName, rack);
            rackConnector.initialize();
        } catch (ClassNotFoundException e) {
            throw new ComponentBuildException("Module finder class "
                    + finderClassName + " not found");
        } catch (InstantiationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return rackConnector;
    }

    abstract protected void initialize() throws ComponentBuildException;

    public abstract List<SynthComponent> recognizeModules() throws SynthComponentException, ComponentBuildException;
    
    public abstract void modifyAttribute(String[] backendPath, String attributeName, Object value) throws SynthComponentException;
    
    public abstract void removeAttribute(String[] backendPath, String attributeName) throws SynthComponentException;

    public abstract void addSubComponent(String[] backendPath, SynthComponent subComponent) throws SynthComponentException;

    public abstract void removeSubComponent(String[] backendPath, SynthComponent subComponent) throws SynthComponentException;
}
