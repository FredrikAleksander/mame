CPUS["Z80"] = true
MACHINES["Z80DAISY"] = true
MACHINES["Z80CTC"] = true
MACHINES["CLOCK"] = true
VIDEOS["V9938"] = true

--------------------------------------
-- To be able to use IDE devices    --
--------------------------------------
BUSES["ATA"] = true
SOUNDS["CDDA"] = true
SOUNDS["YM3812"] = true
CPUS["FR"] = true

--------------------------------------
-- To be able to use serial devices --
--------------------------------------
CPUS["IE15"] = true
SOUNDS["YM3812"] = true
SOUNDS["BEEP"] = true
MACHINES["IE15"] = true
MACHINES["SWTPC8212"] = true
CPUS["M6800"] = true
BUSES["RS232"] = true
BUSES["SUNKBD"] = true
MACHINES["6821PIA"] = true
MACHINES["INS8250"] = true
MACHINES["INPUT_MERGER"] = true
VIDEOS["MC6845"] = true

function createProjects_ftxemu_ftxemu(_target, _subtarget)
	project ("ftxemu_ftxemu")
	targetsubdir(_target .."_" .. _subtarget)
	kind (LIBTYPE)
	uuid (os.uuid("drv-ftxemu-ftxemu"))
	addprojectflags()
	precompiledheaders_novs()

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. "mame/layout",
	}

files{
	MAME_DIR .. "src/ftxemu/drivers/ftx.cpp",
}
end

function linkProjects_ftxemu_ftxemu(_target, _subtarget)
	links {
		"ftxemu_ftxemu",
	}
end