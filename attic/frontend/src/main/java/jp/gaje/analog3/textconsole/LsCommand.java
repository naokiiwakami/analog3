package jp.gaje.analog3.textconsole;

import java.util.Map.Entry;

import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SynthComponent;

public class LsCommand extends Command {

    @Override
    public String getCommandName()
    {
        return "ls";
    }

    @Override
    public String synopsis()
    {
        return "List components.";
    }

    @Override
    public void printUsage()
    {
        System.err.println("Usage: ls [options] [path]");
        System.err.println("Options:");
        System.err.println("    -r : recursive");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters) throws CommandLineException
    {
        boolean recursive = false;
        boolean printConnection = false;
        int i;
        for (i = 1; i < parameters.length && parameters[i].startsWith("-"); ++i) {
            String options = parameters[i];
            for (int j = 1; j < options.length(); ++j) {
                char option = options.charAt(j);
                switch (option) {
                case 'r':
                    recursive = true;
                    break;
                case 'c':
                    printConnection = true;
                    break;
                default:
                    throw new CommandLineException("Unknown option: " + option);
                }
            }
        }
        SynthComponent target = textConsole.currentComponent;
        if (i < parameters.length) {
            String path = parameters[i];
            target = Command.specifyComponent(path, textConsole.rootComponent,
                    target);
        }
        if (target != null) {
            list(target, recursive, printConnection, "");
        }
        return false;
    }

    protected void list(SynthComponent target, boolean recursive, boolean printConnection, String indent)
    {
        if (target != null) {
            for (Entry<String, SynthComponent> entry : target.getSubComponents().entrySet()) {
                SynthComponent component = entry.getValue();
                String componentType = component.getComponentType();
                System.out.print(indent + componentType + ":" + entry.getKey());
                if (printConnection && component instanceof PortComponent) {
                    PortComponent port = (PortComponent) component;
                    System.out.print(" (" + port.getSignal() + ") ");
                    if (port.getDirection() == PortComponent.INPUT) {
                        System.out.print("< (");
                    } else {
                        System.out.print("> (");
                    }
                    Integer wireId = port.getWireId();
                    if (wireId != null) {
                        System.out.print(wireId);
                    }
                    System.out.println(")");
                } else {
                    System.out.print("\n");
                }
                if (recursive) {
                    list(entry.getValue(), true, printConnection, indent + "  ");
                }
            }
        }
    }
};
