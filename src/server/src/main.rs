mod lib;
use lib::data_handlers::*;

use serde_json::{Value, json};
use local_ipaddress;

use std::io::{Write, LineWriter};
use std::net::TcpListener;
use std::time::Duration;
use std::{thread};

fn json_fmt(data_struct:&mut Data) -> Vec<u8> {

    let data:Value = json!({
        "cpu_usage":data_struct.cpu_usage,
        "memory_usage":data_struct.memory_usage,
        "memory_percent":data_struct.memory_percent,
        "temp":data_struct.temp
    });

    let preform = serde_json::to_vec(&data).unwrap();
    preform
}

pub fn stream_init(){

    /*
    network() for external device
    local() for local device
    */

    let mut address = local_ipaddress::get().unwrap(); 
    println!("Local IP: {}", address);
    address.push_str(":8080");

    let listener = TcpListener::bind(address).unwrap();

    for stream in listener.incoming() {
        thread::spawn(|| {
            let stream = stream.unwrap();
            let mut data_handle = handler_init::init();
            let mut writer = LineWriter::new(stream);
            loop{                
                let mut a = data_fmt(&mut data_handle);
                let mut b:Vec<u8> = json_fmt(&mut a);
                b.push(10);

                writer.write(&mut b).unwrap();
                thread::sleep(Duration::from_secs(1));
            }
        });
    }
}

fn main() {
    println!("Started Server...");
    stream_init();
}
