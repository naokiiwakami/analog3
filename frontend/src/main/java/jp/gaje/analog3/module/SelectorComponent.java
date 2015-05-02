package jp.gaje.analog3.module;

import java.util.Map;
import java.util.Set;

public class SelectorComponent extends ModulePart
{
    public final static String ATTR_CHOICES = "choices";
    public final static String ATTR_VALUE = "value";

    public SelectorComponent(String name, Set<String> choices, String initial,
            RackComponent rack)
    {
        super(TYPE_SELECTOR, name, rack);
        if (initial != null) {
            addAttribute("value", initial);
        }
        attributes.put("choices", choices);

        markReadOnly("choices");
    }

    public SelectorComponent(String componentName,
            Map<String, Object> sourceAttributes, RackComponent rack)
            throws ComponentBuildException
    {
        super(TYPE_SELECTOR, componentName, rack);

        boolean isMandatory = true;
        setAttributeStringSet(ATTR_CHOICES, sourceAttributes, isMandatory);
        markReadOnly(ATTR_CHOICES);
        setAttribute(ATTR_VALUE, sourceAttributes, isMandatory, String.class);

        for (Map.Entry<String, Object> attribute : sourceAttributes.entrySet()) {
            attributes.putIfAbsent(attribute.getKey(), attribute.getValue());
        }
    }

    @SuppressWarnings("unchecked")
    public Set<String> getChoices()
    {
        return (Set<String>) attributes.get("choices");
    }

    public String getValue()
    {
        return getAttribute("value");
    }

    public void setValue(String value)
    {
        try {
            setAttributePersistent(ATTR_VALUE, value);
        } catch (SynthComponentException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

}
