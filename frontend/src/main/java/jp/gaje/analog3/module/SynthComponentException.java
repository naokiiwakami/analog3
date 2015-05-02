package jp.gaje.analog3.module;

public class SynthComponentException extends Exception {
    
    final protected SynthComponent component;
    
    private static final long serialVersionUID = 5322244764421495865L;

    public SynthComponentException(SynthComponent component, final String what)
    {
        super(what);
        this.component = component;
    }
    
    public SynthComponentException(SynthComponent component, final Throwable cause)
    {
        super(cause);
        this.component = component;
    }
    
    public SynthComponentException(SynthComponent component, final String what, final Throwable cause)
    {
        super(what, cause);
        this.component = component;
    }
    
    public SynthComponent getComponent() {
        return component;
    }

}
