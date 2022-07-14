fn main() {
    println!("Hello, world!");

    let y = {
        let x = 3;
        x + 1
    };

    another_function(y);
    another_function(five());
    another_function(loop_counter());
}

fn another_function(x: i32) {
    println!("Another function!");
    println!("The value of x is {x}")
}

fn five() -> i32 {
    5
}

fn loop_counter() -> i32 {
    let mut counter = 0;

    loop {
        counter += 1;

        if counter == 10 {
            break counter * 2;
        }
    }
}
