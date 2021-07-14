<p align="center">
  <img src="https://raw.githubusercontent.com/ARMmbed/mbed-os/master/logo.png" alt="ARM Mbed-OS logo"/>
</p>

# CHIP ARM-MbedOS Lock Example Application

The ARM-MbedOS Lock Example demonstrates how to remotely control a door lock
device with one basic bolt.
It uses buttons to test changing the lock and device states and LEDs to show 
the state of these changes.  
You can use this example as a reference for creating your own application.

The example behaves as a CHIP accessory, that is a device that can be paired 
into an existing CHIP network and can be controlled by this network.

<hr>

-   [CHIP ARM-MbedOS Lock Example Application](#main_title)
    -   [Overview](#Overview)
    -   [Bluetooth LE advertising](#Bluetooth-LE-advertising)
    -   [Bluetooth LE rendezvous](#Bluetooth-LE-rendezvous)
    -   [Wi-Fi provisioning](#Wi-Fi-provisioning)
    -   [Device UI](#Device-UI)

-   [Building and Flashing Example Application](#Building-and-Flashing-Example-Application)
    -   [Development environment setup](#Development-environment-setup)
    -   [Building](#Building)
    -   [Flashing](#Flashing)
    -   [Debugging](#Debugging)
    -   [Testing the example](#Testing-the-example)
    -   [Testing using CHIPTool](#Testing-using-CHIPTool)
    -   [Supported devices](#Supported-devices)
        -   [Notes](#Notes)

<hr>

## Overview

This example is running on the mbedos platform, which is based on 
[ARM Mbed-OS system](#https://os.mbed.com/mbed-os/).

The CHIP device that runs the lock application is controlled by the CHIP
controller device over the Wi-Fi network. Some actions required before establishing full
communication are described below.

## Bluetooth LE advertising

In this example, to commission the device onto a CHIP network, it must be
discoverable over Bluetooth LE. The Bluetooth LE advertising starts
automatically after device boot-up.

## Bluetooth LE rendezvous

In this example, the commissioning procedure (called rendezvous) is done over
Bluetooth LE between a CHIP device and the CHIP controller, where the controller
has the commissioner role.

To start the rendezvous, the controller must get the commissioning information
from the CHIP device. The data payload is encoded within a QR code, printed to
the UART console.

## Wi-Fi provisioning

Last part of the rendezvous procedure, the provisioning operation involves
sending the Wi-Fi network credentials from the CHIP controller to the CHIP
device. As a result, device is able to join the Wi-Fi network and communicate
with other CHIP devices in the network.

## Device UI

This example application has a simple User Interface to depict the state of the
Wi-Fi provisioning and door lock. Additionally it is possible to control the state
of lock manually by using available push-button. One of the user LEDs will be set
 on when the lock is locked, and will be set off when unlocked. That LEDs will flash 
when in the transition state between locked and unlocked.

Second user LED is used to indicate Wi-Fi provisioning status. The following states 
are possible:
-   _Short Flash On (50 ms on/950 ms off)_ &mdash; The device is in the
    unprovisioned (unpaired) state and is waiting for a commissioning
    application to connect.

-   _Rapid Even Flashing (100 ms on/100 ms off)_ &mdash; The device is in the
    unprovisioned state and a commissioning application is connected through
    Bluetooth LE.

-   _Short Flash Off (950ms on/50ms off)_ &mdash; The device is fully
    provisioned, but does not yet have full Wi-Fi network or service
    connectivity.

-   _Solid On_ &mdash; The device is fully provisioned and has full Wi-Fi
    network and service connectivity.

Some of the supported boards may not have sufficient number of LEDs or Push-Buttons to 
follow above description. In that case please refer to [Supported devices](#Supported-devices)
section and check board's 'Notes' column for additional information how that limitation
has been workarounded. 

# Building and Flashing Example Application

## Development environment setup    

Building the example application requires the use of **ARM Mbed-OS** sources 
and **arm-none-gnu-eabi** toolchain. The OpenOCD package is used for 
flashing and debugging purposes.
<br>
Some additional packages may be needed, depending on selected build target 
and its requirements.

> **The VSCode devcontainer has these components pre-installed. Using 
the VSCode devcontainer is the recommended way to interact with ARM Mbed-OS 
port of the CHIP Project.**   
>    
> **Please read this [README.md](../../..//docs/VSCODE_DEVELOPMENT.md) for 
more information about using VSCode in container.**

To initialize development environment, download all registered sub-modules 
and activate development environment:
```
$ source ./scripts/bootstrap.sh    
$ source ./scripts/activate.sh
```
If packages are already installed then simply just activate development 
environment:
```
$ source ./scripts/activate.sh
```
## Building

The Lock-app application can be built in the same way as any other CHIP 
example ported to mbed-os platform.
<br>
* **by using generic vscode task**:  
```
Command Palette (F1)  => Tasks: Run Task => Run Mbed application => build => (example name) => (board name) => (build profile)`
```  
<br>

* **by calling explicitly building script:**
```
$ ${CHIP_ROOT}/scripts/examples/mbed_example.sh -c build -a <example name> -b <board name> -p <build profile>
```
Both approaches are limited to supported evaluation boards which are 
listed in [Supported devices](#Supported-devices) paragraph. 
<br>When using building script, it is possible expand its list of acceptable 
targets which may be useful for rapid testing of a new mbed target.
<br><br>For more detailed description about porting mbed targets into the 
CHIP project, please read **[LINK HERE]**

## Flashing

The Lock-app application can be flashed in the same way as any other CHIP 
example ported to mbed-os platform.
<br>
* **by using generic vscode task**: 
```
Command Palette (F1)  => Tasks: Run Task -> Run Mbed application => flash => (example name) => (board name) => (build profile)
```
<br>

* **by calling explicitly building script:**
```
$ ${CHIP_ROOT}/scripts/examples/mbed_example.sh -c flash -a <example name> -b <board name> -p <build profile>
```

## Debugging
This example can be debugged by using prepared vscode launch task (it will manage gdb-server session by its own):
```
Run and Debug (Ctrl+Shift+D) => Debug Mbed (board name) => Start Debugging (F5)
```

It is possible to connect to external gdb-server session by using specific **'Debug Mbed (board name) remote'** task. 

## Testing the example

Check the [**LINK HERE**] to learn how to use command-line interface 
of the application.

## Testing using CHIPTool

Read the
[**LINK HERE**] to see how to use 
[CHIPTool](../../../src/android/CHIPTool/README.md) for
Android smartphones to commission and control the application 
within a Wi-Fi network.

## Supported devices

The example supports building and running on the following mbed-enabled devices:

| Manufacturer | Hardware platform | Build target | Platform image | Status | Notes |
| ------------ | ----------------- | ------------ | -------------- | :----: | ----- |
| [Cypress<br> Semiconductor](https://www.cypress.com/) | [CY8CPROTO-062-4343W](https://os.mbed.com/platforms/CY8CPROTO-062-4343W/) | `CY8CPROTO_062_4343W` | <details><summary>CY8CPROTO-062-4343W</summary><img src="https://os.mbed.com/media/cache/platforms/p6_wifi-bt_proto.png.250x250_q85.jpg" alt="CY8CPROTO-062-4343W"/></details> | :heavy_check_mark: |  <details><summary>LEDs and buttons limitation</summary><ul><li>Board has only one usable LED which is used to indicate provisioning status.<br>'Lock state' signal has been routed to PB9_6 pin (active high)</li><li>SW2 push-button is not used in this example due to its interation with WIFI module interrupt line.<br>User can change 'Lock State' manually by using BTN1 capacitive button</li></ul></details> |

There is a plan to expend above list with new platforms within mbed-os ecosystem.

#### Notes

* More details and guidelines about porting new hardware into the CHIP project with mbed-os can be found at **[LINK HERE]**
* Some useful information about HW platform specific settings can be found in `lock-app/mbed/mbed_app.json`.    
Information about this file syntax and its meaning in mbed-os project can be found here: [Mbed-Os configuration system](https://os.mbed.com/docs/mbed-os/v6.12/program-setup/advanced-configuration.html))