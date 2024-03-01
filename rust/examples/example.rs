extern crate ebs;
use image::io::Reader as ImageReader;
use rand::RngCore;

fn open_image(filename: &str) -> ebs::Image {
    let img = ImageReader::open(filename).unwrap().decode().unwrap();
    ebs::Image {
        width: img.width() as u64,
        height: img.height() as u64,
        channel: img.color().channel_count() as u64,
        pixels: img.into_bytes(),
    }
}

fn main() {
    let mut image_list: ebs::ImageList = ebs::ImageList(vec![
        open_image("examples/samples/sample1.png"),
        open_image("examples/samples/sample2.png"),
        open_image("examples/samples/sample3.png"),
    ]);
    let mut message: ebs::Message = vec![0;1024];
    rand::thread_rng().fill_bytes(&mut message);
    let _ = image_list.embed(&message, 16);
}
