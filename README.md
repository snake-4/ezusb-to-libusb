<h3 align="center">EZ-USB to libusb Compatibility Layer</h3>

  <p align="center">
    A library that hooks DeviceIoControl calls and redirects the legacy EZ-USB API to libusb.
    <br />
    <br />
    <a href="https://github.com/snake-4/EZUSBToLibusb/issues">Report Bug</a>
    ·
    <a href="https://github.com/snake-4/EZUSBToLibusb/issues">Request Feature</a>
    ·
    <a href="https://github.com/snake-4/EZUSBToLibusb/releases">Latest Release</a>
  </p>
</div>


<!-- ABOUT THE PROJECT -->
## About The Project

This library hooks various WinAPI functions in an attempt to emulate the EZ-USB driver on modern systems.

<!-- USAGE  -->
## Usage

### For Xeltek SuperPro:
* These releases are built with `SUPERPRO_WINMM_BUILD` defined.

1. Ensure that the `SP3000.exe` file is the original version. The hex edited version won't work.
1. Uninstall any existing driver for the SuperPro device. To do this, open the Device Manager, locate the SuperPro device, right-click on it, select "Uninstall," and make sure to check the checkbox that says "Attempt to remove the driver for this device."
1. Install the WinUSB driver for the SuperPro device. You can use <a href="https://zadig.akeo.ie/">Zadig</a>, which provides a usage guide available at <a href="https://github.com/pbatard/libwdi/wiki/Zadig#user-content-Basic_usage">this link</a>.
1. Download the <a href="https://github.com/snake-4/EZUSBToLibusb/releases">latest SuperPro release</a>, and extract the files from the zip to the folder where the `SP3000.exe` is located.
1. Update the VID and PID values in the `eu2lu_config.ini` file. To do that, follow these steps: Open Device Manager, locate the SuperPro device, right-click and select "Properties," go to the "Details" tab, choose "Hardware Ids" from the drop-down menu, note the VID and PID values, and update them in the `eu2lu_config.ini` file.

### For other devices:

1. Install the WinUSB driver for your  device. You can use <a href="https://zadig.akeo.ie/">Zadig</a>, which provides a usage guide available at <a href="https://github.com/pbatard/libwdi/wiki/Zadig#user-content-Basic_usage">this link</a>.
1. Download the <a href="https://github.com/snake-4/EZUSBToLibusb/releases">latest generic release</a>.
1. Modify the `eu2lu_config.ini` file to reflect the correct VID and PID values for your device. After making the modifications, move the file to the folder where your target software is located.
1. Inject the `EZUSBToLibusb.dll` file into your target software. The process for injecting the DLL may vary depending on the specific software you are using.

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/FeatureName`)
3. Commit your Changes (`git commit -m 'Add some FeatureName'`)
4. Push to the Branch (`git push origin feature/FeatureName`)
5. Open a Pull Request


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.