// build.rs

use std::process::Command;

fn build_hello() {
    // Build hello.asm
    let status = Command::new("nasm")
        .args(["-f", "elf64", "examples/hello.asm"])
        .status()
        .expect("Failed to assemble hello.asm");
    assert!(status.success());

    let status = Command::new("ld")
        .args(["examples/hello.o", "-o", "examples/hello"])
        .status()
        .expect("Failed to link hello.o");

    assert!(status.success());
}

fn build_c_loader() {
    // Build INSTRUCTOR example with debug
    let status = Command::new("gcc")
        .args([
            "cloader/main.c",
            "-g",
            "-DINSTRUCTOR",
            "-o",
            "cloader/cloader_debug",
        ])
        .status()
        .unwrap();

    assert!(status.success());
}
fn main() {
    build_hello();
    build_c_loader();

    println!("cargo:rerun-if-changed=examples/hello.asm");
    println!("cargo:rerun-if-changed=cloader/main.c");
}
