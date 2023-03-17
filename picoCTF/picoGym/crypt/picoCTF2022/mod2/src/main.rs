use modinverse::modinverse;

fn main() {
    let map = vec![
        "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q",
        "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8",
        "9", "_",
    ];

    let msg = [
        268, 413, 438, 313, 426, 337, 272, 188, 392, 338, 77, 332, 139, 113, 92, 239, 247, 120,
        419, 72, 295, 190, 131,
    ];

    for ct in msg.iter() {
        let rem = ct % 41;

        for i in 1..=41 {
            if ((rem * i) % 41 == 1) {
                print!("{}", map[i]);
            }
        }
    }
}
