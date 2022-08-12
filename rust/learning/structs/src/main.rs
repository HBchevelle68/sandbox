#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn area(&self) -> u32 {
        self.width * self.height
    }

    fn can_hold(&self, other: &Rectangle) -> bool {
        self.width > other.width && self.height > other.height
    }

    fn square(size: u32) -> Rectangle {
        Rectangle {
            width: size,
            height: size,
        }
    }

    // fn mut_inc_all(&mut self) {
    //     self.width += 1;
    //     self.height += 1;
    // }
}
955-2834
fn main() {
    println!("Hello, structs!");

    let width1 = 30;
    let height1 = 50;

    let rect = (30, 50);

    let rect1 = Rectangle {
        width: dbg!(30 * 1),
        height: 50,
    };

    let rect2 = Rectangle {
        width: 10,
        height: 40,
    };
    let rect3 = Rectangle {
        width: 60,
        height: 45,
    };

    let rect4 = Rectangle::square(25);

    println!(
        "SCALARS: The area of the rectangle is {} square pixels.",
        area_scalars(width1, height1)
    );

    println!(
        "TUPLE: The area of the rectangle is {} square pixels.",
        area_tuple(rect)
    );

    println!(
        "STRUCT: The area of the rectangle is {} square pixels.",
        area_struct(&rect1)
    );

    // dbg!(&rect1);
    // rect1.mut_inc_all();
    // dbg!(&rect1);

    println!(
        "STRUCT METHOD: The area of the rectangle is {} square pixels.",
        rect1.area()
    );

    println!("rect1 is pretty print {:?}", rect1);
    println!("rect1 is even prettier print {:#?}", rect1);

    println!("Can rect1 hold rect2? {}", rect1.can_hold(&rect2));
    println!("Can rect1 hold rect3? {}", rect1.can_hold(&rect3));
    println!("Can rect1 hold rect4? {}", rect1.can_hold(&rect4));

    dbg!(&rect1);
}

fn area_scalars(width: u32, height: u32) -> u32 {
    width * height
}

fn area_tuple(dimensions: (u32, u32)) -> u32 {
    dimensions.0 * dimensions.1
}

fn area_struct(rect: &Rectangle) -> u32 {
    rect.width * rect.height
}
