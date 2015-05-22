package jp.gaje.analog3.textconsole;

import jp.gaje.analog3.module.PerspectiveComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class DisconnectCommand extends Command {

    @Override
    public String getCommandName()
    {
        return "disconnect";
    }

    @Override
    public String synopsis()
    {
        return "Disconnect two ports.";
    }

    @Override
    public void printUsage()
    {
        System.err.println("Usage: disconnect <source_path> <listener_path>");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
            throws CommandLineException
    {
        if (parameters.length < 3) {
            throw new CommandLineException("Too few arguments");
        }
        int iarg = 1;
        final String sourcePath = parameters[iarg++];
        final String listenerPath = parameters[iarg++];

        // find source port
        SynthComponent source = Command.specifyComponent(sourcePath, textConsole.rootComponent,
                textConsole.currentComponent);

        if (source == null) {
            System.err.println("Source " + sourcePath + " not found.");
            return false;
        }

        SynthComponent listener = Command.specifyComponent(listenerPath, textConsole.rootComponent,
                textConsole.currentComponent);

        if (listener == null) {
            System.err.println("Listener " + listenerPath + " not found.");
            return false;
        }

        SynthComponent perspective = source.getPerspective();
        if (perspective == null) {
            System.err.println("Source component does not have perspective.");
            return false;
        }
        SynthComponent listenerPerspective = listener.getPerspective();
        if (listenerPerspective == null || perspective != listenerPerspective) {
            System.err.println("Listener component does not have perspective or has different one from source.");
            return false;
        }

        try {
            Integer wireId = ((PerspectiveComponent) perspective).disconnect(source.getPath(), listener.getPath());
            System.out.println("wireId=" + wireId);
        } catch (SynthComponentException ex) {
            System.err.println(ex.getMessage());
        }

        return false;
    }
}
