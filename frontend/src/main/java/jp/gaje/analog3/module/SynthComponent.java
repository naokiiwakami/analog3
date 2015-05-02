package jp.gaje.analog3.module;

import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class SynthComponent {

    public final static String TYPE_ROOT = "Root";
    public final static String TYPE_PERSPECTIVE = "Perspective";
    public final static String TYPE_RACK = "Rack";
    public final static String TYPE_MODULE = "Module";
    public final static String TYPE_KNOB = "Knob";
    public final static String TYPE_SELECTOR = "Selector";
    public final static String TYPE_PORT = "Port";
    public final static String TYPE_WIRE = "Wire";
    public final static String TYPE_WIRES = "Wires";

    private final String componentClass;
    protected String name;
    private final String fullName;
    protected SynthComponent parent;
    protected final Map<String, Object> attributes;
    
    protected UserInterfaceComponent userInterfaceComponent;
    
    /**
     * Set of attribute names that are read only.
     */
    private final Set<String>  readOnlyAttributes;
    
    /**
     * Map of component name and sub-component
     */
    protected final Map<String, SynthComponent> subComponents;

    public SynthComponent(String componentClass, String name) {
        this.componentClass = componentClass;
        this.name = name;
        this.fullName = componentClass + "." + name;
        parent = null;
        attributes = new LinkedHashMap<String, Object>();
        subComponents = new LinkedHashMap<String, SynthComponent>();
        readOnlyAttributes = new HashSet<String>();
        
        userInterfaceComponent = null;
    }

    public SynthComponent(SynthComponent source) {
        componentClass = source.componentClass;
        name = source.name;
        fullName = componentClass + "." + name;
        parent = source.parent;
        attributes = new LinkedHashMap<String, Object>(source.attributes);
        subComponents = new LinkedHashMap<String, SynthComponent>(source.subComponents);
        
        readOnlyAttributes = new HashSet<String>();
        
        userInterfaceComponent = null;
    }

    public final String getComponentType() {
        return componentClass;
    }

    public final String getName() {
        return name;
    }
    
    public final String getFullName()
    {
        return fullName;
    }
    
    public <T> T get(Class<T> cls) {
        return cls.cast(this);
    }
    
    @SuppressWarnings("unchecked")
    protected <T> T setAttribute(String attributeName,
            Map<String, Object> sourceAttributes, boolean isMandatory, Class<T> tClass)
            throws ComponentBuildException
    {
        if (!sourceAttributes.containsKey(attributeName)) {
            if (isMandatory) {
                throw new ComponentBuildException(getComponentType()
                        + ": mandatory attribute " + attributeName + " is missing");
            } else {
                return null;
            }
        }
        T result;
        Object value = sourceAttributes.get(attributeName);
        if (tClass.isAssignableFrom(value.getClass())) {
            result = (T) value;
        } else {
            throw new ComponentBuildException(getComponentType() + ": "
                    + attributeName + ": invalid value type: " + value.getClass().getSimpleName()
                    + ", " + tClass.getSimpleName() + " is expected.");
        }
        attributes.put(attributeName,  result);
        
        return result;
    }

    protected Set<String> setAttributeStringSet(String attributeName,
            Map<String, Object> sourceAttributes, boolean isMandatory)
            throws ComponentBuildException
    {
        Object value = sourceAttributes.get(attributeName);
        if (value == null) {
            if (isMandatory) {
                throw new ComponentBuildException(getComponentType()
                        + ": mandatory attribute " + attributeName + " is missing");
            } else {
                return null;
            }
        }
        Set<String> result = new LinkedHashSet<String>();
        if (value instanceof List<?>) {
            List<?> valueList = (List<?>) value;
            for (Object item : valueList) {
                if (! (item instanceof String)) {
                    throw new ComponentBuildException(getComponentType()
                            + ": attribute " + attributeName + " must be a string list");
                }
                result.add((String) item);
            }
        } else {
            throw new ComponentBuildException(getComponentType() + ": "
                    + attributeName + ": must be a string list");
        }
        
        attributes.put(attributeName, result);
        
        return result;
    }

    public void rename(String newName) throws SynthComponentException {
        if (newName == null || newName.isEmpty()) {
            throw new SynthComponentException(this, newName + "empty component name");
        }
        executeRename(newName);
        SynthComponent parentLocal = (parent);
        parentLocal.subComponents.remove(name);
        parentLocal.subComponents.put(newName, this);
        name = newName;
    }

    protected void executeRename(String newName) throws SynthComponentException {
        // not supported in default
        throw new OperationNotSupportedException(this, "rename");
    }


    public SynthComponent getRoot() {
        SynthComponent current = this;
        while (current.getParent() != null) {
            current = current.getParent();
        }
        return current;
    }
    
    public SynthComponent getParent() {
        return parent;
    }

    public void setParent(SynthComponent parent) {
        this.parent = parent;
    }

    /**
     * Add an attribute to this component. Note this method does not go to
     * back-end, so should not be used for the purpose other than initial object
     * build.
     * 
     * @param attributeName Parameter Name.
     * @param value The value.
     */
    public void addAttribute(String attributeName, String value) {
        attributes.put(attributeName, value);
    }
    
    public void addAttributes(Map<String, Object> newAttributes)
    {
        attributes.putAll(newAttributes);
    }

    public String getAttribute(String name) {
        return (String) attributes.get(name);
    }
    
    void unsetAttribute(String name) {
        attributes.remove(name);
    }
    
    void unsetAttributePersistent(String attributeName) throws SynthComponentException {
        
        String value = getAttribute(attributeName);
        
        try {
            unsetAttribute(attributeName);
            
            backendUnsetAttribute(attributeName);
            
        } catch (SynthComponentException ex) {
            addAttribute(attributeName, value);
            throw ex;
        }
        
        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifyParameterRemoved(attributeName, value);
        }
    }
    
    public <T> void setAttributePersistent(String attributeName, T value) throws SynthComponentException
    {
        // check read only
        if (readOnlyAttributes.contains(attributeName)) {
            throw new OperationNotSupportedException(this, "read only: " + attributeName);
        }
        
        T oldValue = (T) attributes.get(attributeName);
        if (oldValue == null) {
            attributes.put(attributeName, value);
        } else {
            attributes.replace(attributeName, value);
        }
        
        try {
            backendSetAttribute(attributeName, value);
        } catch (SynthComponentException ex) {
            // revert the cache change
            if (oldValue == null) {
                attributes.remove(attributeName);
            } else {
                attributes.replace(attributeName, oldValue);
            }
            throw ex;
        }
        
        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifyParameterAdded(attributeName, value.toString());
        }
    }

    public Map<String, Object> getAttributes() {
        return attributes;
    }

    public Map<String, SynthComponent> getSubComponents() {
        return subComponents;
    }

    
    /**
     * Add a sub-component.  Notifications to backend and UI does not happen.
     * 
     * @param subComponent Sub-component to add.
     */
    public void addSubComponent(final SynthComponent subComponent) {
        subComponent.setParent(this);
        subComponents.put(subComponent.getName(), subComponent);
    }
    
    /**
     * Remove a sub-component.  Notifications to backend and UI does not happen.
     * 
     * @param name Name of the sub-component.
     * @return true if the sub-component was found and removal happened.
     */
    public boolean removeSubComponent(String name) {
        SynthComponent subComponent = subComponents.get(name);
        if (subComponent != null) {
            subComponents.remove(name);
            return true;
        }
        return false;
    }
    
    /**
     * Add a sub-component with notification for backend and UI.
     * 
     * @param subComponent
     * @throws SynthComponentException
     */
    public void addSubComponentPersistent(SynthComponent subComponent) throws SynthComponentException {
        
        SynthComponent oldParent = subComponent.getParent();
        try {
            // update cache
            addSubComponent(subComponent);
            
            // per-implementation modification
            backendAddSubComponent(subComponent);
            
        } catch (SynthComponentException ex) {
            // revert the change
            removeSubComponent(subComponent.getName());
            subComponent.setParent(oldParent);
            throw ex;
        }

        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifySubComponentAdded(subComponent);
        }
    }
    
    /**
     * Remove a sub-component with notifications for backend and UI.
     * 
     * @param name Name of the sub-component.
     * @return true if the sub-component was found and removal happened.
     */
    public boolean removeSubComponentPersistent(String name) throws SynthComponentException {
        SynthComponent subComponent = resolve(name);
        if (subComponent == null) {
            return false;
        }
        
        boolean result = false;
        
        try {
            // update cache
            result = removeSubComponent(subComponent.getName());
            
            // per-implementation modification
            backendRemoveSubComponent(subComponent);
            
        } catch (SynthComponentException ex) {
            // revert the cache
            // TODO: preserve the old sub-components order.
            addSubComponent(subComponent);
        }
        
        if (userInterfaceComponent != null) {
            userInterfaceComponent.notifySubComponentRemoved(subComponent);
        }
        
        return result;
    }
    
    public SynthComponent getSubComponent(String name) {
        return subComponents.get(name);
    }
    
    /**
     * Mark an attribute name read only.
     * @param attributeName
     */
    protected void markReadOnly(String attributeName) {
        readOnlyAttributes.add(attributeName);
    }

    /**
     * @return Full path name of this component.
     */
    public String getPath() {
        Stack<String> stack = new Stack<String>();
        SynthComponent current = this;
        while (current != null) {
            stack.push(current.getName());
            current = current.getParent();
        }

        StringBuilder sb = new StringBuilder();
        String sep = "";
        while (!stack.isEmpty()) {
            sb.append(sep);
            sb.append(stack.pop());
            sep = "/";
        }
        
        return sb.toString();
    }

    /**
     * Resolve component path.
     * 
     * @param targetPath
     *            Component path name.
     * @return Found component. The method returns null if the names couldn't be
     *         resolved.
     */
    public SynthComponent resolve(String targetPath) {
        
        if (targetPath == null || targetPath.isEmpty()) {
            return null;
        }
        
        String[] targetNames = targetPath.split("/");
        List<String> namesList = Arrays.asList(targetNames);
        if (targetPath.startsWith("/")) {
            // this is full path
            SynthComponent root = getRoot();
            return (targetPath.length() == 1) ? root : root.resolve(namesList.subList(1, namesList.size()));  
        }
        
        return resolve(namesList);
    }

    /**
     * Resolve component names.
     * 
     * @param componentNames
     *            Component names that indicates relative position in the tree
     *            from this component.
     * @return Found component. The method returns null if the names couldn't be
     *         resolved.
     */
    public SynthComponent resolve(List<String> componentNames) {
        
        String currentName = componentNames.get(0);
        SynthComponent child = this.getSubComponent(currentName);
        if (child == null) {
            return null;
        }
        
        if (componentNames.size() == 1) {
            return child; 
        } else {
            return child.resolve(componentNames.subList(1, componentNames.size()));
        }
    }
    
    /**
     * Find and return the perspective this object owns.
     * 
     * @return The found perspective or null.
     */
    public SynthComponent getPerspective() {
        SynthComponent current = this.getParent();
        while (current != null) {
            if (current.getComponentType().equals(TYPE_PERSPECTIVE)) {
                return current;
            }
            current = current.getParent();
        }
        return null;
    }
    
    /**
     * Attach an user interface component
     * 
     * @param userInterfaceComponent
     */
    public void attachUserInterfaceComponent(UserInterfaceComponent userInterfaceComponent) {
        this.userInterfaceComponent = userInterfaceComponent;
    }

    /**
     * Get attached user interface component.
     * 
     * @return Attached user interface component. Returns null if no user
     *         interface component is attached.
     */
    public UserInterfaceComponent getUserInterfaceComponent() {
        return userInterfaceComponent;
    }
    
    ///////
    // backend executors for persistent methods.
    
    <T> void backendSetAttribute(String name, T value) throws SynthComponentException
    {
        // do nothing in default
    }

    void backendUnsetAttribute(String name)  throws SynthComponentException
    {
        // do nothing in default
    }
    
    void backendAddSubComponent(SynthComponent subComponent) throws SynthComponentException
    {
        // do nothing in default
    }

    void backendRemoveSubComponent(SynthComponent subComponent) throws SynthComponentException
    {
        // do nothing in default
    }
    
    //////
    
    public final Map<String, Object> collectComponents() {
        Map<String, Object> collection = new LinkedHashMap<String, Object>();
        collection.putAll(attributes);
        for (SynthComponent c : subComponents.values()) {
            SynthComponent component = c;
            collection.put(
                    component.getComponentType() + "." + component.getName(),
                    component.collectComponents());
        }
        return collection;
    }

    @Override
    public String toString() {
        Map<String, Object> subCollection = collectComponents();
        Map<String, Object> collection = new LinkedHashMap<String, Object>();
        collection.put(getComponentType() + "." + getName(), subCollection);
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        return gson.toJson(collection);
    }
}
