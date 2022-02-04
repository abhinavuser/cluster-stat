# Surveillant Server
___

client runtime for monitoring stats using surveillant dashboard

**build instructions:**
```sh
cargo build --release
```

**install with respective systemd service:**
```sh
cp target/release/surveillant-server $HOME
cp surveillant.service /etc/systemd/system/
cp surveillant.sh /etc/systemd/system/
chmod +x /etc/systemd/system/surveillant.sh
systemctl enable surveillant
systemctl start surveillant
```
this should enable and run the server and bring it up online when starting up the server

check status logs of connections and disconnections with the server: 
```sh
systemctl status surveillant
```
### adding custom data collection services, sensor reading
***TBD***
