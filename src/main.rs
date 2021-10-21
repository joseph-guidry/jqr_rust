#[link(name="wordlist", kind="static")]
extern {
    fn testcall_float(v: f32);
}

use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;

fn main() {
    println!("Hello, from Rust!");

    unsafe {
        testcall_float(3.14159);
    };


    let listener = TcpListener::bind("127.0.0.1:8000").unwrap();

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!("Connection established!");

        handle_connection(stream);
    }
}

fn handle_connection(mut stream: TcpStream) {

    let mut buffer = [0; 1024];

    stream.read(&mut buffer).unwrap();

    println!("Request: {}", String::from_utf8_lossy(&buffer[..]));

    let response = "HTTP/1.1 200 OK\r\n\r\n";

    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}