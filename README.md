# analog3
Analog3 is a software/hardware hybrid synthesizer.

### What is this project?
This is a software-controlled analog synthesizer project.

One of the advantages of an analog synthesizer is smoothness of its voice.
Sound of analog VCO is so beautiful and has its taste that is hard to be reproduced by digital/software oscillators.

However, it has significant disadvantage when you want to put complex modulations on the sound.
For example, typical envelope generator of analog synthesizer gives too simple shape to create
musically attracitive sound, especially with attack part.

Digital/software control works much better for such complex control.
Analog3 is an effort to take advantage of both analog and digital synthesizers.
Signal flow to make sound is built by analog circuit modules, i.e., vco, vcf, vca.
Modulations are created digitally by embedded devices.  I chose PSoC for the first platform to implement modulators.

Another problem in handling analog synthesizers is complexity.
You need significantly large number of control parameters in order to control sound precisely.
Typical analog synthesizer has one knob per control parameter on its panel.
Analog3 is aiming to have hundreds of control parameters.
Panel size becomes too large to put such many knobs, and control them during performance is almost impossible.
So this project has another effort to virtualize panels.
The software panel reads modulator devices and draw them as modules on panel.  Number of shown control parameters would be reduced by hiding and/or grouping.

This repository is the software of Analog3 project.  It consists of
- frontend: Software panel and text console
- netsynth: Modulator device manager that communicates with both frontend and modulators
- psoc: Modulators
- protobuf: Protocol buffers schemas to define frontend-netsynth and netsynth-modulators communications

### Build instruction
#### Prerequisites
Frontend is a Java project and can be built on any platform that supports Java.

Netsynth has hardware depenency on Raspberry Pi B+.  Tested only on Raspbian.

PSoC modulators requires PSoC Creator version 3.1 and higher.

Following software tools need to be installed:
- Apache Maven
- Protocol Buffers

This project depends on following third party software components.
- rapidjson https://github.com/miloyip/rapidjson
- log4cplus https://github.com/log4cplus/log4cplus
- Nanopb http://koti.kapsi.fi/jpa/nanopb/

##### rapidjson
Rapidjson consists of C/C++ include files.
Place include/rapidjson directory in the package at $PROJECT_ROOT/thirdparty/include/rapidjson.

##### log4cplus
Install the log4cplus include files and libraries at $PROJECT_ROOT/thirdparty.

##### Nanopb
Expand the Nanopb package at $PROJECT_ROOT.  Rename the directory to nanopb.

#### frontend
This is a Java project.  Go to directory frontend and run 'mvn install'.

#### netsynth
This is a C++ project.  Go to directory netsynth and run 'make'.

#### psoc
Open projects from PSoC Creator and build.
