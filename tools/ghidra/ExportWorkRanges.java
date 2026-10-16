// Export discovery metadata and unreviewed decompiler output for worker ranges.
// Run as an analyzeHeadless post-script; outputs belong under ignored data/.
// @category LR2

import java.io.BufferedWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

public class ExportWorkRanges extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            throw new IllegalArgumentException("Expected an export directory, optionally followed by start:end hex ranges");
        }
        List<long[]> ranges = new ArrayList<>();
        for (int index = 1; index < args.length; index++) {
            String[] limits = args[index].split(":");
            if (limits.length != 2) {
                throw new IllegalArgumentException("Invalid address range: " + args[index]);
            }
            long start = Long.parseUnsignedLong(limits[0].replaceFirst("^0x", ""), 16);
            long end = Long.parseUnsignedLong(limits[1].replaceFirst("^0x", ""), 16);
            if (start >= end) {
                throw new IllegalArgumentException("Range must have start < end: " + args[index]);
            }
            ranges.add(new long[] { start, end });
        }
        if (ranges.isEmpty()) {
            ranges.add(new long[] { 0x45dc60L, 0x45e700L });
            ranges.add(new long[] { 0x4a2000L, 0x4a4000L });
            ranges.add(new long[] { 0x4d0000L, 0x4d2000L });
        }
        Path directory = Path.of(args[0]);
        Files.createDirectories(directory);
        DecompInterface decompiler = new DecompInterface();
        if (!decompiler.openProgram(currentProgram)) {
            throw new IllegalStateException("Could not initialize decompiler");
        }
        int count = 0;
        int exported = 0;
        try (BufferedWriter inventory = Files.newBufferedWriter(directory.resolve("discovered-functions.csv"), StandardCharsets.UTF_8)) {
            inventory.write("address,name,body_bytes,maximum_address,thunk,external\n");
            FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
            while (functions.hasNext() && !monitor.isCancelled()) {
                Function function = functions.next();
                long address = function.getEntryPoint().getOffset();
                count++;
                inventory.write(String.format("%08x,%s,%d,%s,%b,%b%n", address,
                    function.getName().replace(',', '_'), function.getBody().getNumAddresses(),
                    function.getBody().getMaxAddress(), function.isThunk(), function.isExternal()));
                boolean workerRange = false;
                for (long[] range : ranges) {
                    if (address >= range[0] && address < range[1]) {
                        workerRange = true;
                        break;
                    }
                }
                if (!workerRange || function.isThunk() || function.isExternal()) {
                    continue;
                }
                DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
                if (result.decompileCompleted() && result.getDecompiledFunction() != null) {
                    String text = "// UNREVIEWED GHIDRA OUTPUT: confirm types, boundaries and callers.\n" +
                        "// Original VA: " + function.getEntryPoint() + "\n\n" +
                        result.getDecompiledFunction().getC();
                    Files.writeString(directory.resolve(String.format("%08x.c", address)), text, StandardCharsets.UTF_8);
                    exported++;
                }
            }
        } finally {
            decompiler.dispose();
        }
        println("Discovered " + count + " functions; exported " + exported + " worker candidates.");
        println("Discovery counts and body sizes require review; do not use them as verified progress totals.");
    }
}
