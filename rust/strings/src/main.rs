fn main() {
    let s1 = String::from("Hello ");
    let s2 = String::from("world");
    let s3 = String::from("!");

    let s4 = s1 + &s2 + &s3;

    dbg!(&s4);

    println!("String: {:?}", &s4);
}
