package jp.gaje.analog3.softwarerack;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

public class SoftwareRackTest {

    @Test
    public void testMakeTitleA() {
        runMakeTitleTest("one", "One");
    }

    @Test
    public void testMakeTitleB() {
        runMakeTitleTest("twoWords", "Two Words");
    }

    @Test
    public void testMakeTitleC() {
        runMakeTitleTest("nowThreeWords", "Now Three Words");
    }
    
    @Test
    public void testMakeTitleD() {
        runMakeTitleTest("port0", "Port 0");
    }
    
    @Test
    public void testMakeTitleE() {
        runMakeTitleTest("already hasSpace 0", "Already Has Space 0");
    }
    
    @Test
    public void testMakeTitleF() {
        runMakeTitleTest("LFO01", "LFO 01");
    }
    
    @Test
    public void testMakeTitleG() {
        runMakeTitleTest("analogPWM", "Analog PWM");
    }
    
    private void runMakeTitleTest(String source, String expected) {
        String actual = SoftwareRack.makeTitle(source);
        // System.out.println("source=" + source + ", expected=" + expected + ", actual=" + actual);
        assertEquals(expected, actual);
    }

}
