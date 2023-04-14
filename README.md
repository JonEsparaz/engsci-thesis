# Transforming Water Quality Sampling in Drinking Water Pipes via a Large Volume, Event-Based Sampling Device

Arduino source code, LabView VI, and CAD files for B.A.Sc. Thesis by Jonathan Esparaz (Engineering Science 2T2 + PEY, Machine Intelligence Major, University of Toronto).

## Project structure

The `ino` directory contains a single Arduino sketch that controls a prototype large volume, event-based sampling device. Associated wiring diagram:

![arduino diagram](https://user-images.githubusercontent.com/48423418/232114150-ef6d8893-402c-4e32-8aa6-a9a03bdf2a01.png)

The solenoid valve requires an external 12V power supply, 220 ohm resistor, a TIP102 (NPN) transistor, and a IN4148 diode.

The `labview` directory contains a single LabView Virtual Instrument (VI) that records data from two [PX309-V150G5V pressure transducers](https://www.omega.ca/en/pressure-measurement/pressure-transducers/px309/p/PX309-V150G5V), one [G2S07N42GMX flow meter](https://www.omega.ca/en/flow-instruments/flow-meters/turbine-flow-meters/g2-series-gp/p/G2S07N42GMX), the output voltage from the Arduino used to control the solenoid valve (connection not represented in Arduino wiring diagram), and the output voltage from the Arduino turbidity sensor (connection not represented in Arduino wiring diagram). Data is saved to an Excel spreadsheet. Associated wiring diagram:

![circuit](https://user-images.githubusercontent.com/48423418/232114163-6df15a15-821d-4fbf-a655-652858184ab7.png)

The flow meter requires an external 12V power supply and 250 ohm resistor.

The `cad` directory contains files that specify a custom adaptor for fitting the Arduino turbidity sensor in 1-inch PVC pipes (~60 PSI system pressure).

## Credits

Thank you to my thesis supervisors: Dr. David Meyer (Department of Civil & Mineral Engineering, University of Toronto) and Dr. Aaron Bivins (Department of Civil & Environmental Engineering, Louisiana State University). The LabView VI is based on example code provided by Dr. Meyer.
