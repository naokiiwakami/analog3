# analog3
Analog3 is a software/hardware hybrid synthesizer system.

## About This Project
This is a software-controlled analog synthesizer project.

One of the advantages of an analog synthesizer is smoothness of its voice.
Sound of analog VCO is so beautiful and has its taste that is hard to be reproduced by digital/software oscillators.

However, it has significant disadvantage when you want to put complex modulations.
For example, typical envelope generator of analog synthesizer gives too simple shape to create musically-attractive sound, especially at the attack part - it usually has only
four parameters.

Digital/software control works much better for such complex control.
Analog3 is an effort to take advantage of both analog and digital synthesis.
Signal flow to make sound goes analog circuit paths, e.g., VCO, VCF, VCA.
Modulations can be made digitally or in conventionally analog.

An Analog3 system has a CAN bus and each module connects to the bus to exchange
information such as

  - configuration
  - performance data (similar to MIDI messages)
  - control values (namely digital CV)

The network has a special module called "[Mission Control](https://github.com/naokiiwakami/analog3.mission-control)". It organizes modules such as identifying them and assigning
IDs, helps connecting individual modules. It also provides user interface to help operating
the system.

Analog3 also aims to solve another issue of analog synthesizers that their panels would
~become too complicated if you increase number of parameters. An Analog3 module can have more
parameters than ones provided on the panel. These hidden parameters can be tweaked
externally. Also, multiple parameters can be associated to a knob to simplify the
manipulation.

## About This Repository

This repository maintains the common part of the Analog3 project that includes:

- [Module communication protocol specification](docs/analog3-spec.md)
- Software library (TBD)

## Related Projects

- [Mission Control](https://github.com/naokiiwakami/analog3.mission-control) - Manages modules
- [Can Controller](https://github.com/naokiiwakami/can-controller) - C library to enable
CAN interface in various micro controllers / Raspberry Pi
- [CV-Depot](https://github.com/naokiiwakami/cv-depot) - MIDI/CV converter with Analog3 interface