/*
 *  Copyright (C) 2020 GrammaTech, Inc.
 *
 *  This code is licensed under the MIT license. See the LICENSE file in the
 *  project root for license terms.
 *
 *  This project is sponsored by the Office of Naval Research, One Liberty
 *  Center, 875 N. Randolph Street, Arlington, VA 22203 under contract #
 *  N68335-17-C-0700.  The content of the information does not necessarily
 *  reflect the position or policy of the Government and no official
 *  endorsement should be inferred.
 *
 */

package com.grammatech.gtirb;

import com.grammatech.gtirb.proto.ModuleOuterClass;
import java.util.ArrayList;
import java.util.List;
import java.util.NavigableMap;
import java.util.TreeMap;
import java.util.UUID;

public class Module extends Node {

    public enum FileFormat {
        Format_Undefined,
        COFF,
        ELF,
        PE,
        IdaProDb32,
        IdaProDb64,
        XCOFF,
        MACHO,
        RAW
    }

    public enum ISA {
        ISA_Undefined,
        IA32,
        PPC32,
        X64,
        ARM,
        ValidButUnsupported,
        PPC64,
        ARM64,
        MIPS32,
        MIPS64
    }

    public enum ByteOrder { ByteOrder_Undefined, BigEndian, LittleEndian }

    private com.grammatech.gtirb.proto.ModuleOuterClass.Module protoModule;

    private String binaryPath;
    private long preferredAddr;
    private long rebaseDelta;
    private ISA isa;
    private FileFormat fileFormat;
    private ArrayList<Section> sectionList;
    private ArrayList<Symbol> symbolList;
    private ArrayList<ProxyBlock> proxyBlockList;
    private AuxData auxData;
    private String name;
    private UUID entryPointUuid;
    private ByteOrder byteOrder;
    private NavigableMap<Long, Block> blockAddressMap;
    private boolean blockAddressMapInitialized;

    public Module(
        com.grammatech.gtirb.proto.ModuleOuterClass.Module protoModule) {
        this.protoModule = protoModule;

        UUID myUuid = Util.byteStringToUuid(protoModule.getUuid());
        super.setUuid(myUuid);
        this.binaryPath = protoModule.getBinaryPath();
        this.preferredAddr = protoModule.getPreferredAddr();
        this.rebaseDelta = protoModule.getRebaseDelta();
        this.fileFormat = FileFormat.values()[protoModule.getFileFormatValue()];
        this.isa = ISA.values()[protoModule.getIsaValue()];
        this.name = protoModule.getName();
        this.entryPointUuid =
            Util.byteStringToUuid(protoModule.getEntryPoint());
        this.byteOrder = ByteOrder.values()[protoModule.getByteOrderValue()];
        this.blockAddressMap = new TreeMap<Long, Block>();
        this.blockAddressMapInitialized = false;
    }

    public boolean initializeSectionList() {

        this.sectionList = new ArrayList<Section>();

        // For each section, add to sectionList in this class
        List<com.grammatech.gtirb.proto.SectionOuterClass.Section>
            protoSectionList = protoModule.getSectionsList();
        for (com.grammatech.gtirb.proto.SectionOuterClass.Section protoSection :
             protoSectionList) {
            Section newSection = new Section(protoSection);
            sectionList.add(newSection);
        }
        return true;
    }

    public boolean initializeSymbolList() {

        this.symbolList = new ArrayList<Symbol>();

        // For each symbol, add to symbolList in this class
        List<com.grammatech.gtirb.proto.SymbolOuterClass.Symbol>
            protoSymbolList = protoModule.getSymbolsList();
        for (com.grammatech.gtirb.proto.SymbolOuterClass.Symbol protoSymbol :
             protoSymbolList) {
            Symbol newSymbol = new Symbol(protoSymbol);
            symbolList.add(newSymbol);
        }
        return true;
    }

    public boolean initializeProxyBlockList() {

        this.proxyBlockList = new ArrayList<ProxyBlock>();

        // For each proxy block, add to proxyBlockList in this class
        List<com.grammatech.gtirb.proto.ProxyBlockOuterClass.ProxyBlock>
            protoProxyBlockList = protoModule.getProxiesList();
        for (com.grammatech.gtirb.proto.ProxyBlockOuterClass
                 .ProxyBlock protoProxyBlock : protoProxyBlockList) {
            ProxyBlock newProxyBlock = new ProxyBlock(protoProxyBlock);
            proxyBlockList.add(newProxyBlock);
        }
        return true;
    }

    public boolean initializeAuxData() {
        this.auxData = new AuxData(this.protoModule.getAuxDataMap());
        return true;
    }

    public ArrayList<Section> getSections() { return this.sectionList; }

    public ArrayList<Symbol> getSymbols() { return this.symbolList; }

    public ArrayList<ProxyBlock> getProxyBlockList() { return proxyBlockList; }

    public FileFormat getFileFormat() { return this.fileFormat; }

    public ISA getISA() { return this.isa; }

    public ByteOrder getByteOrder() { return this.byteOrder; }

    public String getName() { return this.name; }

    public AuxData getAuxData() { return this.auxData; }

    public CodeBlock getEntryPoint() {
        Node cb = Node.getByUuid(this.entryPointUuid);
        if ((cb != null) && (cb instanceof CodeBlock))
            return (CodeBlock)cb;
        return null;
    }

    public String getBinaryPath() { return binaryPath; }

    public long getPreferredAddr() { return preferredAddr; }

    public long getRebaseDelta() { return rebaseDelta; }

    public ModuleOuterClass.Module getProtoModule() { return this.protoModule; }

    public boolean initializeBlockAddressMap() {
        if (this.blockAddressMapInitialized) {
            return true;
        }

        // Remove old entries if there are any
        this.blockAddressMap.clear();

        // Iterate through all blocks in all sections, storing the address
        for (Section section : this.sectionList) {
            for (ByteInterval byteInterval : section.getByteIntervals()) {
                Long byteIntervalAddress = byteInterval.getAddress();
                if (byteIntervalAddress == null) {
                    continue;
                }
                for (Block block : byteInterval.getBlockList()) {
                    Long blockStartAddress =
                        byteIntervalAddress + block.getOffset();
                    this.blockAddressMap.put(blockStartAddress, block);
                }
            }
        }
        this.blockAddressMapInitialized = true;
        return true;
    }

    public Block getBlockFromAddress(Long address) {
        //
        // Make sure the address map has been initialized
        //     IF/WHEN block addresses are ever changed,
        //     blockAddressMapInitialized should be set to false, so
        //     that the map will be refreshed before being used.
        //
        if (this.blockAddressMapInitialized != true) {
            if (this.initializeBlockAddressMap() != true) {
                return null;
            }
        }

        Block block = this.blockAddressMap.floorEntry(address).getValue();
        if (block != null) {
            long size;
            if (block.getCodeBlock() != null) {
                size = block.getCodeBlock().getSize();
            } else if (block.getDataBlock() != null) {
                size = block.getDataBlock().getSize();
            } else {
                return null;
            }
            Long byteIntervalAddress = block.getByteInterval().getAddress();
            Long blockEndAddress =
                byteIntervalAddress + block.getOffset() + size;
            if (address < blockEndAddress) {
                return block;
            }
        }
        return null;
    }
}
