package jp.gaje.analog3.module;

import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.junit.Test;

public class SynthComponentImplTest {

    @Test
    public void testResolveString() {
        
        SynthComponent root = new SynthComponent(SynthComponent.TYPE_ROOT, "root");
        
        SynthComponent perspective = new SynthComponent(SynthComponent.TYPE_PERSPECTIVE, "test");
        root.addSubComponent(perspective);
        
        RackComponent rack = new RackComponent("testRack");
        
        perspective.addSubComponent(rack);
        
        ModuleComponent vco = new ModuleComponent("vco1", "VCO", rack);
        rack.addSubComponent(vco);
        KnobComponent freq_vco = new KnobComponent("frequency", "1023", "0", rack);
        vco.addSubComponent(freq_vco);
        PortComponent mod_vco = new PortComponent("mod", PortComponent.INPUT, PortComponent.SIGNAL_VALUE, rack);
        vco.addSubComponent(mod_vco);
        
        ModuleComponent vcf = new ModuleComponent("vcf1", "VCF", rack);
        rack.addSubComponent(vcf);
        rack.addSubComponent(vco);
        KnobComponent freq_vcf = new KnobComponent("frequency", "1023", "0", rack);
        vcf.addSubComponent(freq_vcf);
        KnobComponent resonance_vcf = new KnobComponent("resonance", "1023", "0", rack);
        vcf.addSubComponent(resonance_vcf);
        PortComponent mod_vcf = new PortComponent("mod", PortComponent.INPUT, PortComponent.SIGNAL_VALUE, rack);
        vcf.addSubComponent(mod_vcf);
        
        SynthComponent found = rack.resolve("/test/testRack/vco1");
        assertSame(found, vco);

        found = perspective.resolve("/test/testRack/vco1");
        assertSame(found, vco);
        
        found = vco.resolve("/test/testRack/vco1");
        assertSame(found, vco);
        
        found = perspective.resolve("/test/testRack/vco1/mod");
        assertSame(found, mod_vco);

        found = perspective.resolve("/test/testRack/vcf1/resonance");
        assertSame(found, resonance_vcf);

        found = rack.resolve("vco1/frequency");
        assertSame(found, freq_vco);

        found = rack.resolve("vcf1/frequency");
        assertSame(found, freq_vcf);

        found = vco.resolve("frequency");
        assertSame(found, freq_vco);

        found = vcf.resolve("frequency");
        assertSame(found, freq_vcf);
        
        found = vco.resolve("resonance");
        assertNull(found);

        found = rack.resolve("/test/testRack/vcf1");
        assertSame(found, vcf);
        
        found = rack.resolve("/test2/testRack/vcf1");
        assertNull(found);
        
        found = rack.resolve("/test/testRack/vca1");
        assertNull(found);
        
        found = rack.resolve("/");
        assertSame(found, root);
    }

}
