package jp.gaje.analog3.module;

public class ComponentBuildException extends Exception {
    
    /**
     * 
     */
    private static final long serialVersionUID = -7517546764306994L;

    public ComponentBuildException() {
        super();
    }
    
    public ComponentBuildException(String message) {
        super(message);
    }
    
    public ComponentBuildException(String message, Throwable cause) {
        super(message, cause);
    }
}
