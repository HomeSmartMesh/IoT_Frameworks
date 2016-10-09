# Smart IO Expander EEPROM and Serial console Control
## EEPROM Sequencing
- reads commands from EEPROM and executes them on the output with delay
##Console Control
- reads command line and sets outputs accordingly
- "P" then return : Stops the EEPROM sequencing execution
- "S" then return : Starts the EEPROM sequencing execution
- "Se00" then return : sends hex 0x00 all outputs OFF
- "SeFF" then return : sends hex 0xFF all outputs ON
- "Se5A" then return : sends hex 0x5A outputs to binary 0101 1010 => OFF,ON,OFF,ON, ON,OFF,ON,OFF
