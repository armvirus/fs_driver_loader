# fs_driver_loader
Load driver on boot before anti-cheats by creating service entry as FSDriver

## Procedure
1. The loader copies the targetted driver so system32\drivers
2. Service entry is created in the registry
3. 
## Example Usage
![Load/Unload Example](./example.jpg)

Note: this project requres your driver to be signed obviously<br />

## Usage

```fs_driver_loader.exe driver.sys -load/-unload```
