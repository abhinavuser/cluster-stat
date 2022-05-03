# Surveillant Server
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

**Want to create your own custom client server ?**

* the client server is very easy to port to other languages, 
  
* template for sending data is in the form of serialised json that is sent every 1 second (refer the provided server code for example), sent on TCP/IP port `8080` (this can be changed, but will require it be changed in the source of the gui app as well)

i.e
in `/src/main.rs` in the server

```rust
31
32    let mut address = local_ipaddress::get().unwrap(); 
33    println!("Local IP: {}", address);
> 34    address.push_str(":8080"); // change 8080 here to change port
35
36    let listener = TcpListener::bind(address).unwrap();
```

and 
in `/src/services/socket_service/init.hpp` of the gui app

```c++
15 
16 #define PORT 8080 // change 8080 here to change port
17
```

struct format:
|Data Name|Data Type|note|
|:-------:|:-------:|:--:|
|cpu_usage|float|average cpu usage data, 0.0 - 100.0|
|memory_usage|integer|memory usage in MB|
|memory_percent|float|memory usage in percentage 0.0 - 100.0|
|temp|integer|temprature in celcius|

example of json sent through the TCP/IP socket:
```js
{
    cpu_usage=4.9;
    memory_usage=1234;
    memory_percent=0.316897791;
    temp=49;
}
```