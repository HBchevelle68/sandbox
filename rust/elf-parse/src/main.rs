use goblin::{error, Object};
use std::env;
use std::fs;
use std::path::Path;
fn main() -> error::Result<()> {
    for (i, arg) in env::args().enumerate() {
        if i == 1 {
            let path = Path::new(arg.as_str());
            let buffer = fs::read(path)?;
            match Object::parse(&buffer)? {
                Object::Elf(elf) => {
                    println!("elf: {:#?}", &elf);
                    println!("elf: {:#?}", &elf.header);
                    println!("elf: {:#?}", &elf.header.e_ident);
                }
                Object::PE(pe) => {
                    println!("pe: {:#?}", &pe);
                }
                Object::Mach(mach) => {
                    println!("mach: {:#?}", &mach);
                }
                Object::Archive(archive) => {
                    println!("archive: {:#?}", &archive);
                }
                Object::Unknown(magic) => {
                    println!("unknown magic: {:#x}", magic)
                }
            }
        }
    }
    Ok(())
}
