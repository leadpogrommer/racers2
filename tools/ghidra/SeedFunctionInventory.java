// Seed callable CRT entries and reviewed function starts before automatic analysis.
// @category LR2

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Set;
import java.util.TreeSet;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.mem.MemoryBlock;

public class SeedFunctionInventory extends GhidraScript {
    private final Set<Long> starts = new TreeSet<>();

    private void add(long value) throws Exception {
        Address address = toAddr(value);
        MemoryBlock block = currentProgram.getMemory().getBlock(address);
        if (block == null || !block.isExecute()) {
            throw new IllegalArgumentException("Non-executable function entry: " + address);
        }
        starts.add(value);
    }

    private void crtTable(long begin, long end) throws Exception {
        for (long slot = begin; slot < end; slot += 4) {
            long value = Integer.toUnsignedLong(getInt(toAddr(slot)));
            if (value == 0) {
                continue;
            }
            add(value);
            // A CRT-called E9 wrapper is itself callable; its unconditional
            // tail target is another entry. Do not treat arbitrary jumps as entries.
            if (getByte(toAddr(value)) == (byte) 0xe9) {
                add(value + 5 + getInt(toAddr(value + 1)));
            }
        }
    }

    @Override
    public void run() throws Exception {
        if (!"6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d"
                .equalsIgnoreCase(currentProgram.getExecutableSHA256())) {
            throw new IllegalArgumentException("Unsupported original executable SHA-256");
        }
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("Expected reccmp/function-sizes.csv path");
        }
        // _initterm calls in the original CRT at 0x0051b180 and 0x0051b18a.
        crtTable(0x0053d000L, 0x0053dee8L);
        crtTable(0x0053deecL, 0x0053df00L);
        for (String line : Files.readAllLines(Path.of(args[0]), StandardCharsets.UTF_8)) {
            String address = line.split(",", 2)[0];
            if (address.equals("address") || address.isBlank()) {
                continue;
            }
            add(Long.parseUnsignedLong(address.replaceFirst("^0x", ""), 16));
        }
        int added = 0;
        int overlapping = 0;
        for (long value : starts) {
            monitor.checkCancelled();
            Address address = toAddr(value);
            if (getFunctionAt(address) != null) {
                continue;
            }
            if (getFunctionContaining(address) != null) {
                println("Review overlapping callable entry: " + address);
                overlapping++;
                continue;
            }
            disassemble(address);
            if (createFunction(address, null) == null) {
                throw new IllegalStateException("Could not create function at " + address);
            }
            added++;
        }
        println("Seeded " + added + " callable functions; " + overlapping + " overlapping entries need review.");
    }
}
