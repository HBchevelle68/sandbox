// in `elk/src/main.rs`

use std::{env, error::Error, fs};

unsafe fn jmp(addr: *const u8) {
    let fn_ptr: fn() = std::mem::transmute(addr);
    fn_ptr();
}

fn main() -> Result<(), Box<dyn Error>> {
    let input_path = env::args().nth(1).expect("usage: elk FILE");
    let input: Vec<u8> = fs::read(&input_path)?;

    let file = match delf::File::parse_or_print_error(&input[..]) {
        Some(f) => f,
        None => std::process::exit(1),
    };
    println!("{:#?}", file);

    // println!("Executing {:?}...", input_path);
    // use std::process::Command;
    // let status = Command::new(input_path).status()?;
    // if !status.success() {
    //     return Err("process did not exit successfully".into());
    // }

    Ok(())
}
