# WoT-microcontroller-servient-gui

> WoT-microcontroller-servient is an implementation of W3C Web of Things Servient for embedded systems. This module allows users to: define W3C WoT Thing Descriptions, generate scripting files (sketches) in Embedded-C programming language executable by microcontrollers to expose Things, compile and flash these sketches.

## How does it work?

This module provides an Electron-made GUI. The GUI uses the [WoT-microcontroller-servient project](https://github.com/UniBO-PRISMLab/WoT-microcontroller-servient). User can insert all the metadata included in W3C WoT Thing Descriptions and store them in a JSON Thing Description file. 
The project can also build a scripting file including the Thing logic. This scripting file is executed by microcontrollers and it has the .ino extension.
This project can also compile the built .ino project and upload to the prototipying board, calling the WoTServient CLI and showing it to a built-in terminal.

The **only supported** embedded systems are NodeMCU ESP8266 and ESP32. 

## Prerequisites
This package is only supported by Linux systems.

**Linux**
- Python v3.6.x
- Node v8.10.0 (tested and working also with 9.11.2 and 10.21.0)
- npm (tested with version 3.5.x)
- pip3 Python package manager
- A proper C/C++ compiler toolchain, like GCC

## Usage
### Clone and install
Clone the repository:

```shell
git clone https://github.com/Chello/WoT-microcontroller-servient-GUI.git
```

Go into the repository:

```shell
cd WoT-microcontroller-servient-GUI
```
Install the package and all dependencies:

```shell
pip install .
npm install
```

It is **recomended** to install and use the package inside a virtual environment.

### Start application

The application can be started with the following command:

```shell
npm start
```

### Implemented/supported features

**Scripting API**

- RESTful API :heavy_check_mark:

**Protocol Support**

- HTTP :heavy_check_mark:
- WebSocket :heavy_check_mark:

**MediaType Support**

- JSON :heavy_check_mark:

## License
WoT-microcontroller-servient-GUI is released under the MIT License.


