package jp.gaje.analog3.module;

public class OperationNotSupportedException extends SynthComponentException {
    
    /**
     * 
     */
    private static final long serialVersionUID = 8558818850043276825L;

    OperationNotSupportedException(SynthComponent component, String operation)
    {
        super(component, "operation " + operation + " is not supported for this module");
    }

}
