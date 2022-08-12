use std::io::{self, Write};
use std::process::Command;
fn main() {
    let test_args: Vec<String> = vec![
        "-l".to_string(),
        "/home".to_string(),
        "/home/ap".to_string(),
    ];

    let output = Command::new("/bin/ls")
        .args(&test_args)
        .output()
        .expect("failed to execute process");

    println!("status: {}", output.status);
    io::stdout().write_all(&output.stdout).unwrap();
    io::stderr().write_all(&output.stderr).unwrap();
}
