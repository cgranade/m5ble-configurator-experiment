# Running the server

```dotnetcli
cd blazor/Configurator
dotnet run --pathbase=/m5ble-configurator-experiment
```

# Resources

- https://docs.microsoft.com/events/dotnetconf-2021/connecting-gadgets-to-blazor
- https://duino4projects.com/m5paper-weather-station/
- https://github.com/naoki-sawada/m5stack-ble/blob/master/m5stack-ble/m5stack-ble.ino.
- https://github.com/notisrac/FileToCArray --- useful for hardcoding images.

# Pitfalls

- ArduinoBLE is not ESP32 compatible, but the ESP32 board support library for Arduino includes a _different_ BLE library built-in.
- Need to have `delay(1)` to allow BLE tasks to run: https://github.com/espressif/arduino-esp32/issues/3871
