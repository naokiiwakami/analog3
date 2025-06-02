package jp.gaje.analog3.textconsole;

import jp.gaje.analog3.module.SynthComponent;

public class CdCommand extends Command {
    
    @Override
    public String getCommandName()
    {
        return "cd";
    }

    @Override
    public String synopsis()
    {
        return "Change current layer of components.";
    }
    
    @Override
    public void printUsage()
    {
        System.err.println("Usage: cd [path]");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
    {
        SynthComponent current = textConsole.currentComponent;
        if (parameters.length == 1) {
            current = textConsole.rootComponent;
        } else {
            current = Command.specifyComponent(parameters[1],
                    textConsole.rootComponent, current);
        }
        if (current != null) {
            textConsole.currentComponent = current;
        }

        return false;
    }

}
