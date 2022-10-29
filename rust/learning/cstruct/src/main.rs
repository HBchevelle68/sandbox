mod config;
//use crate::config;

fn main() {
    println!("Hello, world!");

    // // First access to `HASHMAP` initializes it
    // println!("The entry for `0` is \"{}\".", HASHMAP.get(&0).unwrap());

    // // Any further access to `HASHMAP` just returns the computed value
    // println!("The entry for `1` is \"{}\".", HASHMAP.get(&1).unwrap());

    // let mut _a_copy = MYSTRUCT._a;
    // let mut _b_copy = MYSTRUCT._b;

    // println!("My struct: {:?} {:?}", _a_copy, _b_copy);

    // let mynum = *MYNUM0;

    // println!("TTL is {}", mynum);

    // println!("TTL is {}", unsafe { TTL });

    // unsafe {
    //     TTL += 11111;
    // }
    // println!("TTL is {}", unsafe { TTL });

    // println!(
    //     "{} {} {} {} {}",
    //     unsafe { CONFIG.header },
    //     unsafe { CONFIG._ttl[0] },
    //     unsafe { CONFIG._num[0] },
    //     unsafe { CONFIG._ip },
    //     unsafe { CONFIG.footer }
    // );
    config::init_config();
    println!("TTL is {}", config::get_ttl());
    println!("TTL is {}", "00000012345".parse::<u32>().unwrap());
}
