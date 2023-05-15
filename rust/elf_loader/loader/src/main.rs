// in `elk/src/main.rs`

use std::{env, error::Error, fs};

fn ndisasm(code: &[u8], origin: delf::Addr) -> Result<(), Box<dyn Error>> {
    use std::{
        io::Write,
        process::{Command, Stdio},
    };

    let mut child = Command::new("ndisasm")
        .arg("-b")
        .arg("64")
        .arg("-o")
        .arg(format!("{}", origin.0))
        .arg("-")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .spawn()?;
    child.stdin.as_mut().unwrap().write_all(code)?;
    let output = child.wait_with_output()?;
    println!("{}", String::from_utf8_lossy(&output.stdout));

    Ok(())
}

// And this little helper function is new as well!
fn pause(reason: &str) -> Result<(), Box<dyn Error>> {
    println!("Press Enter to {}...", reason);
    {
        let mut s = String::new();
        std::io::stdin().read_line(&mut s)?;
    }
    Ok(())
}

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

    println!("Disassembling {:?}...", input_path);
    let code_ph = file
        .program_headers
        .iter()
        .find(|ph| ph.mem_range().contains(&file.entry_point))
        .expect("segment with entry point not found");

    ndisasm(&code_ph.data[..], file.entry_point)?;

    //**********************************************************************

    println!("Executing {:?} in memory...", input_path);

    use region::{protect, Protection};
    let code = &code_ph.data;
    pause("protect")?; // this is new
    unsafe {
        protect(code.as_ptr(), code.len(), Protection::ReadWriteExecute)?;
    }

    let entry_offset = file.entry_point - code_ph.vaddr;
    let entry_point = unsafe { code.as_ptr().add(entry_offset.into()) };
    println!("       code  @ {:?}", code.as_ptr());
    println!("entry offset @ {:?}", entry_offset);
    println!("entry point  @ {:?}", entry_point);

    pause("jmp")?; // that is also new

    unsafe {
        jmp(entry_point);
    }

    Ok(())
}
