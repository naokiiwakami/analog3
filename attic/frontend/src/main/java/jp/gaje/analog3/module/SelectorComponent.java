package jp.gaje.analog3.module;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class SelectorComponent extends ModulePart
{
    public final static String ATTR_CHOICES = "choices";
    public final static String ATTR_VALUE = "value";
    
    final protected List<String> choices;

    public SelectorComponent(String name, Set<String> choices, Integer initial,
            RackComponent rack)
    {
        super(TYPE_SELECTOR, name, rack);
        if (initial != null) {
            addAttribute("value", initial);
        }
        attributes.put("choices", choices);
        markReadOnly("choices");
        
        this.choices = new ArrayList<String>(choices.size());
        this.choices.addAll(choices);
    }

    public SelectorComponent(String componentName,
            Map<String, Object> sourceAttributes, RackComponent rack)
            throws ComponentBuildException
    {
        super(TYPE_SELECTOR, componentName, rack);

        boolean isMandatory = true;
        setAttributeStringSet(ATTR_CHOICES, sourceAttributes, isMandatory);
        markReadOnly(ATTR_CHOICES);
        setAttribute(ATTR_VALUE, sourceAttributes, isMandatory, Integer.class);

        Set<String> choices = getAttribute(ATTR_CHOICES);
        Integer index = getAttribute(ATTR_VALUE);
        if (index >= choices.size()) {
            throw new ComponentBuildException(getComponentType()
                    + ": value exceeds number of choices " + choices.size());
        }
        this.choices = new ArrayList<String>(choices.size());
        this.choices.addAll(choices);

        for (Map.Entry<String, Object> attribute : sourceAttributes.entrySet()) {
            attributes.putIfAbsent(attribute.getKey(), attribute.getValue());
        }
    }

    public Set<String> getChoices()
    {
        return getAttribute(ATTR_CHOICES);
    }

    public String getValue()
    {
        Integer index = getAttribute(ATTR_VALUE); 
        return choices.get(index);
    }

    public void setValue(String value) throws SynthComponentException
    {
        int index;
        for (index = 0; index < choices.size(); ++index) {
            if (choices.get(index).equals(value)) {
                break;
            }
        }
        if (index >= choices.size()) {
            throw new SynthComponentException(this, "setValue(): invalid value: " + value);
        }
        setAttributePersistent(ATTR_VALUE, Integer.valueOf(index));
    }
}
