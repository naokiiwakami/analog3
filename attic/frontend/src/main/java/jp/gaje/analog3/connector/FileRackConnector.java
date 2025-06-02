package jp.gaje.analog3.connector;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.KnobComponent;
import jp.gaje.analog3.module.ModuleComponent;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.RackComponent;
import jp.gaje.analog3.module.SelectorComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.JsonPrimitive;

public class FileRackConnector extends RackConnector
{
    Map<String, FileModuleConnector> moduleConnectors;

    @Override
    protected void initialize() throws ComponentBuildException
    {
        moduleConnectors = new LinkedHashMap<String, FileModuleConnector>();
    }

    @Override
	public
    List<SynthComponent> recognizeModules() throws ComponentBuildException, SynthComponentException
    {
        String key = rackName + ".connector.directory";
        String dir = properties.getProperty(key);
        if (dir == null) {
            throw new ComponentBuildException(FileRackConnector.class.getSimpleName()
                    + ": mandatory key to specify directory for module files \"" + key
                    + "\" is missing");
        }

        String moduleDir = "conf/" + dir;
        Path modulesFolder = Paths.get(moduleDir);
        if (!Files.exists(modulesFolder)) {
            throw new ComponentBuildException("Module files directory " + moduleDir + " not found");
        }
        if (!Files.isDirectory(modulesFolder)) {
            throw new ComponentBuildException(moduleDir + ": not a directory");
        }

        List<SynthComponent> modules = new ArrayList<SynthComponent>();
        File folder = new File(moduleDir);
        File[] listOfFiles = folder.listFiles();
        for (File file : listOfFiles) {
            if (file.isFile() && file.getName().toLowerCase().endsWith(".json")) {
                ModuleComponent module = create(file, rack);
                moduleConnectors.put(module.getName(), new FileModuleConnector(file, module));
                if (module != null) {
                    modules.add(module);
                }
            }
        }
        return modules;
    }

    public static ModuleComponent create(File file, RackComponent rack)
            throws ComponentBuildException
    {
        FileReader reader = null;
        try {
            reader = new FileReader(file);
        } catch (FileNotFoundException e) {
            throw new ComponentBuildException("file not found", e);
        }

        JsonElement je;
        try {
            je = new JsonParser().parse(reader);
        } catch (Exception ex) {
            throw new ComponentBuildException("JSON file parsing failed.  file=" + file.getName()
                    + ", " + ex.getMessage(), ex);
        }

        JsonObject jo = je.getAsJsonObject();

        ModuleComponent module = null;

        for (Map.Entry<String, JsonElement> entry : jo.entrySet()) {
            String key = entry.getKey();
            String[] words = key.split("\\.", 2);
            if (words.length < 2) {
                // TODO: exception
                System.err.println("invalid object name: " + key);
                continue;
            }
            String componentClass = words[0];
            String name = words[1];
            module = createComponentFromJsonElement(componentClass, name, entry.getValue(), file,
                    rack);
            if (module != null) {
                break;
            }
        }

        return module;
    }

    static ModuleComponent createComponentFromJsonElement(String componentClass, String name,
            JsonElement je, File file, RackComponent rack) throws ComponentBuildException
    {
        if (!componentClass.equalsIgnoreCase("Module")) {
            // TODO: exception
            System.err.println("Component class is expected to be Module");
            return null;
        }
        if (!je.isJsonObject()) {
            // TODO: exception
            System.err.println("Invalid file format");
            return null;
        }
        JsonObject jo = je.getAsJsonObject();
        JsonElement jeModuleType = jo.get("Metadata.moduleType");
        if (jeModuleType == null) {
            System.err.println("Property modulepType is mandatory");
            return null;
        }
        String entryType = jeModuleType.getAsString();
        ModuleComponent module = new ModuleComponent(name, entryType, rack);

        for (Map.Entry<String, JsonElement> entry : je.getAsJsonObject().entrySet()) {
            String key = entry.getKey();
            JsonElement value = entry.getValue();
            if (key.equalsIgnoreCase("moduleType")) {
                // already handled
                continue;
            }
            if (key.startsWith("Port.")) {
                module.addSubComponent(createPort(key, value.getAsJsonObject(), rack));
            } else if (key.startsWith("Selector.")) {
                module.addSubComponent(createSelector(key, value.getAsJsonObject(), rack));
            } else if (key.startsWith("Knob.")) {
                module.addSubComponent(createKnob(key, value.getAsJsonObject(), rack));
            } else {
                try {
                    module.addAttribute(key, value.getAsString());
                } catch (UnsupportedOperationException ex) {
                    throw new ComponentBuildException("value of parameter " + key
                            + " is expected to be string but it's not. " + value);
                }
            }
        }

        return module;
    }

    static SynthComponent createKnob(String classAndName, JsonObject jo, RackComponent rack)
            throws ComponentBuildException
    {
        String[] words = classAndName.split("\\.", 2);
        JsonElement je = jo.get("scale");
        if (je == null) {
            throw new ComponentBuildException("Mandatory property \"scale\" is missing");
        }
        String scale = je.getAsString();

        je = jo.get("value");
        String initial = (je != null) ? je.getAsString() : null;

        KnobComponent knob = new KnobComponent(words[1], scale, initial, rack);
        
        for (Map.Entry<String, JsonElement> entry : jo.entrySet()) {
            String key = entry.getKey();
            JsonElement value = entry.getValue();
            if (key.equals("value") || key.equals("scale")) {
                // already handled
                continue;
            }
            if (key.startsWith("Port.")) {
                knob.addSubComponent(createPort(key, value.getAsJsonObject(), rack));
            } else {
                try {
                    knob.addAttribute(key, value.getAsString());
                } catch (UnsupportedOperationException ex) {
                    throw new ComponentBuildException("value of parameter " + key
                            + " is expected to be string but it's not. " + value);
                }
            }
        }

        return knob;
    }

    static SynthComponent createSelector(String classAndName, JsonObject jo, RackComponent rack)
            throws ComponentBuildException
    {
        String[] words = classAndName.split("\\.", 2);
        JsonElement je = jo.get("choices");
        if (je == null) {
            throw new ComponentBuildException("Mandatory property \"choices\" is missing");
        }
        if (!je.isJsonArray()) {
            throw new ComponentBuildException(classAndName + ": expected to be an array");
        }
        JsonArray ja = je.getAsJsonArray();
        Set<String> choices = new LinkedHashSet<String>();
        for (int i = 0; i < ja.size(); ++i) {
            choices.add(ja.get(i).getAsString());
        }
        je = jo.get("value");
        Integer initial = (je != null) ? je.getAsInt() : null;
        SelectorComponent selector = new SelectorComponent(words[1], choices, initial, rack);

        return selector;
    }

    static SynthComponent createPort(String classAndName, JsonObject jo, RackComponent rack)
            throws ComponentBuildException
    {
        String[] words = classAndName.split("\\.", 2);
        // read direction
        JsonElement je = jo.get(PortComponent.ATTR_DIRECTION);
        if (je == null) {
            throw new ComponentBuildException(
                    "PortComponent: Mandatory property \"direction\" is missing");
        }
        PortComponent.Direction direction = PortComponent.Direction.valueOf(je.getAsString());
        // read signal
        je = jo.get(PortComponent.ATTR_SIGNAL);
        if (je == null) {
            throw new ComponentBuildException(
                    "PortComponent: Mandatory property \"signal\" is missing");
        }
        String signal = je.getAsString();
        PortComponent port = new PortComponent(words[1], direction, signal, rack);
        for (Map.Entry<String, JsonElement> entry : jo.entrySet()) {
            String key = entry.getKey();
            String value = entry.getValue().getAsString();
            if (!key.equalsIgnoreCase(PortComponent.ATTR_DIRECTION)
                    && !key.equalsIgnoreCase(PortComponent.ATTR_SIGNAL)) {
                port.addAttribute(key, value);
            }
        }
        return port;
    }

    static Map<String, Object> createMapFromJsonObject(JsonObject jo)
    {
        Map<String, Object> map = new HashMap<String, Object>();
        for (Map.Entry<String, JsonElement> entry : jo.entrySet()) {
            String key = entry.getKey();
            JsonElement value = entry.getValue();
            map.put(key, getValueFromJsonElement(value));
        }
        return map;
    }

    static Object getValueFromJsonElement(JsonElement je)
    {
        if (je.isJsonObject()) {
            return createMapFromJsonObject(je.getAsJsonObject());
        } else if (je.isJsonArray()) {
            JsonArray array = je.getAsJsonArray();
            List<Object> list = new ArrayList<Object>(array.size());
            for (JsonElement element : array) {
                list.add(getValueFromJsonElement(element));
            }
            return list;
        } else if (je.isJsonNull()) {
            return null;
        } else // must be primitive
        {
            JsonPrimitive p = je.getAsJsonPrimitive();
            if (p.isBoolean())
                return p.getAsBoolean();
            if (p.isString())
                return p.getAsString();
            // else p is number, but don't know what kind
            String s = p.getAsString();
            try {
                return new BigInteger(s);
            } catch (NumberFormatException e) {
                // must be a decimal
                return new BigDecimal(s);
            }
        }
    }

    @Override
	public
    void unsetAttribute(String[] path, String attributeName) throws SynthComponentException
    {
        update(path);
    }

    @Override
	public
    void addSubComponent(String[] path, SynthComponent subComponent) throws SynthComponentException
    {
        update(path);
    }
    

    @Override
	public
    void removeSubComponent(String[] path, SynthComponent subComponent)
            throws SynthComponentException
    {
        update(path);
    }
    void update(String[] path) throws SynthComponentException
    {
        if (path.length < 1 || !moduleConnectors.containsKey(path[0])) {
            // do nothing;
            return;
        }

        FileModuleConnector connector = moduleConnectors.get(path[0]);
        if (connector != null) {
            connector.writeToFile();
        } else {
            // TODO: throw something
        }
    }

    private static class FileModuleConnector
    {
        final File file;
        final SynthComponent module;

        FileModuleConnector(File file, SynthComponent module)
        {
            this.file = file;
            this.module = module;
        }

        void writeToFile() throws SynthComponentException
        {
            FileOutputStream output = null;
            try {
                output = new FileOutputStream(file.getAbsolutePath(), false);
                output.write(module.toString().getBytes());
                output.write('\n');
            } catch (FileNotFoundException e) {
                throw new SynthComponentException(module, "File open error", e);
            } catch (IOException e) {
                throw new SynthComponentException(module, "File write error", e);
            } finally {
                if (output != null) {
                    try {
                        output.close();
                    } catch (IOException e) {
                        // ignore error
                    }
                }
            }
        }
    }

    @Override
    public void setAttribute(String[] backendPath, String attributeName, Object value)
            throws SynthComponentException
    {
        // TODO Auto-generated method stub
        
    }
}
