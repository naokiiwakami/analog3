package jp.gaje.analog3.module;

import java.util.Stack;

import jp.gaje.analog3.connector.RackConnector;

public class ModulePart extends SynthComponent
{
    protected final RackComponent rack;
    protected String[] backendPath;

    public ModulePart(String componentClass, String name, RackComponent rack)
    {
        super(componentClass, name);

        this.rack = rack;
        backendPath = null;
    }

    public RackComponent getRack()
    {
        return rack;
    }

    public RackConnector getRackConnector()
    {
        return rack.getConnector();
    }

    public String[] getBackendPath() throws SynthComponentException
    {
        if (backendPath == null) {
            Stack<String> stack = new Stack<String>();
            SynthComponent current = this;
            while (current != null && current != rack) {
                stack.push(current.getFullName());
                current = current.getParent();
            }
            if (current == null) {
                throw new SynthComponentException(this, "Rack not found in ancestors");
            }

            backendPath = new String[stack.size()];
            int i = 0;
            while (!stack.isEmpty()) {
                backendPath[i++] = stack.pop();
            }
        }

        return backendPath;
    }

    // backend executors for persistent methods.

    @Override
    <T> void backendSetAttribute(String attributeName, T value) throws SynthComponentException
    {
        RackConnector connector = getRackConnector();
        if (connector != null) {
            connector.setAttribute(getBackendPath(), attributeName, value);
        }
    }
    
    @Override
    void backendUnsetAttribute(String name)  throws SynthComponentException
    {
        RackConnector connector = getRackConnector();
        if (connector != null) {
            connector.unsetAttribute(getBackendPath(), name);
        }
    }
    
    @Override
    void backendAddSubComponent(SynthComponent subComponent) throws SynthComponentException
    {
        RackConnector connector = getRackConnector();
        if (connector != null) {
            connector.addSubComponent(getBackendPath(), subComponent);
        }
    }
    
    @Override
    void backendRemoveSubComponent(SynthComponent subComponent) throws SynthComponentException {
        RackConnector connector = getRackConnector();
        if (connector != null) {
            connector.removeSubComponent(getBackendPath(), subComponent);
        }
    }
}
