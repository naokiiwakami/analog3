package jp.gaje.analog3.textconsole;

import java.io.IOException;

import jline.UnsupportedTerminal;
import jline.console.ConsoleReader;
import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.PerspectiveBuilder;
import jp.gaje.analog3.module.RootModule;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class TextConsole {

    static {
        deployCommands();
    }

    SynthComponent rootComponent;
    SynthComponent currentComponent;
    
    final ConsoleReader reader;    
    
    public TextConsole() throws IOException {
        reader = new ConsoleReader();
        initialize(null);
    }

    public TextConsole(final SynthComponent rootComponent, boolean isDumbTerminal) throws IOException {
        if (isDumbTerminal) {
            reader = new ConsoleReader(System.in, System.out, new UnsupportedTerminal());
        } else {
            reader = new ConsoleReader();
        }
        initialize(rootComponent);
    }        
        
    private void initialize(final SynthComponent rootComponent) {

        // bind synth
        try {
            if (rootComponent == null) {
                System.out.println("recognizing modules...");
                this.rootComponent = new RootModule();
                this.rootComponent.addSubComponent(new PerspectiveBuilder("hardware").build());
            } else { 
                this.rootComponent = rootComponent;
            }
            currentComponent = this.rootComponent;
            // System.out.println(hardware.toString());

        } catch (SynthComponentException ex) {
            SynthComponent component = ex.getComponent();
            if (component != null) {
                System.err.println("Component: " + component.getComponentType() + "." + component.getName());
            }
            System.err.println(ex.getMessage());
            System.exit(1);
        } catch (ComponentBuildException ex) {
            System.err.println(ex.getMessage());
            System.exit(1);
        }
    }

    public void runSession() throws IOException {
        reader.setPrompt("analog3> ");
        String line;
        boolean terminateSession = false;
        do {
            line = reader.readLine();
            String[] parameters = line.split("\\s+");
            if (parameters.length > 0 && !parameters[0].isEmpty()) {
                String command = parameters[0].toLowerCase();
                Command processor = Command.get(command);
                if (processor != null) {
                    try {
                        terminateSession = processor.execute(this, parameters);
                    } catch (CommandLineException e) {
                        System.err.println(e.getMessage());
                        processor.printUsage();
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                } else {
                    System.err.println(parameters[0] + ": Command not found");
                }
            }
        } while (!terminateSession);
    }
    
    public static void main(String[] args) throws IOException {
        TextConsole console = new TextConsole();
        System.out.println("ready");
        console.runSession();
    }

    private static void deployCommands() {
        Command.placeCommand(new PwdCommand());
        Command.placeCommand(new CdCommand());
        Command.placeCommand(new LsCommand());
        Command.placeCommand(new ShowCommand());
        Command.placeCommand(new RenameCommand());
        Command.placeCommand(new ModifyCommand());
        Command.placeCommand(new ConnectCommand());
        Command.placeCommand(new DisconnectCommand());
        Command.placeCommand(new HelpCommand());
        Command.placeCommand(new QuitCommand());
    }
}
