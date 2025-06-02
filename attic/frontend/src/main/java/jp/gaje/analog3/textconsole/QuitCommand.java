package jp.gaje.analog3.textconsole;

public class QuitCommand extends Command {

    @Override
    public String getCommandName()
    {
        return "quit";
    }

    @Override
    public String synopsis()
    {
        return "Disconnect the console from the synth.";
    }

    @Override
    public void printUsage()
    {
        System.err.println("Usage: quit");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
    {
        System.out.println("bye");
        return true;
    }

}
