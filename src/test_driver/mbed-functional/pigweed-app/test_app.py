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
from time import sleep

from echo_service import echo_service_pb2

from common.pigweed_client import PigweedClient

RPC_PROTOS = [echo_service_pb2]
PW_ECHO_TEST_MESSAGE = "Test_message"

@pytest.mark.smoketest
def test_smoke_test(device):
    device.reset(duration=1)
    ret = device.wait_for_output("chip-mbed-pigweed-example")
    assert ret != None and len(ret) > 1

@pytest.mark.pigweedRpcTest
def test_echo(device):
    pw_client = PigweedClient(device, RPC_PROTOS)
    status, payload = pw_client.rpcs.chip.rpc.Echo.Send(msg=PW_ECHO_TEST_MESSAGE)
    assert status.ok() == True
    assert payload.msg == PW_ECHO_TEST_MESSAGE
