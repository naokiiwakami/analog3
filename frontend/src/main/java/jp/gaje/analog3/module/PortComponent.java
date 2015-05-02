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
        addParameter(ATTR_DIRECTION, direction.name());
        addReadOnly(ATTR_DIRECTION);

        this.signal = signal;
        addParameter(ATTR_SIGNAL, signal);
        addReadOnly(ATTR_SIGNAL);
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
        addParameter(ATTR_DIRECTION, directionStr);
        addReadOnly(ATTR_DIRECTION);

        boolean isMandatory = true;
        signal = setAttribute(ATTR_SIGNAL, sourceAttributes, isMandatory, String.class);
        addReadOnly(ATTR_SIGNAL);
        
        isMandatory = false;
        setAttribute(ATTR_WIREID, sourceAttributes, isMandatory, String.class);

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
    public void setWireId(String wireId) throws SynthComponentException
    {
        if (wireId == null || wireId.isEmpty()) {
            throw new SynthComponentException(this,
                    "setWireId error. Given wireId is null or empty.");
        }
        if (wireId.equals(getWireId())) {
            return;
        }
        modifyAttribute("wireId", wireId);
    }

    public String getWireId()
    {
        return getParameter("wireId");
    }

    public void removeWireId() throws SynthComponentException
    {
        removeParameterPersistent("wireId");
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
