use goblin::{error, Object};
use std::{env, fs, path::PathBuf};

// ELF Header:
//   Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
//   Class:                             ELF64
//   Data:                              2's complement, little endian
//   Version:                           1 (current)
//   OS/ABI:                            UNIX - System V
//   ABI Version:                       0
//   Type:                              EXEC (Executable file)
//   Machine:                           Advanced Micro Devices X86-64
//   Version:                           0x1
//   Entry point address:               0x401050
//   Start of program headers:          64 (bytes into file)
//   Start of section headers:          13912 (bytes into file)
//   Flags:                             0x0
//   Size of this header:               64 (bytes)
//   Size of program headers:           56 (bytes)
//   Number of program headers:         13
//   Size of section headers:           64 (bytes)
//   Number of section headers:         31
//   Section header string table index: 30

// fn parse_elf_header(elf: goblin::elf::Elf) -> error::Result<()> {
//     //println!("elf: {:#?}", &elf);
//     println!("ELF Header:");
//     print!("\tMagic:\t ");
//     for m in elf.header.e_ident.into_iter() {
//         print!("{:X}    ", m);
//     }
//     println!("\n\tClass:\t {}")
//     //println!("elf: {:#?}", &elf.header);
//     println!("elf: {:#?}", &elf.header.e_ident);
//     Ok(())
// }

fn main() -> error::Result<()> {
    for (i, arg) in env::args().enumerate() {
        if i == 1 {
            let path = Path::new(arg.as_str());
            let buffer = fs::read(path)?;
            match Object::parse(&buffer)? {
                Object::Elf(elf) => {
                    parse_elf_header(elf)?;
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
