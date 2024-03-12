PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_usb_pcdc
=======================

Document Number 
---------------
R01AN2030EJ
R01AN2030JJ

Version
-------
v1.31


Overview
--------
USB Peripheral Communications Device Class Driver (PCDC)

Features
--------
The PCDC conforms to the abstract control model of the communication
device class specification (CDC) and enables communication with a USB Host.


Supported MCUs
--------------
* RX64M Group
* RX71M Group
* RX65N Group
* RX66T Group
* RX72T Group
* RX72M Group
* RX72N Group
* RX66N Group
* RX671 Group


Boards Tested On
----------------
* RSKRX64M
* RSKRX71M
* RSKRX65N
* RSKRX65N_2MB
* RSKRX72T
* RSKRX72M
* RSKRX72N
* RSKRX671


Limitations
-----------

Peripherals Used Directly
-------------------------


Required Packages
-----------------
* r_bsp
* r_usb_basic

How to add to your project
--------------------------

Toolchain(s) Used
-----------------
* Renesas RX v.3.03.00
* GCC for Renesas RX 4.08.04.201902
* IAR C/C++ Compiler for Renesas version 4.12.01

File Structure
--------------
r_usb_pcdc
|   readme.txt
|   r_usb_pcdc_if.h
|
+---doc
|     \en
|     |   r01an2030ej0131_usb.pdf
|     \jp
|         r01an2030jj0131_usb.pdf
|
+---ref
|       r_usb_pcdc_config_reference.h
|
+---utilities
|       CDC_Demo.inf
|
\---src
     |  r_usb_pcdc_driver.c
     |
     \---inc
             r_usb_pcdc.h

