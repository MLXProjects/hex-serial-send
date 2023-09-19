# hex-serial-send
small & easy Linux utility to send bytes via serial, 2 at a time

## Usage
```./ser [device] \<hex bytes...>```  
first byte defaults to A5, can be replaced using 2-byte sequences  
examples:
- ```./ser /dev/ttyUSB0 A52F```
- ```./ser A52F```
- ```./ser 2F```
- ```./ser A52F A53F A54F```

## Build
```gcc ser.c -o ser```
