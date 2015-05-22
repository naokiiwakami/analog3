package jp.gaje.analog3.module;

import java.util.Map;

/**
 * This class implements SynthComponent that projects input/output port
 * component.
 */
public class PortComponent extends ModulePart
{

    public enum Direction
    {
        INPUT, OUTPUT
    }

    public final static Direction INPUT = Direction.INPUT;
    public final static Direction OUTPUT = Direction.OUTPUT;

    public final static String ATTR_DIRECTION = "direction";
    public final static String ATTR_SIGNAL = "signal";
    public final static String ATTR_WIREID = "wireId";

    public final static String SIGNAL_NOTE = "note";
    public final static String SIGNAL_VALUE = "value";

    final private Direction direction;
    final private String signal;

    /**
     * @param name
     * @param direction
     */
    public PortComponent(String componentName, Direction direction,
            String signal, RackComponent rack)
    {
        super(TYPE_PORT, componentName, rack);

        this.direction = direction;
        addAttribute(ATTR_DIRECTION, direction.name());
        markReadOnly(ATTR_DIRECTION);

        this.signal = signal;
        addAttribute(ATTR_SIGNAL, signal);
        markReadOnly(ATTR_SIGNAL);
    }

    public PortComponent(String componentName,
            Map<String, Object> sourceAttributes,
            RackComponent rack) throws ComponentBuildException
    {
        super(TYPE_PORT, componentName, rack);

        String directionStr;
        Object value = sourceAttributes.get(ATTR_DIRECTION);
        if (value == null || ! (value instanceof String)) {
            throw new ComponentBuildException(getComponentType()
                    + ": mandatory attribute " + ATTR_DIRECTION + " is missing or invalid");
        }
        directionStr = (String) value;
        direction = Direction.valueOf(directionStr);
        addAttribute(ATTR_DIRECTION, directionStr);
        markReadOnly(ATTR_DIRECTION);

        boolean isMandatory = true;
        signal = setAttribute(ATTR_SIGNAL, sourceAttributes, isMandatory, String.class);
        markReadOnly(ATTR_SIGNAL);
        
        isMandatory = false;
        setAttribute(ATTR_WIREID, sourceAttributes, isMandatory, Integer.class);

        for (Map.Entry<String, Object> attribute : sourceAttributes.entrySet()) {
            attributes.putIfAbsent(attribute.getKey(), attribute.getValue());
        }

    }

    /**
     * Set wire ID of this port.
     * 
     * @param wireId
     *            Wire ID to set.
     * @throws SynthComponentException
     */
    public void setWireId(Integer wireId) throws SynthComponentException
    {
        if (wireId == null) {
            throw new SynthComponentException(this,
                    "setWireId error. Given wireId is null or empty.");
        }
        if (wireId.equals(getWireId())) {
            return;
        }
        setAttributePersistent("wireId", wireId);
    }

    public Integer getWireId()
    {
        return getAttributeInteger(ATTR_WIREID);
    }

    public void removeWireId() throws SynthComponentException
    {
        unsetAttributePersistent(ATTR_WIREID);
    }

    public Direction getDirection()
    {
        return direction;
    }

    public String getSignal()
    {
        return signal;
    }

}
