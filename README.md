## Unofficial Dual Sense Support (WIP)
<a target="_blank" href="LICENSE" title="License: MIT"><img src="https://img.shields.io/badge/License-MIT-blue.svg"></a>
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://xertdev.github.io/dual_sense/)
<br>
<span>Support for reading state from DualSense pad written in c++.</span>
### Example
```c++
    const auto enumerated = dual_sense::enumerate();
    const dual_sense::Gamepad gamepad(enumerated.front());
    const auto state = gamepad.poll();
	
    std::cout << "Charging: " << std::boolalpha << state.battery.charging << std::endl;
```

### License
MIT © Xert