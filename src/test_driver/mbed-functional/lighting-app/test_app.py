# Copyright (c) 2009-2020 Arm Limited
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pytest
import subprocess
from time import sleep

from chip.setup_payload import SetupPayload
from chip import exceptions

from common.utils import scan_chip_ble_devices, run_wifi_provisioning

from common.pigweed_client import PigweedClient
from button_service import button_service_pb2
from lighting_service import lighting_service_pb2
from device_service import device_service_pb2

import logging
log = logging.getLogger(__name__)

DEVICE_NODE_ID=1234
RPC_PROTOS = [button_service_pb2, lighting_service_pb2, device_service_pb2]

@pytest.mark.smoketest
def test_smoke_test(device):
    device.reset(duration=1)
    ret = device.wait_for_output("Starting CHIP task")
    assert ret != None and len(ret) > 0

@pytest.mark.pigweedRpcTest
def test_smoke_test_rpc(device):
    pw_client = PigweedClient(device, RPC_PROTOS)
    status, payload = pw_client.rpcs.chip.rpc.Device.GetDeviceInfo()
    assert status.ok() == True
    assert payload.vendor_id != None
    assert payload.product_id != None

    ret = device.wait_for_output("SetupQRCode")
    assert ret != None and len(ret) > 1

    qr_code = ret[-1].split()[-1].strip("[]")
    device_details = dict(SetupPayload().ParseQrCode("VP:vendorpayload%{}".format(qr_code)).attributes)
    assert device_details != None and len(device_details) != 0
    assert int(device_details["VendorID"]) == payload.vendor_id
    assert int(device_details["ProductID"]) == payload.product_id

@pytest.mark.deviceControllerTest
def test_wifi_provisioning_dev_ctrl(device, network, device_controller):
    network_ssid = network[0]
    network_pass = network[1]
    
    ret = device.wait_for_output("SetupQRCode")
    assert ret != None and len(ret) > 1

    qr_code = ret[-1].split()[-1].strip("[]")
    device_details = dict(SetupPayload().ParseQrCode("VP:vendorpayload%{}".format(qr_code)).attributes)
    assert device_details != None and len(device_details) != 0

    ble_chip_device = scan_chip_ble_devices(device_controller)
    assert ble_chip_device != None and len(ble_chip_device) != 0

    chip_device_found = False

    for ble_device in ble_chip_device:
        if (int(ble_device.discriminator) == int(device_details["Discriminator"]) and
            int(ble_device.vendorId) == int(device_details["VendorID"]) and
            int(ble_device.productId) == int(device_details["ProductID"])):
            chip_device_found = True
            break

    assert chip_device_found

    ret = run_wifi_provisioning(device_controller, network_ssid, network_pass, int(device_details["Discriminator"]), int(device_details["SetUpPINCode"]), DEVICE_NODE_ID)
    assert ret != None and ret == DEVICE_NODE_ID

@pytest.mark.chipToolTest
def test_wifi_provisioning_chip_tool(device, network, chip_tools_dir):
    network_ssid = network[0]
    network_pass = network[1]
    
    ret = device.wait_for_output("SetupQRCode")
    assert ret != None and len(ret) > 1

    qr_code = ret[-1].split()[-1].strip("[]")
    device_details = dict(SetupPayload().ParseQrCode("VP:vendorpayload%{}".format(qr_code)).attributes)
    assert device_details != None and len(device_details) != 0

    process = subprocess.Popen(["./chip-tool", "pairing", "ble", network_ssid, network_pass, str(device_details["SetUpPINCode"]), str(device_details["Discriminator"])], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    assert "ConnectDevice complete" in out and "Network Provisioning Success" in out


@pytest.mark.deviceControllerTest
def test_light_ctrl_dev_ctrl(device, network, device_controller):
    network_ssid = network[0]
    network_pass = network[1]
    
    ret = device.wait_for_output("SetupQRCode")
    assert ret != None and len(ret) > 1

    qr_code = ret[-1].split()[-1].strip("[]")
    device_details = dict(SetupPayload().ParseQrCode("VP:vendorpayload%{}".format(qr_code)).attributes)
    assert device_details != None and len(device_details) != 0

    ret = run_wifi_provisioning(device_controller, network_ssid, network_pass, int(device_details["Discriminator"]), int(device_details["SetUpPINCode"]), DEVICE_NODE_ID)
    assert ret == DEVICE_NODE_ID

    device.flush(5)

    args = {}
    
    try:
        # Check on command
        err, res = device_controller.ZCLSend("OnOff", "On", DEVICE_NODE_ID, 1, 0, args, blocking=True)
        assert err == 0

        ret = device.wait_for_output("Lock Action has been completed")
        assert ret != None and len(ret) > 1

        # Check off command
        err, res = device_controller.ZCLSend("OnOff", "Off", DEVICE_NODE_ID, 1, 0, args, blocking=True)
        assert err == 0

        ret = device.wait_for_output("Unlock Action has been completed")
        assert ret != None and len(ret) > 1

        # Check toggle command
        err, res = device_controller.ZCLSend("OnOff", "Toggle", DEVICE_NODE_ID, 1, 0, args, blocking=True)
        assert err == 0

        ret = device.wait_for_output("Lock Action has been completed")
        assert ret != None and len(ret) > 1

        # Check move to level command
        args["level"] = 200
        args["transitionTime"] = 1
        args["optionMask"] = 0
        args["optionOverride"] = 0
        err, res = device_controller.ZCLSend("LevelControl", "MoveToLevel", DEVICE_NODE_ID, 1, 0, args, blocking=True)
        assert err == 0

        ret = device.wait_for_output("Level Action has been completed")
        assert ret != None and len(ret) > 1

    except exceptions.ChipStackException as ex:
        log.error("Exception occurred during process ZCL command: {}".format(str(ex)))
        assert False
    except Exception as ex:
        log.error("Exception occurred during processing input: {}".format(str(ex)))
        assert False

@pytest.mark.chipToolTest
def test_light_ctrl_chip_tool(device, network, chip_tools_dir):
    network_ssid = network[0]
    network_pass = network[1]
    
    ret = device.wait_for_output("SetupQRCode")
    assert ret != None and len(ret) > 1

    qr_code = ret[-1].split()[-1].strip("[]")
    device_details = dict(SetupPayload().ParseQrCode("VP:vendorpayload%{}".format(qr_code)).attributes)
    assert device_details != None and len(device_details) != 0

    process = subprocess.Popen(["./chip-tool", "pairing", "ble", network_ssid, network_pass, str(device_details["SetUpPINCode"]), str(device_details["Discriminator"])], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    device_connected = "ConnectDevice complete" in out
    assert device_connected
    device_provisioned = "Network Provisioning Success" in out
    assert device_provisioned

    # Check on command
    process = subprocess.Popen(["./chip-tool", "onoff", "on", "1"], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    assert "SendMessage returned No Error" in out and "Data model processing success" in out

    ret = device.wait_for_output("Level Action has been completed")
    assert ret != None and len(ret) > 1

    # Check off command
    process = subprocess.Popen(["./chip-tool", "onoff", "off", "1"], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    assert "SendMessage returned No Error" in out and "Data model processing success" in out

    ret = device.wait_for_output("Level Action has been completed")
    assert ret != None and len(ret) > 1

    # Check toggle command
    process = subprocess.Popen(["./chip-tool", "onoff", "toggle", "1"], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    assert "SendMessage returned No Error" in out and "Data model processing success" in out

    ret = device.wait_for_output("Level Action has been completed")
    assert ret != None and len(ret) > 1

    # Check move to level command
    process = subprocess.Popen(["./chip-tool", "levelcontrol", "move-to-level", "200", "1", "0", "0", "1"], cwd=chip_tools_dir, stdout=subprocess.PIPE, universal_newlines=True)
    out = process.stdout.read()
    process.wait()
    assert process.returncode == 0
    assert "SendMessage returned No Error" in out and "Data model processing success" in out

    ret = device.wait_for_output("Level Action has been completed")
    assert ret != None and len(ret) > 1

@pytest.mark.pigweedRpcTest
def test_ligth_ctrl_rpc(device):
    pw_client = PigweedClient(device, RPC_PROTOS)
    
    # Check light on
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Set(on=True)
    assert status.ok() == True
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Get()
    assert status.ok() == True
    assert payload.on == True

     # Check light off
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Set(on=False)
    assert status.ok() == True
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Get()
    assert status.ok() == True
    assert payload.on == False

@pytest.mark.pigweedRpcTest
def test_button_ctrl_rpc(device):
    pw_client = PigweedClient(device, RPC_PROTOS)

    # Check button 0 (lighting)
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Get()
    assert status.ok() == True
    initial_state = bool(payload.on)

    compare_state = not initial_state
    status, payload = pw_client.rpcs.chip.rpc.Button.Event(idx=0, pushed=True)
    assert status.ok() == True
    sleep(0.5)
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Get()
    assert status.ok() == True
    assert payload.on == compare_state

    compare_state = initial_state
    status, payload = pw_client.rpcs.chip.rpc.Button.Event(idx=0, pushed=True)
    assert status.ok() == True
    sleep(0.5)
    status, payload = pw_client.rpcs.chip.rpc.Lighting.Get()
    assert status.ok() == True
    assert payload.on == compare_state

    # Check button 1 (lighting)
    # Trigger factory reset
    status, payload = pw_client.rpcs.chip.rpc.Button.Event(idx=1, pushed=True)
    assert status.ok() == True