package jp.gaje.analog3.module;

import java.util.Map;


public class KnobComponent extends ModulePart {
    
    public final static String ATTR_SCALE = "scale";
    public final static String ATTR_VALUE = "value";
    
    private final double scale;

    public KnobComponent(String name, String scale, String initial, RackComponent rack) {
        super(TYPE_KNOB, name, rack);
        
        this.scale = Double.parseDouble(scale);
        addParameter(ATTR_SCALE, scale);
        addReadOnly(ATTR_SCALE);
        
        addParameter(ATTR_VALUE, initial);
    }

    public KnobComponent(String name, Map<String, Object> sourceAttributes,
            RackComponent rack) throws ComponentBuildException
    {
        super(TYPE_KNOB, name, rack);
        
        final boolean isMandatory = true;
        scale = setAttribute(ATTR_SCALE, sourceAttributes, isMandatory, Integer.class);
        addReadOnly(ATTR_SCALE);
        setAttribute(ATTR_VALUE, sourceAttributes, isMandatory, Integer.class);
        
        for (Map.Entry<String, Object> attribute : sourceAttributes.entrySet()) {
            attributes.putIfAbsent(attribute.getKey(), attribute.getValue());
        }
    }
    
    public double getValue() {
        return (Integer) attributes.get(ATTR_VALUE);
    }
    
    public void setValue(double value) {
        Integer valueToSet = (int) (value + 0.5);
        try {
            modifyAttribute(ATTR_VALUE, valueToSet);
            // modifyParameter("value", Integer.toString(valueToSet), false);
        } catch (SynthComponentException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
    
    public double getScale() {
        return scale;
    }

}
