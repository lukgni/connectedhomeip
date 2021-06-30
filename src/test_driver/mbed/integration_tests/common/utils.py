# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import os
import platform
import random
from ipaddress import ip_address, IPv4Address

from chip import exceptions

if platform.system() == 'Darwin':
    from chip.ChipCoreBluetoothMgr import CoreBluetoothManager as BleManager
elif sys.platform.startswith('linux'):
    from chip.ChipBluezMgr import BluezManager as BleManager

import logging
log = logging.getLogger(__name__)

NETWORK_SCAN_RETRIES=5

def validIPAddress(IP: str) -> str:
    """
    Check if string is IP address
    :param IP: IP string
    :return: "Invalid" if string in not IP address definition, otherwise "IPv4" or "IPv6"
    """
    try:
        return "IPv4" if type(ip_address(IP)) is IPv4Address else "IPv6"
    except ValueError:
        return "Invalid"

def is_network_visible(net_list, net_ssid):
    """
    Check if network is on the list
    :param net_list: network list
    :param net_ssid: network ssid we are looking for
    :return: True if netowrk exists on the list, otherwise False
    """
    try:
    for line in net_list:
        if "Network:" in line and net_ssid in line:
            return True
    return False

def scan_chip_ble_devices(devCtrl):
    """
    BLE scan CHIP device
    BLE scanning for 10 seconds and collect the results 
    :param devCtrl: device controller instance
    :return: List of visible BLE devices
    """
    devices = []
    bleMgr = BleManager(devCtrl)
    bleMgr.scan("-t 10")

    for device in bleMgr.peripheral_list:
        devIdInfo = bleMgr.get_peripheral_devIdInfo(device)
        if devIdInfo:
            log.info("Found CHIP device {}".format(device.Name))
            devices.append(devIdInfo)

    return devices

def run_wifi_provisioning(devCtrl, ssid, password, discriminator, pinCode, nodeId=None):
    """
    Run WiFi provisionning via BLE 
    :param devCtrl: device controller instance
    :param ssid: network ssid
    :param password: network password
    :param discriminator: CHIP device discriminator
    :param pinCode: CHIP device pin code
    :param nodeId: default value of node ID
    :return: node ID is provisioning  successful, otherwise None
    """
    if nodeId == None:
        nodeId = random.randint(1, 1000000)

    try:
        devCtrl.SetWifiCredential(ssid, password)
        devCtrl.ConnectBLE(int(discriminator), int(pinCode), int(nodeId))
    except exceptions.ChipStackException as ex:
        log.error("WiFi provisioning failed: {}".format(str(ex)))
        return None

    return nodeId
