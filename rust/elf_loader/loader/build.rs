// build.rs

use std::process::Command;

fn build_hello() {
    // Build hello.asm
    let status = Command::new("nasm")
        .args(["-f", "elf64", "samples/hello.asm"])
        .status()
        .expect("Failed to assemble hello.asm");
    assert!(status.success());

    let status = Command::new("ld")
        .args(["samples/hello.o", "-o", "samples/hello"])
        .status()
        .expect("Failed to link hello.o");

    assert!(status.success());
}

fn build_nodata() {
    // Build nodata.asm
    let status = Command::new("nasm")
        .args(["-f", "elf64", "samples/nodata.asm"])
        .status()
        .expect("Failed to assemble nodata.asm");
    assert!(status.success());

    let status = Command::new("ld")
        .args(["samples/nodata.o", "-o", "samples/nodata"])
        .status()
        .expect("Failed to link nodata.o");

    assert!(status.success());
}

fn build_hello_pie() {
    // Build hello_pie.asm
    let status = Command::new("nasm")
        .args(["-f", "elf64", "samples/hello_pie.asm"])
        .status()
        .expect("Failed to assemble hello_pie.asm");
    assert!(status.success());

    let status = Command::new("ld")
        .args([
            "--dynamic-linker",
            "/lib64/ld-linux-x86-64.so.2",
            "-pie",
            "samples/hello_pie.o",
            "-o",
            "samples/hello_pie",
        ])
        .status()
        .expect("Failed to link hello_pie.o");

    assert!(status.success());
}

fn build_entry_point() {
    let status = Command::new("gcc")
        .args(["samples/entry_point.c", "-g", "-o", "samples/entry_point"])
        .status()
        .expect("Failed to build entry_point");

    assert!(status.success());
}

fn build_samples() {
    build_hello();
    build_nodata();
    build_hello_pie();
    build_entry_point();
}

fn build_c_loader() {
    // Build INSTRUCTOR example with debug
    let status = Command::new("gcc")
        .args([
            "cloader/main.c",
            "cloader/instructor.c",
            "-g",
            "-DINSTRUCTOR",
            "-o",
            "cloader/cloader_debug",
        ])
        .status()
        .expect("Failed to build CLOADER");

    assert!(status.success());
}

fn main() {
    build_samples();
    build_c_loader();

    // Assembly
    println!("cargo:rerun-if-changed=samples/hello.asm");
    println!("cargo:rerun-if-changed=samples/nodata.asm");
    println!("cargo:rerun-if-changed=samples/hello_pie.asm");

    // C Loader
    println!("cargo:rerun-if-changed=samples/entry_point.c");
    println!("cargo:rerun-if-changed=cloader/main.c");
    println!("cargo:rerun-if-changed=cloader/instructor.c");
    println!("cargo:rerun-if-changed=cloader/instructor.h");
}
