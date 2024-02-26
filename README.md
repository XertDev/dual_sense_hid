# Unofficial Dual Sense Support (WIP)
[![Ubuntu](https://github.com/XertDev/dual_sense/actions/workflows/ubuntu.yml/badge.svg?branch=master)](https://github.com/XertDev/dual_sense/actions/workflows/ubuntu.yml)
[![Nix build](https://github.com/XertDev/dual_sense_hid/actions/workflows/nix.yml/badge.svg)](https://github.com/XertDev/dual_sense_hid/actions/workflows/nix.yml)
<a target="_blank" href="LICENSE" title="License: MIT"><img src="https://img.shields.io/badge/License-MIT-blue.svg"></a>
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://xertdev.github.io/dual_sense/)
<br>

## Functionality 
### Reading from pad
Support for reading state from DualSense pad written in c++. 
#### Example
```c++
    const auto enumerated = dual_sense::enumerate();
    const dual_sense::Gamepad gamepad(enumerated.front());
    const auto state = gamepad.poll();
	
    std::cout << "Charging: " << std::boolalpha << state.battery.charging << std::endl;
```
### Pushing state to pad
Support for manipulation of lights settings on pad.

#### Example
```c++
    const auto enumerated = dual_sense::enumerate();
    const dual_sense::Gamepad gamepad(enumerated.front());
	
	auto& lights = gamepad.lights();
    lights.set_player_indicator(dual_sense_hid::Gamepad::Lights::PlayerIndicator::PLAYER_FOUR);
	gamepad.push_state();
```

## License
MIT © Xert