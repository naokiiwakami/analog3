package jp.gaje.analog3.module;

import java.util.Map;

public class ModuleComponent extends SynthComponent
{
    public final static String ATTR_MODULE_TYPE = "Metadata.moduleType"; 
    private final RackComponent rack;

    public ModuleComponent(String name, String moduleType, RackComponent rack)
    {
        super(TYPE_MODULE, name);
        attributes.put(ATTR_MODULE_TYPE, moduleType);
        markReadOnly(ATTR_MODULE_TYPE);

        this.rack = rack;
    }
    
    public ModuleComponent(String name, Map<String, Object> sourceAttributes,
            RackComponent rack) throws ComponentBuildException
    {
        super(TYPE_MODULE, name);
        
        final boolean isMandatory = true;
        setAttribute(ATTR_MODULE_TYPE, sourceAttributes, isMandatory, String.class);
        markReadOnly(ATTR_MODULE_TYPE);
        
        for (Map.Entry<String, Object> attribute : sourceAttributes.entrySet()) {
            attributes.putIfAbsent(attribute.getKey(), attribute.getValue());
        }
        
        this.rack = rack;
    }
    
    RackComponent getRack()
    {
        return rack;
    }
}
