package jp.gaje.analog3.module;

import static org.junit.Assert.*;

import java.util.Set;

import org.junit.Test;

public class PerspectiveImplTest {

    @Test
    public void testGetNextWireId00() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        String nextWireId = perspective.getNextWireId();
        assertEquals(nextWireId, "1");
    }

    @Test
    public void testGetNextWireId01() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        perspective.addUsedWireId(2);
        perspective.addUsedWireId(3);
        String nextWireId = perspective.getNextWireId();
        assertEquals(nextWireId, "4");
    }

    @Test
    public void testGetNextWireId02() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(2);
        perspective.addUsedWireId(3);
        String nextWireId = perspective.getNextWireId();
        assertEquals(nextWireId, "1");
    }

    @Test
    public void testGetNextWireId03() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        perspective.addUsedWireId(3);
        perspective.addUsedWireId(4);
        String nextWireId = perspective.getNextWireId();
        assertEquals(nextWireId, "2");
    }

    @Test
    public void testGetNextWireId04() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        perspective.addUsedWireId(2);
        perspective.addUsedWireId(4);
        String nextWireId = perspective.getNextWireId();
        assertEquals(nextWireId, "3");
    }

    @Test
    public void testGetNextWireId05() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        for (Integer i = 1; i <= PerspectiveComponent.MAX_WIREID; ++i) {
            perspective.addUsedWireId(i);
        }
        String nextWireId = perspective.getNextWireId();
        assertNull(nextWireId);
    }
    
    @Test
    public void addUsedWireTest01() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        perspective.addUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
    }

    @Test
    public void addUsedWireTest02() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
        perspective.addUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
        perspective.removeUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
    }

    @Test
    public void addUsedWireTest03() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
        perspective.addUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
        perspective.removeUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "2");
        perspective.removeUsedWireId(1);
        assertEquals(perspective.getNextWireId(), "1");
    }

    @Test
    public void addUsedWireTest04() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        perspective.addUsedWireId(1);
        assertEquals("2", perspective.getNextWireId());
        perspective.removeUsedWireId(1);
        assertEquals("1", perspective.getNextWireId());
        perspective.addUsedWireId(1);
        assertEquals("2", perspective.getNextWireId());
        perspective.removeUsedWireId(1);
        assertEquals("1", perspective.getNextWireId());
    }
    
    @Test
    public void wireBetweenPortsTest() {
        PerspectiveComponent perspective = new PerspectiveComponent("test");
        
        // basic functionality
        try {
            WireComponent wire = perspective.resolveWire("w1");
            assertNull(wire);
            perspective.wireBetweenPorts("src", "dst11", "1");
            wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            assertEquals("src", wire.getSourcePath());
            Set<String> listeners = wire.getListenerPaths();
            assertEquals(1, listeners.size());
            for (String path : listeners) {
                assertEquals("dst11", path);
            }
            assertEquals("1", wire.getWireId());
        } catch (SynthComponentException e) {
            fail("exception");
        }

        boolean passed;
        // negative: give null source
        try {
            perspective.wireBetweenPorts(null, "dst", "1");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // negative: give empty source
        try {
            perspective.wireBetweenPorts("", "dst", "1");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // negative: give null listener
        try {
            perspective.wireBetweenPorts("src", null, "1");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // negative: give empty listener
        try {
            perspective.wireBetweenPorts("src", "", "1");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // negative: give null wireId
        try {
            perspective.wireBetweenPorts("src", "dst11", null);
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // negative: give empty wireId
        try {
            perspective.wireBetweenPorts("src", "dst11", "");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
        
        // add another listener to wire
        try {
            WireComponent wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            perspective.wireBetweenPorts("src", "dst12", "1");
            wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            assertEquals("src", wire.getSourcePath());
            Set<String> listeners = wire.getListenerPaths();
            assertEquals(2, listeners.size());
            assertTrue(listeners.contains("dst11"));
            assertTrue(listeners.contains("dst12"));
            assertEquals("1", wire.getWireId());
        } catch (SynthComponentException e) {
            fail("exception");
        }
        
        // add existing listener to wire -- nothing should change.
        try {
            WireComponent wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            perspective.wireBetweenPorts("src", "dst12", "1");
            wire = perspective.resolveWire("w1");
            assertNotNull(wire);
            assertEquals("src", wire.getSourcePath());
            Set<String> listeners = wire.getListenerPaths();
            assertEquals(2, listeners.size());
            assertTrue(listeners.contains("dst11"));
            assertTrue(listeners.contains("dst12"));
            assertEquals("1", wire.getWireId());
        } catch (SynthComponentException e) {
            fail("exception");
        }

        // add another wire
        try {
            WireComponent wire = perspective.resolveWire("w2");
            assertNull(wire);
            perspective.wireBetweenPorts("src2", "dst21", "2");
            wire = perspective.resolveWire("w2");
            assertNotNull(wire);
            assertEquals("src2", wire.getSourcePath());
            Set<String> listeners = wire.getListenerPaths();
            assertEquals(1, listeners.size());
            assertTrue(listeners.contains("dst21"));
            assertEquals("2", wire.getWireId());
        } catch (SynthComponentException e) {
            fail("exception");
        }
        
        // negative: try to add a connection with wrong source path
        try {
            perspective.wireBetweenPorts("wrongSrc", "dst13", "1");
            passed = true;
        } catch (SynthComponentException e) {
            passed = false;
        }
        assertFalse(passed);
    }
 }
