package jp.gaje.analog3.module;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.Before;
import org.junit.Test;

public class PerspectiveImplConnectTest {

    private PerspectiveComponent perspective;

    private String sourcePathMidiNote1 = "/perspective/rack/midi/note1";
    private String sourcePathMidiNote2 = "/perspective/rack/midi/note2";
    private String sourcePathLfoWave = "/perspective/rack/lfo/wave";
    private String sourcePathEgOut = "/perspective/rack/eg/out";

    private String listenerPathVcoFrequency = "/perspective/rack/vco/frequency";
    private String listenerPathVcoNote = "/perspective/rack/vco/note";
    private String listenerPathLfoGate = "/perspective/rack/lfo/gate";
    private String listenerPathLfoFrequency = "/perspective/rack/lfo/frequency";
    private String listenerPathEgAttack = "/perspective/rack/eg/attack";

    @Before
    public void setUp() throws Exception {
        SynthComponent root = new RootModule();

        perspective = new PerspectiveComponent("perspective");
        root.addSubComponent(perspective);

        RackComponent rack = new RackComponent("rack");
        perspective.addSubComponent(rack);

        ModuleComponent midi = new ModuleComponent("midi", "MIDI", rack);
        rack.addSubComponent(midi);
        midi.addSubComponent(new PortComponent("note1", PortComponent.OUTPUT, PortComponent.SIGNAL_NOTE, rack));
        midi.addSubComponent(new PortComponent("note2", PortComponent.OUTPUT, PortComponent.SIGNAL_NOTE, rack));

        ModuleComponent lfo = new ModuleComponent("lfo", "LFO", rack);
        rack.addSubComponent(lfo);
        lfo.addSubComponent(new PortComponent("wave", PortComponent.OUTPUT, PortComponent.SIGNAL_VALUE, rack));
        lfo.addSubComponent(new PortComponent("gate", PortComponent.INPUT, PortComponent.SIGNAL_NOTE, rack));
        lfo.addSubComponent(new KnobComponent("frequency", "1023", "10", rack));

        ModuleComponent vco = new ModuleComponent("vco", "VCO", rack);
        rack.addSubComponent(vco);
        vco.addSubComponent(new PortComponent("note", PortComponent.INPUT, PortComponent.SIGNAL_NOTE, rack));
        vco.addSubComponent(new KnobComponent("frequency", "32767", "16384", rack));

        ModuleComponent eg = new ModuleComponent("eg", "EG", rack);
        rack.addSubComponent(eg);
        eg.addSubComponent(new KnobComponent("attack", "1023", "0", rack));
        eg.addSubComponent(new KnobComponent("decay", "1023", "0", rack));
        eg.addSubComponent(new KnobComponent("sustain", "1023", "0", rack));
        eg.addSubComponent(new KnobComponent("release", "1023", "0", rack));
        eg.addSubComponent(new PortComponent("out", PortComponent.OUTPUT, PortComponent.SIGNAL_VALUE, rack));
    }

    @Test
    public void testConnect01() {

        try {
            // make one connection
            assertEquals(Integer.valueOf(1), perspective.connect(sourcePathMidiNote1, listenerPathLfoGate));
            WireComponent wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            assertEquals(sourcePathMidiNote1, wire.getSourcePath());
            assertTrue(wire.getListenerPaths().contains(listenerPathLfoGate));

            // connect to another listener from the same source
            String listenerPathVcoNote = "/perspective/rack/vco/note";
            assertEquals(Integer.valueOf(1), perspective.connect(sourcePathMidiNote1, listenerPathVcoNote));
            assertNotNull(perspective.resolveWire("w1"));
            assertNull(perspective.resolveWire("w2"));
            assertEquals(2, wire.getListenerPaths().size());
            assertTrue(wire.getListenerPaths().contains(listenerPathLfoGate));
            assertTrue(wire.getListenerPaths().contains(listenerPathVcoNote));

            // connect from a port to a knob
            assertEquals(Integer.valueOf(2), perspective.connect(sourcePathLfoWave, listenerPathVcoFrequency));
            WireComponent wire2 = perspective.resolveWire("w2");
            assertNotNull(wire2);
            assertEquals(sourcePathLfoWave, wire2.getSourcePath());
            assertTrue(wire2.getListenerPaths().contains(listenerPathVcoFrequency + "/w2"));
            assertNotNull(perspective.resolve(listenerPathVcoFrequency + "/w2"));
            assertEquals(Integer.valueOf(2), ((PortComponent) perspective.resolve(listenerPathVcoFrequency + "/w2")).getWireId());

            // connect to the same knob
            assertEquals(Integer.valueOf(3), perspective.connect(sourcePathEgOut, listenerPathVcoFrequency));
            WireComponent wire3 = perspective.resolveWire("w3");
            assertNotNull(wire3);
            assertEquals(sourcePathEgOut, wire3.getSourcePath());
            assertTrue(wire3.getListenerPaths().contains(listenerPathVcoFrequency + "/w3"));
            
            // connect from a port to a knob that is already connected.
            assertEquals(Integer.valueOf(2), perspective.connect(sourcePathLfoWave, listenerPathVcoFrequency));
            assertNull(perspective.resolveWire("w4"));
            
        } catch (SynthComponentException e) {
            e.printStackTrace();
            fail("Unexpected exception: " + e.getMessage());
        }

        try {
            // connect to a listener that is already occupied
            perspective.connect(sourcePathMidiNote2, listenerPathLfoGate);
            fail("expected excpetion didn't happen");
        } catch (SynthComponentException e) {
        }
        assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote2)).getWireId());
        assertNull(perspective.resolveWire("w4"));
    }

    // negative cases
    @Test
    public void testConnect02() {
        // input and input (value)
        try {
            perspective.connect(listenerPathVcoFrequency, listenerPathLfoFrequency);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // input and input (note)
        try {
            perspective.connect(listenerPathVcoNote, listenerPathLfoGate);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));

        // output and output (value)
        try {
            perspective.connect(sourcePathLfoWave, sourcePathEgOut);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // output and output (note)
        try {
            perspective.connect(sourcePathMidiNote1, sourcePathMidiNote2);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // make sure no wireId's are attached.
        assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
        assertTrue(perspective.resolve(listenerPathLfoFrequency).getSubComponents().isEmpty());
        assertNull(((PortComponent) perspective.resolve(sourcePathLfoWave)).getWireId());
        assertNull(((PortComponent) perspective.resolve(sourcePathEgOut)).getWireId());
        assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
        assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote2)).getWireId());
        assertNull(((PortComponent) perspective.resolve(listenerPathVcoNote)).getWireId());
        assertNull(((PortComponent) perspective.resolve(listenerPathLfoGate)).getWireId());
    }
    
    // signal mismatch test
    @Test
    public void testConnect03() {
        try {
            perspective.connect(sourcePathMidiNote1, listenerPathVcoFrequency);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        try {
            perspective.connect(sourcePathLfoWave, listenerPathVcoNote);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // make sure no wireId's are attached.
        assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
        assertNull(((PortComponent) perspective.resolve(sourcePathLfoWave)).getWireId());
        assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
        assertNull(((PortComponent) perspective.resolve(listenerPathVcoNote)).getWireId());
    }
    
    // try to connect from/to wrong place
    @Test
    public void testConnect04() {
        // source does not exist
        try {
            perspective.connect("abc", listenerPathVcoFrequency);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // listener does not exist
        try {
            perspective.connect(sourcePathMidiNote1, "abc");
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // source is not pluggable
        try {
            perspective.connect("/perspective/rack/midi", listenerPathVcoFrequency);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // listener is not pluggable
        try {
            perspective.connect(sourcePathMidiNote1, "/perspective/rack/vco");
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNull(perspective.resolveWire("w1"));
        
        // make sure no wireId's are attached.
        assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
        assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
    }
    
    @Test
    public void testDisconnect() {
        try {
            ////////////////////////////////////
            // make a wire with a port listener
            perspective.connect(sourcePathMidiNote1, listenerPathVcoNote);
            
            // disconnect
            perspective.disconnect(sourcePathMidiNote1, listenerPathVcoNote);
            assertNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
            assertNull(((PortComponent) perspective.resolve(listenerPathVcoNote)).getWireId());
            assertNull(perspective.resolveWire("w1"));

            //////////////////////////////////////
            // make a wire with two port listeners
            perspective.connect(sourcePathMidiNote1, listenerPathVcoNote);
            perspective.connect(sourcePathMidiNote1, listenerPathLfoGate);
            
            // disconnect one listener
            perspective.disconnect(sourcePathMidiNote1, listenerPathLfoGate);
            assertNotNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
            assertNotNull(((PortComponent) perspective.resolve(listenerPathVcoNote)).getWireId());
            assertNull(((PortComponent) perspective.resolve(listenerPathLfoGate)).getWireId());
            WireComponent wire;
            assertNotNull((wire = perspective.resolveWire("w1")));
            assertEquals(sourcePathMidiNote1, wire.getSourcePath());
            assertEquals(1, wire.getListenerPaths().size());
            
            /////////////////////////////////////////
            // make another wire with a knob listener
            perspective.connect(sourcePathLfoWave, listenerPathVcoFrequency);
            
            // disconnect them
            perspective.disconnect(sourcePathLfoWave, listenerPathVcoFrequency + "/w2");
            assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
            assertNull((wire = perspective.resolveWire("w2")));
            
            /////////////////////////////////////////
            // make a wire with two knob listeners
            perspective.connect(sourcePathLfoWave, listenerPathVcoFrequency);
            perspective.connect(sourcePathLfoWave, listenerPathEgAttack);
            
            // disconnect one
            perspective.disconnect(sourcePathLfoWave, listenerPathVcoFrequency + "/w2");
            assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
            assertNotNull(perspective.resolve(listenerPathEgAttack));
            assertNotNull((wire = perspective.resolveWire("w2")));
            assertEquals(sourcePathLfoWave, wire.getSourcePath());
            assertEquals(1, wire.getListenerPaths().size());
            
        } catch (SynthComponentException ex) {
            fail("Unexpected exception: " + ex.getMessage());
        }
        
        // Try disconnecting ports that are not connected
        try {
            perspective.connect(sourcePathMidiNote1, listenerPathEgAttack);
            fail("expected exception didn't happen");
        } catch (SynthComponentException ex) {
        }
        assertNotNull(((PortComponent) perspective.resolve(sourcePathMidiNote1)).getWireId());
        assertNotNull(((PortComponent) perspective.resolve(listenerPathVcoNote)).getWireId());
        WireComponent wire;
        assertNotNull((wire = perspective.resolveWire("w1")));
        assertEquals(sourcePathMidiNote1, wire.getSourcePath());
        assertEquals(1, wire.getListenerPaths().size());

        assertTrue(perspective.resolve(listenerPathVcoFrequency).getSubComponents().isEmpty());
        assertNotNull(perspective.resolve(listenerPathEgAttack));
        assertNotNull((wire = perspective.resolveWire("w2")));
        assertEquals(sourcePathLfoWave, wire.getSourcePath());
        assertEquals(1, wire.getListenerPaths().size());

    }
}
