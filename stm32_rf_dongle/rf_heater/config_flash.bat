REM $env:Path += ";C:\Users\Wass\.platformio\packages\tool-stlink"
SET PATH=%PATH%;C:\Users\Wass\.platformio\packages\tool-stlink
REM 0x0800FC00 is the adress of the last block 0..63
st-flash write .\BUILD\BLUEPILL_F103C8\GCC_ARM\config.bin 0x0800FC00
