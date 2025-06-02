package jp.gaje.analog3.module;

import java.util.List;
import java.util.Properties;

import jp.gaje.analog3.connector.RackConnector;

public class RackComponent extends SynthComponent
{
    protected final RackConnector rackConnector;

    public RackComponent(String name, String className, Properties properties) throws ComponentBuildException, SynthComponentException
    {
        super(TYPE_RACK, name);

        RackConnector rackConnector = RackConnector.getConnector(className, properties, name, this);

        List<SynthComponent> modules = rackConnector.recognizeModules();
        for (SynthComponent module : modules) {
            addSubComponent(module);
        }
        
        this.rackConnector = rackConnector;
    }

    /**
     * Simplified constructor for stub/test 
     * 
     * @param name
     * @throws ComponentBuildException
     * @throws SynthComponentException
     */
    public RackComponent(String name)
    {
        super(TYPE_RACK, name);

        this.rackConnector = null;
    }

    RackConnector getConnector()
    {
        return rackConnector;
    }
}
