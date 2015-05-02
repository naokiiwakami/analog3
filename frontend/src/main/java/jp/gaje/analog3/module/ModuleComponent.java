package jp.gaje.analog3.module;

public class ModuleComponent extends SynthComponent
{
    public final static String ATTR_MODULE_TYPE = "Metadata.moduleType"; 
    private final RackComponent rack;

    public ModuleComponent(String name, String moduleType, RackComponent rack)
    {
        super(TYPE_MODULE, name);
        attributes.put(ATTR_MODULE_TYPE, moduleType);
        addReadOnly(ATTR_MODULE_TYPE);

        this.rack = rack;
    }
    
    RackComponent getRack()
    {
        return rack;
    }
}
