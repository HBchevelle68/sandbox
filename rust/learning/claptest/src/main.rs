use clap::{Arg, Command};

fn main() {
    let m = Command::new("")
        .author("Me")
        .version("0.0.1")
        .about("A centralized helper")
        .subcommand_required(true);

    let matches = m.get_matches();

    dbg!(&matches);
}
