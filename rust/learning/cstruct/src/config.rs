// #[macro_use]
// extern crate lazy_static;

// use std::collections::HashMap;

// lazy_static! {
//     static ref HASHMAP: HashMap<u32, &'static str> = {
//         let mut m = HashMap::new();
//         m.insert(0, "123.321.123.321");
//         m.insert(1, "bar");
//         m.insert(2, "baz");
//         m
//     };
// }

struct MyPackedStuct<'a> {
    header: &'a str,
    ttl: u32,
    footer: &'a str,
}

// lazy_static! {
//     static ref MYSTRUCT: MyPackedStuct<'static> = {
//         let m = MyPackedStuct {
//             header: "HEADERHEADERHEADER",
//             ttl: 44444,
//             footer: "FOOTERFOOTERFOOTER",
//         };
//         m
//     };
// }

static mut MYSTRUCT2: MyPackedStuct = MyPackedStuct {
    header: "",
    ttl: "00000012345".parse::<u32>().unwrap(),
    footer: "",
};

pub fn init_config() {
    unsafe {
        MYSTRUCT2 = MyPackedStuct {
            header: "HEADER123",
            ttl: "00000012345".parse::<u32>().unwrap(),
            footer: "FOOTER123",
        };
    }
}

pub fn get_ttl() -> u32 {
    MYSTRUCT2.ttl
}
