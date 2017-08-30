stmm-input-gtk-bt
=================

Keyboard like devices for stmm-input framework connected over bluetooth.

This source package contains:

- bluetoother:
    GUI tool to fine-tune bluetooth adpter connectivity for systemd and bluez.

- libstmm-input-gtk-bt:
    library that implements a device manager that integrates into Gtk's
    main event loop sending key events from bluetooth devices to listeners.
    The device manager starts a bluetooth server that accepts connections
    from clients on a specific L2CAP port following a very simple protocol.
    The library can be stand-alone or loaded as a plugin by stmm-input-dl.

- stmm-input-bttest:
    test client that connects to the server defined in libstmm-input-gtk-bt.
    It simulates a very trivial keyboard.


Read the INSTALL file for installation instructions.

An example can be found in the libstmm-input-gtk-bt subfolder.


Warning
-------
The APIs of the library isn't stable yet.
