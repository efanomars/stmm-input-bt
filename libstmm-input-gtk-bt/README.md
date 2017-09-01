stmm-input-gtk-bt                                                  {#mainpage}
=================

Implementation of the stmm-input library for Gtkmm and Bluetooth devices.

The device manager provided by this library sends key events defined in the
stmm-input-ev library to listeners.
It optionally can be used as a plugin (of stmm-input-dl).

The device manager starts a server that accepts bluetooth client connections
following a simple protocol shown below.
To test the device manager stmm-input-bttest can be used (on another computer).

The device manager attaches itself to the Gtk event loop.

Once a client is connected (which reflects in the creation of a device in the
device manager) it is not removed even if the connection is dropped. The device
is considered attached with a virtual cable to the server. To actually remove the
device from the manager, a remove command has to be sent by the client itself 
(see protocol below) or the device manager has to be deleted.
This allows for client devices to momentarily go out of range (i.ex. during
a game) without having to reconfigure all its keys.

Example servers supporting this device manager can be found in the
examples/keys-srv folder or in the stmm-input source package.


Setting up bluetooth
--------------------

To fine tune your bluetooth adapter you can use the bluetoother GUI application.

Alternatively you can use the available command line tools of your distribution
as follows.

First: make sure there is bluetooth hardware on your pc with

    $> /usr/sbin/rfkill list bluetooth

which should output something like

    1: hci0: Bluetooth
           Soft blocked: no
           Hard blocked: no

If it's hard blocked you need to unblock it in your BIOS settings.
If it's soft blocked use rfkill itself to unblock it (man rfkill).

Second: make sure the bluetooth service is running

    $> ps -A | grep bluetoothd

should output something like

    4636 ?        00:00:00 bluetoothd

if it doesn't, call

    $>sudo systemctl start bluetooth.service

if you want it automatically started at startup

    $>sudo systemctl enable bluetooth.service


Sometimes bluetooth discoverability is limited by the linux distribution.
If bluetoothd is running but other devices can't see it, call
the following command (for example if the chip is on hci0, see above)

    $sudo hciconfig hci0 piscan


The mentioned commands can be found in (Debian Stretch, Linux Mint 18) packages:

    rfkill, bluez



The btkeys protocol
-------------------

Currently the server listens for connections on L2CAP port 8353 (0x20A1).
In the future, the server might listen to a random free port and publish it
through SDP (bluetooth's Service Discovery Protocol) so that more than one
server can run simultaneously on a computer.

Once a client successfully connects, it starts to send packets of the format

    struct KeyPacket
    {
        char m_nMagic1; // = '7'
        char m_nMagic2; // = 'A'
        char m_nCmd;            // PACKET_CMD
        char m_nKeyType;        // stmi::KeyEvent::KEY_INPUT_TYPE
        int32_t m_nHardwareKey; // stmi::HARDWARE_KEY
    };

The possible commands are

    PACKET_CMD_KEY = 0   // the packet contains a valid key and type
    PACKET_CMD_NOOP = 1  // used to check if the connection still alive
    PACKET_CMD_REMOVE_DEVICE = 3  // the packet signals the server to remove
                                  // the device and close the connection.

The values allowed for fields m_nKeyType and m_nHardwareKey are defined in the 
libstmm-input-ev and libstmm-input libraries respectively.



Warning
-------
The API of this library isn't stable yet.
