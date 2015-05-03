package jp.gaje.analog3.connector;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import jp.gaje.analog3.connector.ConnectorMessages.Attribute;
import jp.gaje.analog3.connector.ConnectorMessages.Component;
import jp.gaje.analog3.connector.ConnectorMessages.Reply;
import jp.gaje.analog3.connector.ConnectorMessages.Request;
import jp.gaje.analog3.connector.ConnectorMessages.Value;
import jp.gaje.analog3.module.ComponentBuildException;
import jp.gaje.analog3.module.KnobComponent;
import jp.gaje.analog3.module.ModuleComponent;
import jp.gaje.analog3.module.PortComponent;
import jp.gaje.analog3.module.SelectorComponent;
import jp.gaje.analog3.module.SynthComponent;
import jp.gaje.analog3.module.SynthComponentException;

public class NetRackConnector extends RackConnector
{

    private final static String propertyClass = "connector";

    Socket socket = null;
    String host;
    int port;

    @Override
    protected void initialize() throws ComponentBuildException
    {
        String key = rackName + "." + propertyClass + ".host";
        host = properties.getProperty(key);
        if (host == null) {
            throw new ComponentBuildException(
                    NetRackConnector.class.getSimpleName()
                            + ": mandatory key to specify NetSynth host name \""
                            + key + "\" is missing");
        }
        key = rackName + "." + propertyClass + ".port";
        String portString = properties.getProperty(key);
        if (portString == null) {
            throw new ComponentBuildException(
                    NetRackConnector.class.getSimpleName()
                            + ": mandatory key to specify NetSynth port \""
                            + key + "\" is missing");
        }
        try {
            port = Integer.parseInt(portString);
        } catch (NumberFormatException ex) {
            throw new ComponentBuildException(
                    NetRackConnector.class.getSimpleName() + ": " + key
                            + ": Invalid port number.");
        }
        try {
            socket = new Socket(host, port);
            socket.setKeepAlive(true);
        } catch (IOException ex) {
            throw new ComponentBuildException(
                    NetRackConnector.class.getSimpleName()
                            + ": Failed to connect the NetSynth. host=" + host
                            + ", port=" + portString, ex);
        }
    }

    @Override
    public List<SynthComponent> recognizeModules()
            throws SynthComponentException, ComponentBuildException
    {
        // Fetch modules description via the rack connector.
        Reply reply;
        try {
            Request.Builder rb = Request.newBuilder();
            rb.setCommand(Request.Command.DESCRIBE);
            Request request = rb.build();
            sendRequest(request);

            reply = receiveReply();
            System.out.println(reply);

        } catch (IOException ex) {
            throw new ComponentBuildException(
                    NetRackConnector.class.getSimpleName()
                            + ": Network error. host=" + host + ", port="
                            + port, ex);
        }

        // Build modules from the description. 
        List<SynthComponent> components = new ArrayList<SynthComponent>();
        for (Component cnComponent : reply.getComponentList()) {
            SynthComponent module = fetchComponent(cnComponent);
            if (module != null) {
                components.add(module);
            }
        }

        return components;
    }
    
    @Override
    public void setAttribute(String[] backendPath, String attributeName, Object value)
            throws SynthComponentException
    {
        // Reply reply;
        try {
            Request.Builder requestBuilder = Request.newBuilder();
            
            requestBuilder.setCommand(Request.Command.SET_ATTRIBUTE);
            
            for (String token : backendPath) {
                requestBuilder.addPath(token);
            }
            
            Attribute.Builder attributeBuilder = Attribute.newBuilder();
            attributeBuilder.setName(attributeName);
            Value.Builder vb = Value.newBuilder();
            if (value instanceof Integer) {
                vb.setIvalue((Integer) value);
            } else if (value instanceof String) {
                vb.setSvalue((String) value);
            }
            attributeBuilder.setValue(vb);
            requestBuilder.setAttribute(attributeBuilder);
            
            Request request = requestBuilder.build();
            sendRequest(request);

            // TODO: handle errors
            Reply reply = receiveReply();

        } catch (IOException ex) {
            throw new SynthComponentException(null,
                    NetRackConnector.class.getSimpleName() + ": Network error. host="
                            + host + ", port=" + port, ex);
        }
    }
    
    @Override
    public void unsetAttribute(String[] backendPath, String attributeName)
            throws SynthComponentException
    {
        try {
            Request.Builder requestBuilder = Request.newBuilder();
            
            requestBuilder.setCommand(Request.Command.UNSET_ATTRIBUTE);
            
            for (String token : backendPath) {
                requestBuilder.addPath(token);
            }
            
            Attribute.Builder attributeBuilder = Attribute.newBuilder();
            attributeBuilder.setName(attributeName);
            requestBuilder.setAttribute(attributeBuilder);
            
            Request request = requestBuilder.build();
            sendRequest(request);

            // TODO: handle errors
            Reply reply = receiveReply();

        } catch (IOException ex) {
            throw new SynthComponentException(null,
                    NetRackConnector.class.getSimpleName() + ": Network error. host="
                            + host + ", port=" + port, ex);
        }
    }

    @Override
    public void addSubComponent(String[] backendPath,
            SynthComponent subComponent) throws SynthComponentException
    {
        try {
            Request.Builder requestBuilder = Request.newBuilder();
            
            requestBuilder.setCommand(Request.Command.ADD_SUBCOMPONENT);
            
            for (String token : backendPath) {
                requestBuilder.addPath(token);
            }
            
            Component.Builder componentBuilder = Component.newBuilder();
            componentBuilder.setName(subComponent.getFullName());
            Map<String, Object> attributes = subComponent.getAttributes();
            for (Map.Entry<String, Object> attribute : attributes.entrySet()) {
                Attribute.Builder attributeBuilder = Attribute.newBuilder();
                attributeBuilder.setName(attribute.getKey());
                Object value = attribute.getValue();
                Value.Builder valueBuilder = Value.newBuilder();
                if (value instanceof String) {
                    valueBuilder.setSvalue((String) value);
                } else if (value instanceof Integer) {
                    valueBuilder.setIvalue((Integer) value);
                }
                attributeBuilder.setValue(valueBuilder);
                componentBuilder.addAttribute(attributeBuilder);
            }
            requestBuilder.setComponent(componentBuilder);
            
            Request request = requestBuilder.build();
            sendRequest(request);

            // TODO: handle errors
            Reply reply = receiveReply();

        } catch (IOException ex) {
            throw new SynthComponentException(null,
                    NetRackConnector.class.getSimpleName() + ": Network error. host="
                            + host + ", port=" + port, ex);
        }
    }

    @Override
    public void removeSubComponent(String[] backendPath,
            SynthComponent subComponent) throws SynthComponentException
    {
        try {
            Request.Builder requestBuilder = Request.newBuilder();
            
            requestBuilder.setCommand(Request.Command.REMOVE_SUBCOMPONENT);
            
            for (String token : backendPath) {
                requestBuilder.addPath(token);
            }
            
            Component.Builder componentBuilder = Component.newBuilder();
            componentBuilder.setName(subComponent.getFullName());
            requestBuilder.setComponent(componentBuilder);
            
            Request request = requestBuilder.build();
            sendRequest(request);

            // TODO: handle errors
            Reply reply = receiveReply();

        } catch (IOException ex) {
            throw new SynthComponentException(null,
                    NetRackConnector.class.getSimpleName() + ": Network error. host="
                            + host + ", port=" + port, ex);
        }
    }

    protected void sendRequest(Request request) throws IOException
    {
        byte[] payload = request.toByteArray();
        int length = payload.length;
        byte[] header = new byte[] { (byte) (length >> 24),
                (byte) (length >> 16), (byte) (length >> 8), (byte) length };

        OutputStream ostream = socket.getOutputStream();
        ostream.write(header);
        ostream.write(payload);
        ostream.flush();
    }
    
    protected Reply receiveReply() throws IOException
    {
        InputStream istream = socket.getInputStream();
        byte[] header = new byte[4];
        istream.read(header, 0, 4);
        int length = ((header[0] & 0xff) << 24) + ((header[1] & 0xff) << 16)
                + ((header[2] & 0xff) << 8) + (header[3] & 0xff);
        byte[] data = new byte[length];
        int remaining = length;
        while (remaining > 0) {
            int nread = istream.read(data, length - remaining, remaining);
            if (nread <= 0) {
                break;
            }
            remaining -= nread;
        }
        return Reply.parseFrom(data);
    }
    
    private SynthComponent fetchComponent(Component cnComponent)
    {
        // fetch name
        String name = cnComponent.getName();
        String[] words = name.split("\\.", 2);
        if (words.length < 2) {
            // TODO: exception
            System.err.println(name + ": invalid object name.");
            return null;
        }
        String componentType = words[0];
        String componentName = words[1];

        // Fetch attributes
        Map<String, Object> attributes = new LinkedHashMap<String, Object>();
        for (Attribute cnAttribute : cnComponent.getAttributeList()) {
            Object value = fetchValue(cnAttribute.getValue());
            if (value != null) {
                attributes.put(cnAttribute.getName(), value);
            }
        }

        // Build the output component
        SynthComponent component = null;
        try {
            if (componentType.equalsIgnoreCase(SynthComponent.TYPE_MODULE)) {
                component = new ModuleComponent(componentName, attributes, rack);
            } else if (componentType.equalsIgnoreCase(SynthComponent.TYPE_KNOB)) {
                component = new KnobComponent(componentName, attributes, rack);
            } else if (componentType.equalsIgnoreCase(SynthComponent.TYPE_SELECTOR)) {
                component = new SelectorComponent(componentName, attributes, rack);
            } else if (componentType.equalsIgnoreCase(SynthComponent.TYPE_PORT)) {
                component = new PortComponent(componentName, attributes, rack);
            }
        } catch (ComponentBuildException ex) {
            System.err.println(name + ": Component build error: " + ex);
            return null;
        }
        
        if (component == null) {
            // unknown component
            return null;
        }
        
        // Build subcomponents
        for (Component cnSubComponent : cnComponent.getSubComponentList()) {
            SynthComponent subComponent = fetchComponent(cnSubComponent);
            if (subComponent != null) {
                component.addSubComponent(subComponent);
            }
            else {
                System.err.println(component.getPath() + ": failed to create a sub component");
            }
        }
        
        return component;
    }
    
    Object fetchValue(Value cnValue)
    {
        Object value = null;
        if (cnValue.hasSvalue()) {
            value = cnValue.getSvalue(); 
        } else if (cnValue.hasIvalue()) {
            value = cnValue.getIvalue();
        } else if (cnValue.hasDvalue()) {
            value = cnValue.getDvalue();
        } else if (cnValue.getAvalueCount() > 0) {
            List<Object> values = new ArrayList<Object>(cnValue.getAvalueCount());
            for (Value avalue : cnValue.getAvalueList()) {
                values.add(fetchValue(avalue));
            }
            value = values;
        }
        return value;
    }
}
