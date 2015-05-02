package jp.gaje.analog3.textconsole;

import java.util.Map;
import java.util.TreeMap;

import jp.gaje.analog3.module.SynthComponent;

public abstract class Command {
    
    final static Map<String, Command> processors;
    static {
        processors = new TreeMap<String, Command>();
    }
    
    static Command get(final String command) {
        return processors.get(command);
    }
    
    static void placeCommand(Command command) {
        if (command != null) {
            processors.put(command.getCommandName(), command);
        }
    }
    
    public abstract String getCommandName();
    
    public abstract String synopsis();
    
    public abstract void printUsage();
    
    public abstract boolean execute(TextConsole textConsole,
            final String[] parameters) throws CommandLineException;

    public static SynthComponent specifyComponent(String path, SynthComponent root,
            SynthComponent current)
    {
        SynthComponent target = current;
        String[] dirs = path.split("/");
        for (String dir : dirs) {
            if (dir.isEmpty()) {
                target = root;
            } else if (dir.equals("..")) {
                if (target.getParent() != null) {
                    target = target.getParent();
                }
            } else if (!dir.equalsIgnoreCase(".")) {
                SynthComponent newTarget = target.getSubComponent(dir);
                if (newTarget == null) {
                    // TODO: exception
                    System.err.println(path + ": path not found");
                    return null;
                }
                target = newTarget;
            }
        }
        return target;
    }

}
