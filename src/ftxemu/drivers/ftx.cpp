#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/ins8250.h"
#include "sound/3812intf.h"
#include "bus/rs232/rs232.h"
#include "machine/clock.h"
#include "bus/ata/ataintf.h"
#include "speaker.h"
#include "video/v9938.h"
#include "machine/z80ctc.h"

#define DECLARE_READ8_MEMBER(name)      u8     name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 mem_mask = 0xff)
#define DECLARE_WRITE8_MEMBER(name)     void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 data, ATTR_UNUSED u8 mem_mask = 0xff)

#define READ8_MEMBER(name)              u8     name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 mem_mask)
#define WRITE8_MEMBER(name)             void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 data, ATTR_UNUSED u8 mem_mask)

class ftx_state : public driver_device
{
public:
	ftx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_ata(*this, "ata")
		, m_v9958(*this, "v9958")
		, m_region_maincpu(*this, "maincpu")
		, m_bank1(*this, "bank1")
		, m_bank2(*this, "bank2")
		, m_bank3(*this, "bank3")
		, m_bank4(*this, "bank4")
		
	{ }

	void ftx(machine_config &config);

private:
	DECLARE_READ8_MEMBER(ide_cs0_r);
	DECLARE_WRITE8_MEMBER(ide_cs0_w);
	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_WRITE8_MEMBER(ram_w);

	void mem_map(address_map &map);
	void mem_io(address_map &map);

	void update_banks();

	virtual void machine_start() override;
	virtual void machine_reset() override;
	
	uint8_t m_bank_reg[4];
	required_device<z80_device> m_maincpu;
	required_device<ata_interface_device> m_ata;
	required_device<v9958_device> m_v9958;
	required_memory_region m_region_maincpu;
	required_memory_bank m_bank1;
	required_memory_bank m_bank2;
	required_memory_bank m_bank3;
	required_memory_bank m_bank4;
};

static const z80_daisy_config daisy_chain[] =
{
	{ "ctc" },
	{ nullptr }
};

// The 2 IDE functions here are used because IDE is a 16-bit
// bus, but ATA supports running in a 8-bit mode (rarely implemented
// by harddrives, commonly implemented by CF cards and SD to IDE adapters)

READ8_MEMBER(ftx_state::ide_cs0_r)
{
	return m_ata->cs0_r(offset);
}

WRITE8_MEMBER(ftx_state::ide_cs0_w)
{
	m_ata->cs0_w(offset, data);
}

WRITE8_MEMBER(ftx_state::bank_w)
{
	m_bank_reg[offset & 3] = data;
	update_banks();
}

/******************************************************************************
 Machine Start/Reset
******************************************************************************/

void ftx_state::machine_start()
{
}

void ftx_state::update_banks()
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	uint8_t *mem = m_region_maincpu->base();

	m_bank1->set_base(mem + (m_bank_reg[0] << 14));
	m_bank2->set_base(mem + (m_bank_reg[1] << 14));
	m_bank3->set_base(mem + (m_bank_reg[2] << 14));
	m_bank4->set_base(mem + (m_bank_reg[3] << 14));

	if (m_bank_reg[0] & 0x80) {
		space.unmap_write(0x0000, 0x3fff);
	} else {
		space.install_write_bank(0x0000, 0x3fff, "bank1");
	}
	if (m_bank_reg[1] & 0x80) {
		space.unmap_write(0x4000, 0x7fff);
	} else {
		space.install_write_bank(0x4000, 0x7fff, "bank2");
	}
	if (m_bank_reg[2] & 0x80) {
		space.unmap_write(0x8000, 0xbfff);
	} else {
		space.install_write_bank(0x8000, 0xbfff, "bank3");
	}
	if (m_bank_reg[3] & 0x80) {
		space.unmap_write(0xc000, 0xffff);
	} else {
		space.install_write_bank(0xc000, 0xffff, "bank4");
	}
}

void ftx_state::machine_reset()
{
	m_bank_reg[0] = 128;
	m_bank_reg[1] = 0;
	m_bank_reg[2] = 2;
	m_bank_reg[3] = 3;
	update_banks();
}
/******************************************************************************
 Address Maps
******************************************************************************/

void ftx_state::mem_map(address_map &map)
{
	map(0x0000, 0x3fff).bankrw("bank1");
	map(0x4000, 0x7fff).bankrw("bank2");
	map(0x8000, 0xbfff).bankrw("bank3");
	map(0xc000, 0xffff).bankrw("bank4");
}

void ftx_state::mem_io(address_map &map)
{
	map.global_mask(0xff);
	map(0x20, 0x23).rw("ctc", FUNC(z80ctc_device::read), FUNC(z80ctc_device::write));
	map(0x18, 0x1f).rw("ns16550", FUNC(ns16550_device::ins8250_r), FUNC(ns16550_device::ins8250_w));
	map(0x10, 0x11).rw("ym3812", FUNC(ym3812_device::read_port_r), FUNC(ym3812_device::write_port_w));
	map(0x98, 0x9B).rw("v9958", FUNC(v9958_device::read), FUNC(v9958_device::write));
	map(0x08, 0x0F).rw(FUNC(ftx_state::ide_cs0_r), FUNC(ftx_state::ide_cs0_w));
	map(0xFC, 0xFF).w(FUNC(ftx_state::bank_w));
}

/******************************************************************************
 Input Ports
******************************************************************************/

static INPUT_PORTS_START( ftx )
INPUT_PORTS_END

static DEVICE_INPUT_DEFAULTS_START( terminal )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_9600 )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_9600 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_0 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END

/******************************************************************************
 Machine Drivers
******************************************************************************/

void ftx_state::ftx(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(4'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &ftx_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &ftx_state::mem_io);
	m_maincpu->set_daisy_config(daisy_chain);

	z80ctc_device& ctc(Z80CTC(config, "ctc", 4'000'000));
	ctc.intr_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);

	// NEED VERIFICATION: From what I can gather, interrupts in MAME equals 1.
	//                    With the exception of the UART, all devices in the FTX
	//                    use active-low interrupts, so when connecting these
	//                    as inputs to the CTC, they must be inverted so they are 0 when
	//                    asserted, or software written for hardware will not work
	//                    in the emulator and vice versa

	clock_device &uart_clock(CLOCK(config, "uart_clock", 1'843'200));
	uart_clock.signal_handler().set("ns16550", FUNC(ns16550_device::tx_clock_w));
	uart_clock.signal_handler().append("ns16550", FUNC(ns16550_device::rx_clock_w));

	ns16550_device &uart(NS16550(config, "ns16550", 0));
	uart.out_tx_callback().set("rs232", FUNC(rs232_port_device::write_txd));
	uart.out_dtr_callback().set("rs232", FUNC(rs232_port_device::write_dtr));
	uart.out_rts_callback().set("rs232", FUNC(rs232_port_device::write_rts));
	uart.out_int_callback().set("ctc", FUNC(z80ctc_device::trg3));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set("ns16550", FUNC(ins8250_uart_device::rx_w));
	rs232.dcd_handler().set("ns16550", FUNC(ins8250_uart_device::dcd_w));
	rs232.dsr_handler().set("ns16550", FUNC(ins8250_uart_device::dsr_w));
	rs232.ri_handler().set("ns16550", FUNC(ins8250_uart_device::ri_w));
	rs232.cts_handler().set("ns16550", FUNC(ins8250_uart_device::cts_w));

	rs232.set_option_device_input_defaults("terminal", DEVICE_INPUT_DEFAULTS_NAME(terminal));

	speaker_device &speaker(SPEAKER(config, "mono"));
	speaker.front_center();

	ym3812_device &ym3812(YM3812(config, "ym3812", 3579545));
	ym3812.add_route(ALL_OUTPUTS, speaker, 1.0);
	ym3812.irq_handler().set("ctc", FUNC(z80ctc_device::trg1)).invert();

	V9958(config, m_v9958, XTAL(21'477'272));
	m_v9958->set_screen_pal("screen");
	m_v9958->set_vram_size(0x30000);
	m_v9958->int_cb().set("ctc", FUNC(z80ctc_device::trg0)).invert();

	SCREEN(config, "screen", SCREEN_TYPE_RASTER);

	ATA_INTERFACE(config, m_ata).options(ata_devices, "hdd", nullptr, false);
}


/******************************************************************************
 ROM Definitions
******************************************************************************/

ROM_START( ftx )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "ftx_monitor.rom", 0x200000, 0x36, CRC(113ef4f0) SHA1(862b53abbe35f87fddb5198f742058b8d9317d2e))
ROM_END


/******************************************************************************
 Drivers
******************************************************************************/

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT   CLASS         INIT        COMPANY                FULLNAME  FLAGS
COMP( 2020, ftx,    0,      0,      ftx,     ftx,    ftx_state,    empty_init, "Fredrik Kristiansen", "FTX",    MACHINE_TYPE_COMPUTER )
