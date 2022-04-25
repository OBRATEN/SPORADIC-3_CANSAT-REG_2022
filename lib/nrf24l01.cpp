#include "nrf24l01.hpp"

// CE
#define CE_DDR    DDRB
#define CE_PORT   PORTB
#define CE_PIN    PINB4
// CSN
#define CSN_DDR   DDRB
#define CSN_PORT  PORTB
#define CSN_PIN   PINB0

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

void nRF24_radio::csn(bool mode) {
	if (mode == 1) CSN_DDR |= (1 << CSN_PIN);
	else CSN_DDR &= ~(1 << CSN_PIN);
}

void nRF24_radio::ce(bool mode) {
	if (mode == 1) CE_DDR |= (1 << CE_PIN);
	else CE_DDR &= ~(1 << CE_PIN);
}

inline void nRF24_radio::beginTransaction(void) {
	csn(0);
}

inline void nRF24_radio::endTransaction(void) {
	csn(1);
}

void nRF24_radio::readReg(uint8_t reg, uint8_t* buf, uint8_t len) {
	beginTransaction();
	_status = _spi.transfer(R_REGISTER | reg);
	while (len--) (*buf++ = _spi.transfer(0xFF));
	endTransaction();
}

uint8_t nRF24_radio::readReg(uint8_t reg) {
	uint8_t result;
	beginTransaction();
	_status = _spi.transfer(R_REGISTER | reg);
	result = _spi.transfer(0xff);
	endTransaction();
	return result;
}

void nRF24_radio::writeReg(uint8_t reg, const uint8_t* buf, uint8_t len) {
	beginTransaction();
	_status = _spi.transfer(W_REGISTER | reg);
	while (len--) _spi.transfer(*buf++);
	endTransaction();
}

void nRF24_radio::writeReg(uint8_t reg, uint8_t value, bool is_cmd_only) {
	if (is_cmd_only) {
		beginTransaction();
		_status = _spi.transfer(W_REGISTER | reg);
		endTransaction();
	} else{
		beginTransaction();
		_status = _spi.transfer(W_REGISTER | reg);
		_spi.transfer(value);
		endTransaction();
	}
}

void nRF24_radio::writePayload(const void* buf, uint8_t data_len, const uint8_t writeType) {
	const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
	uint8_t blank_len = !data_len ? 1 : 0;
	if (!dynamic_payloads_enabled) {
		data_len = min(data_len, _payloadSize);
		blank_len = static_cast<uint8_t>(_payloadSize - data_len);
	} else data_len = min(data_len, static_cast<uint8_t>(32));
	beginTransaction();
	_status = _spi.transfer(writeType);
	while (data_len--) { _spi.transfer(*current++); }
	while (blank_len--) { _spi.transfer(0); }
	endTransaction();
}

void nRF24_radio::readPayload(void* buf, uint8_t data_len) {
	uint8_t* current = reinterpret_cast<uint8_t*>(buf);
  uint8_t blank_len = 0;
  if (!dynamic_payloads_enabled) {
    data_len = min(data_len, _payloadSize);
    blank_len = static_cast<uint8_t>(_payloadSize - data_len);
  } else data_len = max(data_len, static_cast<uint8_t>(32));
  beginTransaction();
  _status = _spi.transfer(R_RX_PAYLOAD);
  while (data_len--) {*current++ = _spi.transfer(0xFF);}
  while (blank_len--) {_spi.transfer(0xFF);}
  endTransaction();
}

uint8_t nRF24_radio::flush_rx(void) {
  writeReg(FLUSH_RX, RF24_NOP, true);
  return _status;
}

uint8_t nRF24_radio::flush_tx(void) {
  writeReg(FLUSH_TX, RF24_NOP, true);
  return _status;
}

uint8_t nRF24_radio::getStatus(void) {
  writeReg(RF24_NOP, RF24_NOP, true);
  return _status;
}

nRF24_radio::nRF24_radio(uint32_t _spi_speed)
        :_spiSpeed(_spi_speed), _payloadSize(32), _is_p_variant(false), _is_p0_rx(false),
        addr_width(5), dynamic_payloads_enabled(true), csDelay(5)
{
    _init_obj();
}

void nRF24_radio::_init_obj() {
    pipe0_reading_address[0] = 0;
    if(_spiSpeed <= 35000) _spiSpeed = RF24_SPI_SPEED;
}

void nRF24_radio::setChannel(uint8_t channel) {
    const uint8_t max_channel = 125;
    writeReg(RF_CH, min(channel, max_channel));
}

bool nRF24_radio::begin(void) {
	return _init_pins() && _init_radio();
}

void nRF24_radio::openWritingPipe(uint64_t value) {
  writeReg(RX_ADDR_P0, reinterpret_cast<uint8_t*>(&value), addr_width);
  writeReg(TX_ADDR, reinterpret_cast<uint8_t*>(&value), addr_width);
}

bool nRF24_radio::_init_pins(void) {
	CE_DDR |= (1 << CE_PIN);
	ce(0);
	CSN_DDR |= (1 << CSN_PIN);
	csn(1);
	_delay_ms(100);
	return 1;
}

bool nRF24_radio::_init_radio(void) {
	_delay_ms(5);
	setRetries(5, 15);
	setRadiation(RF24_PA_MAX, RF24_1MBPS);
	uint8_t beforeToggle = readReg(FEATURE);
	toggleFeatures();
	uint8_t afterToggle = readReg(FEATURE);
	_is_p_variant = beforeToggle == afterToggle;
	if (afterToggle) {
		if (_is_p_variant) toggleFeatures();
		writeReg(FEATURE, 0);
	}
	ack_payloads_enabled = false;
  writeReg(DYNPD, 0);
  dynamic_payloads_enabled = false;
  writeReg(EN_AA, 0x3F);
  writeReg(EN_RXADDR, 3);
  setPayloadSize(32);
  setAddressWidth(5);
  setChannel(76);
  writeReg(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
  flush_rx();
  flush_tx();
  writeReg(NRF_CONFIG, ((1 << EN_CRC) | (1 << CRCO)));
  _configReg = readReg(NRF_CONFIG);
  powerUp();
  return _configReg == ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP)) ? true : false;
}

bool nRF24_radio::isChipConnected() {
  return readReg(SETUP_AW) == (addr_width - static_cast<uint8_t>(2));
}

void nRF24_radio::powerDown(void) {
  ce(0); // Guarantee CE is low on powerDown
  _configReg = static_cast<uint8_t>(_configReg & ~(1 << PWR_UP));
  writeReg(NRF_CONFIG, _configReg);
}

void nRF24_radio::powerUp(void) {
  if (!(_configReg & (1 << PWR_UP))) {
    _configReg |= (1 << PWR_UP);
    writeReg(NRF_CONFIG, _configReg);
    _delay_us(RF24_POWERUP_DELAY);
  }
}

bool nRF24_radio::write(const void* buf, uint8_t len, const bool multicast) {
  startFastWrite(buf, len, multicast);
  while (!(getStatus() & ((1 << TX_DS) | (1 << MAX_RT))));
  ce(0);
  writeReg(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
  if (_status & (1 << MAX_RT)) {
    flush_tx();
    return 0;
  } return 1;
}

bool nRF24_radio::write(const void* buf, uint8_t len) {
  return write(buf, len, 0);
}

bool nRF24_radio::writeFast(const void* buf, uint8_t len, const bool multicast) {
  while ((getStatus() & ((1 << TX_FULL)))) if (_status & (1 << MAX_RT)) return 0;
  startFastWrite(buf, len, multicast);
  return 1;
}

bool nRF24_radio::writeFast(const void* buf, uint8_t len) {
    return writeFast(buf, len, 0);
}

void nRF24_radio::startFastWrite(const void* buf, uint8_t len, const bool multicast, bool startTx) {
  writePayload(buf, len, multicast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD);
  if (startTx) ce(1);
}

bool nRF24_radio::startWrite(const void* buf, uint8_t len, const bool multicast) {
  writePayload(buf, len, multicast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD);
  ce(1);
  _delay_ms(1);
  ce(0);
  return !(_status & (1 << TX_FULL));
}

bool nRF24_radio::rxFifoFull() {
  return readReg(FIFO_STATUS) & (1 << RX_FULL);
}

bool nRF24_radio::txStandBy() {
  while (!(readReg(FIFO_STATUS) & (1 << TX_EMPTY))) {
    if (_status & (1 << MAX_RT)) {
      writeReg(NRF_STATUS, (1 << MAX_RT));
      ce(0);
      flush_tx();
      return 0;
    }
  }
  ce(0);
  return 1;
}

void nRF24_radio::setAddressWidth(uint8_t a_width) {
  a_width = static_cast<uint8_t>(a_width - 2);
  if (a_width) {
    writeReg(SETUP_AW, static_cast<uint8_t>(a_width % 4));
    addr_width = static_cast<uint8_t>((a_width % 4) + 2);
  } else {
    writeReg(SETUP_AW, static_cast<uint8_t>(0));
    addr_width = static_cast<uint8_t>(2);
  }
}

void nRF24_radio::toggleFeatures(void) {
  beginTransaction();
  _status = _spi.transfer(ACTIVATE);
  _spi.transfer(0x73);
  endTransaction();
}

void nRF24_radio::setPALevel(uint8_t level, bool lnaEnable) {
  uint8_t setup = readReg(RF_SETUP) & static_cast<uint8_t>(0xF8);
  setup |= _pa_level_reg_value(level, lnaEnable);
  writeReg(RF_SETUP, setup);
}

bool nRF24_radio::setDataRate(rf24_datarate_e speed) {
  bool result = false;
  uint8_t setup = readReg(RF_SETUP);
  setup = static_cast<uint8_t>(setup & ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH)));
  setup |= _data_rate_reg_value(speed);
  writeReg(RF_SETUP, setup);
  if (readReg(RF_SETUP) == setup) {
    result = true;
  } return result;
}

void nRF24_radio::setRetries(uint8_t delay, uint8_t count) {
  writeReg(SETUP_RETR, static_cast<uint8_t>(min(15, delay) << ARD | min(15, count)));
}

void nRF24_radio::setRadiation(uint8_t level, rf24_datarate_e speed, bool lnaEnable) {
  uint8_t setup = _data_rate_reg_value(speed);
  setup |= _pa_level_reg_value(level, lnaEnable);
  writeReg(RF_SETUP, setup);
}

void nRF24_radio::setPayloadSize(uint8_t size) {
  _payloadSize = static_cast<uint8_t>(max(1, min(32, size)));
  for (uint8_t i = 0; i < 6; ++i) writeReg(static_cast<uint8_t>(RX_PW_P0 + i), _payloadSize);
}

uint8_t nRF24_radio::_pa_level_reg_value(uint8_t level, bool lnaEnable) {
  return static_cast<uint8_t>(((level > RF24_PA_MAX ? static_cast<uint8_t>(RF24_PA_MAX) : level) << 1) + lnaEnable);
}

uint8_t nRF24_radio::_data_rate_reg_value(rf24_datarate_e speed) {
  txDelay = 85;
  if (speed == RF24_250KBPS) {
    txDelay = 155;
    return static_cast<uint8_t>((1 << RF_DR_LOW));
  } else if (speed == RF24_2MBPS) {
    txDelay = 65;
    return static_cast<uint8_t>((1 << RF_DR_HIGH));
  } return static_cast<uint8_t>(0);
}