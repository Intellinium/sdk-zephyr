/**
 * @file main..c
 * @author Giuliano FRANCHETTO
 * @date 15 November 2021
 * 
 * @copyright @verbatim
 * Intellinium SAS, 2014-present
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Intellinium SAS and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Intellinium SAS
 * and its suppliers and may be covered by French and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Intellinium SAS.
 * @endverbatim
 * 
 * @brief 
 **/

#include <errno.h>
#include <init.h>

#include <tinycbor/cbor.h>
#include <nrf_rpc_cbor.h>

#include "radio_test.h"

#include "common_ids.h"
#include "bt_hci_rpmsg.h"

/* @formatter:off*/
#define BLE_RPC_DECODER(cmd, cb, var)					       \
	NRF_RPC_CBOR_CMD_DECODER(ble_group, cmd, cmd, cb, var)
/* @formatter:on*/

/* Radio parameter configuration. */
static struct radio_param_config {
	/** Radio transmission pattern. */
	enum transmit_pattern tx_pattern;

	/** Radio mode. Data rate and modulation. */
	nrf_radio_mode_t mode;

	/** Radio output power. */
	uint8_t txpower;

	/** Radio start channel (frequency). */
	uint8_t channel_start;

	/** Radio end channel (frequency). */
	uint8_t channel_end;

	/** Delay time in milliseconds. */
	uint32_t delay_ms;

	/** Duty cycle. */
	uint32_t duty_cycle;
} config = {
	.tx_pattern = TRANSMIT_PATTERN_RANDOM,
	.mode = NRF_RADIO_MODE_BLE_1MBIT,
	.txpower = RADIO_TXPOWER_TXPOWER_0dBm,
	.channel_start = 0,
	.channel_end = 80,
	.delay_ms = 10,
	.duty_cycle = 50,
};

static bool test_in_progress;
static struct radio_test_config test_config;

NRF_RPC_GROUP_DEFINE(ble_group, "nrf_ble_radio", NULL, NULL, NULL);

static void rsp_error_code_send(int err_code)
{
	struct nrf_rpc_cbor_ctx ctx;

	NRF_RPC_CBOR_ALLOC(ctx, 16);

	cbor_encode_int(&ctx.encoder, err_code);

	nrf_rpc_cbor_rsp_no_err(&ctx);
}

static void ble_tx_power(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int tx;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &tx);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || tx < 0) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.txpower = tx;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_TX_POWER, ble_tx_power, NULL);

static void ble_tx_pattern(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int pattern;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &pattern);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || pattern < 0) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.tx_pattern = pattern;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_TRANSMIT_PATTERN, ble_tx_pattern, NULL);

static void ble_start_channel(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int channel;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &channel);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || channel < 0 || channel > 80) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.channel_start = channel;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_CHANNEL, ble_start_channel, NULL);

static void ble_end_channel(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int channel;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &channel);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || channel < 0 || channel > 80) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.channel_end = channel;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_END_CHANNEL, ble_end_channel, NULL);

static void ble_time_on_channel(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int time;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &time);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || time < 0 || time > 99) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.delay_ms = time;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_TIME_ON_CHANNEL, ble_time_on_channel, NULL);

static void ble_cancel(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	nrf_rpc_cbor_decoding_done(packet);

	radio_test_cancel();
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_CANCEL, ble_cancel, NULL);

static void ble_datarate(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int data_rate;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &data_rate);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.mode = data_rate;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_DATA_RATE, ble_datarate, NULL);

static void ble_tx_carrier(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	nrf_rpc_cbor_decoding_done(packet);

	if (test_in_progress) {
		radio_test_cancel();
		test_in_progress = false;
	}

	memset(&test_config, 0, sizeof(test_config));
	test_config.type = UNMODULATED_TX;
	test_config.mode = config.mode;
	test_config.params.unmodulated_tx.txpower = config.txpower;
	test_config.params.unmodulated_tx.channel = config.channel_start;

	radio_test_start(&test_config);

	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_TX_CARRIER, ble_tx_carrier, NULL);

static void tx_modulated_carrier_end(void)
{
	printk("The modulated TX has finished\n");
}

static void ble_start_tx_mod_carrier(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int qty;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &qty);
	nrf_rpc_cbor_decoding_done(packet);

	if (test_in_progress) {
		radio_test_cancel();
		test_in_progress = false;
	}

	memset(&test_config, 0, sizeof(test_config));
	test_config.type = UNMODULATED_TX;
	test_config.mode = config.mode;
	test_config.params.unmodulated_tx.txpower = config.txpower;
	test_config.params.unmodulated_tx.channel = config.channel_start;

	if (cbor_err == CborNoError) {
		test_config.params.modulated_tx.packets_num = qty;
		test_config.params.modulated_tx.cb = tx_modulated_carrier_end;
	}

	radio_test_start(&test_config);

	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_TX_MOD_CARRIER, ble_start_tx_mod_carrier,
		NULL);

static void ble_start_tx_dc_mod_carrier(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	int dc;
	CborError cbor_err;

	cbor_err = cbor_value_get_int(packet, &dc);
	nrf_rpc_cbor_decoding_done(packet);

	if (cbor_err != CborNoError || dc < 0 || dc > 100) {
		rsp_error_code_send(-NRF_EBADMSG);
		return;
	}

	config.duty_cycle = dc;
	memset(&test_config, 0, sizeof(test_config));
	test_config.type = MODULATED_TX_DUTY_CYCLE;
	test_config.mode = config.mode;
	test_config.params.modulated_tx_duty_cycle.txpower = config.txpower;
	test_config.params.modulated_tx_duty_cycle.pattern = config.tx_pattern;
	test_config.params.modulated_tx_duty_cycle.channel =
		config.channel_start;
	test_config.params.modulated_tx_duty_cycle.duty_cycle =
		config.duty_cycle;

	radio_test_start(&test_config);
	test_in_progress = true;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_TX_DC_MOD_CARRIER,
		ble_start_tx_dc_mod_carrier, NULL);

static void ble_start_rx_sweep(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	nrf_rpc_cbor_decoding_done(packet);

	memset(&test_config, 0, sizeof(test_config));
	test_config.type = RX_SWEEP;
	test_config.mode = config.mode;
	test_config.params.rx_sweep.channel_start = config.channel_start;
	test_config.params.rx_sweep.channel_end = config.channel_end;
	test_config.params.rx_sweep.delay_ms = config.delay_ms;

	radio_test_start(&test_config);

	test_in_progress = true;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_RX_SWEEP, ble_start_rx_sweep, NULL);

static void ble_start_tx_sweep(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	nrf_rpc_cbor_decoding_done(packet);

	memset(&test_config, 0, sizeof(test_config));
	test_config.type = TX_SWEEP;
	test_config.mode = config.mode;
	test_config.params.tx_sweep.channel_start = config.channel_start;
	test_config.params.tx_sweep.channel_end = config.channel_end;
	test_config.params.tx_sweep.delay_ms = config.delay_ms;
	test_config.params.tx_sweep.txpower = config.txpower;

	radio_test_start(&test_config);

	test_in_progress = true;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_TX_SWEEP, ble_start_tx_sweep, NULL);

static void ble_start_rx(CborValue *packet, void *handler_data)
{
	ARG_UNUSED(handler_data);

	if (test_in_progress) {
		radio_test_cancel();
		test_in_progress = false;
	}

	nrf_rpc_cbor_decoding_done(packet);

	memset(&test_config, 0, sizeof(test_config));
	test_config.type = RX;
	test_config.mode = config.mode;
	test_config.params.rx.channel = config.channel_start;
	test_config.params.rx.pattern = config.tx_pattern;

	radio_test_start(&test_config);

	test_in_progress = true;
	rsp_error_code_send(0);
}
BLE_RPC_DECODER(RPC_COMMAND_START_RX, ble_start_rx, NULL);

static void err_handler(const struct nrf_rpc_err_report *report)
{
	printk("nRF RPC error %d occurred."
	       "See nRF RPC logs for more details.",
	       report->code);
	k_oops();
}

#include <drivers/clock_control.h>
#include <drivers/clock_control/nrf_clock_control.h>
static void clock_init(void)
{
	int err;
	int res;
	struct onoff_manager *clk_mgr;
	struct onoff_client clk_cli;

	clk_mgr = z_nrf_clock_control_get_onoff(CLOCK_CONTROL_NRF_SUBSYS_HF);
	if (!clk_mgr) {
		printk("Unable to get the Clock manager\n");
		return;
	}

	sys_notify_init_spinwait(&clk_cli.notify);

	err = onoff_request(clk_mgr, &clk_cli);
	if (err < 0) {
		printk("Clock request failed: %d\n", err);
		return;
	}

	do {
		err = sys_notify_fetch_result(&clk_cli.notify, &res);
		if (!err && res) {
			printk("Clock could not be started: %d\n", res);
			return;
		}
	} while (err);

	printk("Clock has started\n");
}

int main(void)
{
	int err;

	printk("Init begin\n");

	clock_init();

	err = nrf_rpc_init(err_handler);
	if (err) {
		return -NRF_EINVAL;
	}

	radio_test_init(&test_config);

	printk("Init done\n");

	bt_hci_rpmsg_loop();

	return 0;
}
