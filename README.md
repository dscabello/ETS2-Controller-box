# ETS2 Controller Box


## Overview
The ETS2 Controller Box is a feedback software designed for Euro Truck Simulator 2 (ETS2). It enhances your gaming experience by providing real-time telemetry data and customizable feedback options. Whether you're a casual player or a simulation enthusiast, this tool helps you monitor your in-game performance and improve your driving skills.

## Features
- Real-time Telemetry: Access live data from your ETS2 game, including truck performance metrics.
- Support for Multiple Inputs: Use various input methods for controlling and receiving feedback, such as buttons, sliders, and dials.

## Getting Started
### Prerequisites

Before you begin, ensure you have met the following requirements:

- Hardware: A compatible device running ETS2.
Software:
- ESP-IDF (for ESP32 development)
- Dependencies as mentioned in the requirements.txt (ESP-IDF).

## Installation

### Clone the repository:

bash
Copy code
git clone [ETS2-Controller-box](https://github.com/dscabello/ETS2-Controller-box).git
cd ETS2-Controller-box

Set up the [ESP-IDF](https://github.com/espressif/esp-idf) environment: Follow the official ESP-IDF Getting Started guide to set up the development environment.

### Build the project:
```
bash
Copy code
idf.py build
```

### Flash the firmware to your ESP32:
```
bash
Copy code
idf.py -p <PORT> flash
```

### Monitor the output:
```
bash
Copy code
idf.py monitor
```

### Usage

Launch the ETS2 Controller Box on your device.
Connect it to your ETS2 game using the configured network settings.
Customize the dashboard according to your preferences.

### Contributing
We welcome contributions! To contribute, please fork the repository, create a new branch, and submit a pull request with your changes.

Fork the project.
Create your feature branch:
bash
Copy code
git checkout -b feature/AmazingFeature
Commit your changes:
bash
Copy code
git commit -m 'Add some amazing feature'
Push to the branch:
bash
Copy code
git push origin feature/AmazingFeature
Open a pull request.

## Folder Contents

Below is short explanation of remaining files in the project folder.
```
├── CMakeLists.txt
├── doc
│   ├── Documentation.md
│   └── Pictures
├── .gitignore
├── LICENSE
├── main
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── ets2ctrl.h
│   │   ├── httpHandler.h
│   │   ├── tasksHandler.h
│   │   └── wifiHandler.h
│   ├── Kconfig.projbuild
│   ├── main.c
│   └── src
│       ├── ets2ctrl.c
│       ├── httpHandler.c
│       ├── tasksHandler.c
│       └── wifiHandler.c
├── README.md
├── sdkconfig
├── sdkconfig.old
└── .vscode
    └── settings.json

```
## License

- Attribution-NonCommercial-ShareAlike 4.0 International [LICENSE](https://github.com/dscabello/ETS2-Controller-box/blob/main/LICENSE])

