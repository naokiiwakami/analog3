package jp.gaje.analog3.connector;

import java.util.ArrayList;
import java.util.List;

import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.ModuleComponent;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class StubRackConnector extends RackConnector {

    @Override
    protected void initialize() throws ComponentBuildException
    {
        // TODO Auto-generated method stub

    }

    @Override
	public
    List<SynthComponent> recognizeModules() throws SynthComponentException
    {
        List<SynthComponent> components = new ArrayList<SynthComponent>();
        
        // Make a dummy envelope generator
        ModuleComponent eg = new ModuleComponent("eg", "ADSR", rack);
        eg.addParameter("attackTime", "10");
        eg.addParameter("decayTime", "480");
        eg.addParameter("sustainLevel", "230");
        eg.addParameter("releaseTime", "120");
        eg.addParameter("curve", "exponential");
        eg.addParameter("velocityDepth", "1023");
        eg.addParameter("timeScaleDepth", "100");
        
        eg.addSubComponent(new PortComponent("gate", PortComponent.INPUT, PortComponent.SIGNAL_NOTE, rack));
        eg.addSubComponent(new PortComponent("envelope", PortComponent.OUTPUT, PortComponent.SIGNAL_VALUE, rack));
        components.add(eg);
        
        return components;
    }

    @Override
	public
    void addSubComponent(String[] backendPath, SynthComponent subComponent)
            throws SynthComponentException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
	public
    void removeAttribute(String[] path, String attributeName) throws SynthComponentException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
	public
    void removeSubComponent(String[] backendPath, SynthComponent subComponent)
            throws SynthComponentException
    {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void modifyAttribute(String[] backendPath, String attributeName, Object value)
            throws SynthComponentException
    {
        // TODO Auto-generated method stub
        
    }

}
