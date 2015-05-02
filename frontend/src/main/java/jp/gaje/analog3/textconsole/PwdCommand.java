package jp.gaje.analog3.textconsole;

import java.util.ArrayList;
import java.util.List;

import jp.gaje.analog3.module.SynthComponent;

public class PwdCommand extends Command {
    
    @Override
    public String getCommandName()
    {
        return "pwd";
    }

    @Override
    public String synopsis()
    {
        return "Show current component layer.";
    }
    
    @Override
    public void printUsage()
    {
        System.err.println("Usage: pwd");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
    {
        SynthComponent current = textConsole.currentComponent;
        List<String> names = new ArrayList<String>();
        for (SynthComponent c = current; c != null; c = c.getParent()) {
            names.add(c.getName());
        }
        StringBuilder sb = new StringBuilder();
        int size = names.size();
        for (int i = size; --i >= 0; ) {
            sb.append(names.get(i));
            if (i > 0 || sb.length() == 0) {
                sb.append("/");
            }
        }
        System.out.println(sb);
        return false;
    }

}
