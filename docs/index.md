## Surveillant Docs

**Clone the project:**
```
git clone https://github.com/zephyrlabs/surveillant.git
```

**build instructions:**
```sh
cd src
cmake -S ./ -B ./build
cd build && make -j
```

**running target:**
```sh
./surveillant
```

**blinking linux fbcon cursor can be disabled with:**
```sh
echo 0 > /sys/class/graphics/fbcon/cursor_blink
```

**surveillant can be integrated with the system by creating a systemd service.**

```sh
cp surveillant $HOME/
cp ../theme.json $HOME/
cp ../config.txt $HOME/
cp ../startup.sh /etc/systemd/system/
cp ../startup.service /etc/systemd/system/
chmod +x  /etc/systemd/system/startup.sh
systemctl enable startup
```

**application can be started with:**
```sh
systemctl start startup
```

**NOTE:** 
* in the startup.sh file as well as in the systemd setup commands,
`$HOME` in the first line must be replaced with the home dir of the user, 
eg. `/home/foo` 


### Configuring Surveillant:

* replace the default targets to be connect to in `config.txt`.
values must be space seperated pairs of hostname and ipv4 address,
no return line at the end of the config, it is very sensitive and can return invalid address error during runtime.

* `theme.json` is the theming config file, it should always have default 
values and must be present during runtime.
    * `BG COLOR` background color
    * `FG COLOR` foreground color of chart backgrounds ref. styling.cpp
    * `PRIMARY_LINE_COLOR` (unused as of now)
    * `SECONDARY_LINE_COLOR` (used as the background color of arcs)

## Setting up surveillant server on client devices:
ref to [surveillant server docs](../src/server/README.md)
