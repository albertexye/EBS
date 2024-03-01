use std::slice;
use libc::c_int;
use num_derive::FromPrimitive;
use num_traits::FromPrimitive;

#[repr(C)]
struct CImage {
    width: u64,
    height: u64,
    channel: u64,
    pixels: *mut u8,
}

#[repr(C)]
struct CImageList {
    size: u64,
    images: *mut CImage,
}

#[repr(C)]
struct CMessage {
    size: u64,
    data: *const u8,
}

#[link(name = "EBS")]
extern {
    fn EBS_MessageEmbed(c_image_list: *mut CImageList, c_message: *const CMessage, square_size: u64, error_code: *mut c_int);
    fn EBS_MessageExtract(c_image_list: *mut CImageList, square_size: u64, error_code: *mut c_int) -> CMessage;
    fn EBS_MessageFree(c_message: *mut CMessage);
}

pub struct Image {
    pub width: u64,
    pub height: u64,
    pub channel: u64,
    pub pixels: Vec<u8>,
}

pub struct ImageList(pub Vec<Image>);
pub type Message = Vec<u8>;

#[derive(FromPrimitive)]
#[repr(i32)]
pub enum Error {
    OOM = 1,
    InvalidMessage = 2,
    Overflow = 3,
    BadSquareSize = 4,
    InvalidImage = 5,
}

impl ImageList {
    fn to_c(&mut self) -> CImageList {
        let mut c_images: Vec<CImage> = Vec::with_capacity(self.0.len());
        for image in &mut self.0 {
            let c_image: CImage = CImage {
                width: image.width,
                height: image.height,
                channel: image.channel,
                pixels: image.pixels.as_mut_ptr(),
            };
            c_images.push(c_image);
        }
        CImageList {
            size: c_images.len() as u64,
            images: c_images.as_mut_ptr(),
        }
    }

    pub fn embed(&mut self, message: &Message, square_size: u64) -> Result<(), Error> {
        let mut c_image_list: CImageList = self.to_c();

        let c_message: CMessage = CMessage {
            size: message.len() as u64,
            data: message.as_ptr(),
        };

        let mut error_code: c_int = 0;

        unsafe { EBS_MessageEmbed(&mut c_image_list as *mut _, &c_message as *const CMessage, square_size, &mut error_code as *mut _); }

        if error_code != 0 {
            return Err(Error::from_i32(error_code).unwrap());
        }

        Ok(())
    }

    pub fn extract(&mut self, square_size: u64) -> Result<Message, Error> {
        let mut c_image_list: CImageList = self.to_c();

        let mut error_code: c_int = 0;

        let mut c_message: CMessage = unsafe { EBS_MessageExtract(&mut c_image_list as *mut _, square_size, &mut error_code as *mut _) };

        if error_code != 0 {
            return Err(Error::from_i32(error_code).unwrap());
        }

        let message = unsafe { slice::from_raw_parts(c_message.data, c_message.size as usize) }.to_vec();

        unsafe { EBS_MessageFree(&mut c_message as *mut _); }

        Ok(message)
    }
}
