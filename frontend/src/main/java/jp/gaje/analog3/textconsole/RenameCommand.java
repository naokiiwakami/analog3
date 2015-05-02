package jp.gaje.analog3.textconsole;

import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class RenameCommand extends Command {
    
    @Override
    public String getCommandName()
    {
        return "rename";
    }

    @Override
    public String synopsis()
    {
        return "Rename a component.";
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
            throws CommandLineException
    {
        if (parameters.length < 2) {
            throw new CommandLineException("Too few arguments");
        }
        SynthComponent target = textConsole.currentComponent;
        String newName = parameters[1];
        try {
            target.rename(newName);
        } catch (SynthComponentException e) {
            System.err.println(e.getMessage());
        }
        
        return false;
    }
    
    @Override
    public void printUsage()
    {
        System.err.println("Usage: rename <new_name>");
    }

}
