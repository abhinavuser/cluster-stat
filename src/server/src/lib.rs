pub mod data_handlers {
    use std::io::{BufReader};
    use std::fs::File;
    
    pub struct Data{
        pub cpu_usage:f32,
        pub memory_usage:i32,
        pub memory_percent:f32,
        pub temp:i32
    }

    /*
    handler initialization module
    create read handles for file/stream or sensor data
    */
    pub mod handler_init {
        use std::io::{BufReader};
        use std::fs::File;

        // cpu handler: fetches cpu avg usage from linux proc
        pub fn cpu_handler() -> (File, BufReader<File>){
            let cpufile:File = File::open("/proc/loadavg").unwrap();
            let cpuf = cpufile.try_clone().unwrap();
            let cpureader:BufReader<File> = BufReader::new(cpufile);
            (cpuf, cpureader)
        }

        // mem handler: fetches mem usage from linux proc
        pub fn mem_handler() -> (File, BufReader<File>){
            let memfile:File = File::open("/proc/meminfo").unwrap();
            let memf = memfile.try_clone().unwrap();
            let memreader:BufReader<File> = BufReader::new(memfile);
            (memf, memreader)
        }

        // temp handler: fetches system operating temprature from sensors accessible from the linux kernel
        pub fn temp_handler() -> (File, BufReader<File>){
            let tempfile:File = File::open("/sys/class/thermal/thermal_zone0/temp").unwrap();
            let tempf = tempfile.try_clone().unwrap();
            let tempreader:BufReader<File> = BufReader::new(tempfile);
            (tempf, tempreader)
        }

        // initialization of handlers
        pub fn init() -> Vec<(File, BufReader<File>)>{
            let cpu = cpu_handler();
            let mem = mem_handler();
            let temp = temp_handler();
            vec!(cpu, mem, temp)

        }
    }

    /*
    data fetch module
    create custom functions to fetch data from any sensor
    or read data/file streams 
    */

    pub mod data_fetch {
        use std::io::{BufRead, BufReader, Seek, SeekFrom};
        use std::io::prelude::*;
        use std::fs::File;

        pub fn cpu_fetch(handler:&mut (File, BufReader<File>)) -> f32{
            let mut cpu_buf = String::new();
            handler.0.seek(SeekFrom::Start(0)).unwrap();
            handler.1.read_line(&mut cpu_buf).unwrap();

            let cpu_buf:Vec<&str> = cpu_buf.split(' ').collect();
            let cpu_buf:f32 = cpu_buf[0].trim().parse().unwrap();
            cpu_buf
        }

        pub fn mem_fetch(handler:&mut (File, BufReader<File>)) -> (i32, f32){
            let mut mem_buf = String::new();
            let _mem_buf_total = String::new();
            let _mem_buf_free = String::new();
            let _mem_buf_available = String::new();

            handler.0.seek(SeekFrom::Start(0)).unwrap();
            handler.0.read_to_string(&mut mem_buf).unwrap();

            let mem_buf:Vec<&str> = mem_buf.split('\n').collect();

            let mem_buf_total = mem_buf[0];
            let mem_buf_free = mem_buf[1];
            let mem_buf_available = mem_buf[2];

            let mem_buf_total:Vec<&str> = mem_buf_total.split(' ').rev().collect();
            let mem_buf_free:Vec<&str> = mem_buf_free.split(' ').rev().collect();
            let mem_buf_available:Vec<&str> = mem_buf_available.split(' ').rev().collect();

            let mem_total:i32 = mem_buf_total[1].trim().parse().unwrap(); 
            let _mem_free:i32 = mem_buf_free[1].trim().parse().unwrap();
            let mem_available:i32 = mem_buf_available[1].trim().parse().unwrap();
            let mem_used: i32 = mem_total - mem_available;
            let mem_percent:f32 = 100.0 * (mem_total as f32 - mem_available as f32)/ mem_total as f32;
            (mem_used, mem_percent)
        }

        pub fn temp_fetch(handler:&mut (File, BufReader<File>)) -> i32{
            let mut temp_buf = String::new();
            handler.0.seek(SeekFrom::Start(0)).unwrap();
            handler.1.read_line(&mut temp_buf).unwrap();

            let temp_buf:i32 = temp_buf.trim().parse().unwrap(); 
            temp_buf
        }
    }

    pub fn data_fmt(handles:&mut Vec<(File, BufReader<File>)>) -> Data {

        let cpu_data = data_fetch::cpu_fetch(&mut handles[0]);
        let mem_data = data_fetch::mem_fetch(&mut handles[1]);
        let temp_data = data_fetch::temp_fetch(&mut handles[2]);

        Data{cpu_usage: cpu_data, memory_usage:mem_data.0, memory_percent:mem_data.1, temp:temp_data}
    }
}
