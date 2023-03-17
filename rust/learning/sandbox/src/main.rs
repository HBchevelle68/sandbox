struct Point1 {
    x: f64,
    y: f64,
    z: f64,
}

type Point2 = (f64, f64, f64);

fn main() {
    println!("Hello, world!");

    let mut p1: Point1 = Point1 {
        x: 1.0,
        y: 2.0,
        z: 7.5,
    };

    let mut p2: Point2 = (3.0, 5.0, 9.0);

    p1.x += 5.0;

    p2.0 += 8.0;

    println!(" Point1 {:?} Point2 {:?}", p1.x, p2.0);
}
