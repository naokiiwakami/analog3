package jp.gaje.analog3.module;

/**
 * The UserInterfaceComponent interaface should be implemented by any class
 * whose instances are intended to be called back by SynthComponent objects.
 */
public interface UserInterfaceComponent
{

    /**
     * This method is called back from SynthComponent when a parameter value is
     * changed. This method is useful to update user face view on parameter
     * change.
     * 
     * @param attributeName
     *            Parameter name.
     * @param newValue
     *            Parameter value.
     */
    public void notifyParameterAdded(final String attributeName, final String newValue);

    /**
     * This method is called back from SynthComponent when a parameter value is
     * removed.
     * 
     * @param attributeName
     *            Parameter name to remove.
     * @param value
     *            Parameter value to remove.
     */
    public void notifyParameterRemoved(final String attributeName, final String value);

    /**
     * This method is called back from SynthComponent when a sub-component is
     * added during operations.
     * 
     * @param subComponent
     *            Added sub-component.
     */
    public void notifySubComponentAdded(final SynthComponent subComponent);

    /**
     * This method is called back from SynthComponent when a sub-component is
     * removed during operations.
     * 
     * @param subComponent
     *            Removed sub-component.
     */
    public void notifySubComponentRemoved(final SynthComponent subComponent);

}
