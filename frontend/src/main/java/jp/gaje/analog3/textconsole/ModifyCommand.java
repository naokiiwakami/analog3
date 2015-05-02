package jp.gaje.analog3.textconsole;

import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class ModifyCommand extends Command {
    
    @Override
    public String getCommandName()
    {
        return "modify";
    }

    @Override
    public String synopsis()
    {
        return "Modify amodule property.";
    }

    @Override
    public void printUsage()
    {
        System.err.println("Usage: modify <path> <name> <value>");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
            throws CommandLineException
    {
        if (parameters.length < 4) {
            throw new CommandLineException("Too few arguments");
        }
        SynthComponent target = Command.specifyComponent(parameters[1],
                textConsole.rootComponent, textConsole.currentComponent);
        
        if (target != null) {
            String name = parameters[2];
            String value = parameters[3];
            try {
                target.setAttributePersistent(name, value);
            } catch (SynthComponentException e) {
                System.err.println(e.getMessage());
            }
        } else {
            System.err.println(parameters[1] + ": not found");
        }
        
        return false;
    }

}
