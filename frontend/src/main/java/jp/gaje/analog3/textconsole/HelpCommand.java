package jp.gaje.analog3.textconsole;

public class HelpCommand extends Command {

    @Override
    public String getCommandName()
    {
        return "help";
    }

    @Override
    public String synopsis()
    {
        return "Return help message.";
    }

    @Override
    public void printUsage()
    {
        System.err.println("Usage: help [command]");
    }

    @Override
    public boolean execute(TextConsole textConsole, String[] parameters)
    {

        if (parameters.length < 2) {
            // default behavior is listing all available commands.
            int maxLen = 0;
            for (String command : processors.keySet()) {
                if (maxLen < command.length()) {
                    maxLen = command.length();
                }
            }
            System.out.print("\nAvailable commands:\n\n");
            for (String command : processors.keySet()) {
                Command processor = processors.get(command);
                System.out.print(command);
                for (int i = command.length(); i < maxLen; ++i) {
                    System.out.print(' ');
                }
                System.out.print(" : ");
                System.out.println(processor.synopsis());
            }
            System.out.println("");
        } else {
            String command = parameters[1].toLowerCase();
            Command processor = Command.get(command);
            if (processor != null) {
                processor.printUsage();
            } else {
                System.err.println(parameters[1] + ": Command not found");
            }
        }

        return false;
    }
}
