###
This project is foucus on Pebble health API using bma423 Accelerameter, you could see more Pebble health API via
https://developer.rebble.io/developer.pebble.com/docs/c/Foundation/Event_Service/HealthService/index.html

### Set up the development environment

Follow Zephyr's [Getting Started
Guide](https://docs.zephyrproject.org/latest/getting_started/index.html) up to
step 3.2 "Get the Zephyr source code". Here you should run the commands below
instead of the ones in the guide:

```
$ git clone https://gitee.com/fwatch/twatch
$ cd twatch
$ west init -l app/
$ west update # update modules
$ west espressif update # update submodules
```

Then complete the remaining steps under section 3 and 4. Finally, run `make
tools`.

### Build and flash twatch

```
west build -p auto -b twatch_v3 -s app
```

```
west flash
```
