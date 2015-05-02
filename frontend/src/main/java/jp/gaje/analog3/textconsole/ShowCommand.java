package jp.gaje.analog3.textconsole;

import jp.gaje.analog3.module.SynthComponent;

public class ShowCommand extends Command {
    
    @Override
    public String getCommandName()
    {
        return "show";
    }

    @Override
    public String synopsis()
    {
        return "Show current or specified component.";
    }
    
    @Override
    public void printUsage()
    {
        System.err.println("Usage: show [path]");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
    {
        SynthComponent target = textConsole.currentComponent;
        
        String targetName = "";
        if (parameters.length > 1) {
            targetName = parameters[1];
            target = Command.specifyComponent(targetName,
                    textConsole.rootComponent, target);
        }
        
        if (target != null) {
            System.out.println(target.toString());
        } else {
            System.err.println(parameters[0] + ": " + targetName + ": Sub-component not found");
        }
        
        return false;
    }

}
