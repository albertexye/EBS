fn main() {
    println!("cargo:rustc-link-search=./");
    println!("cargo:rustc-link-search=D:/msys64/mingw64/lib/");
    println!("cargo:rustc-link-lib=xxhash");
}