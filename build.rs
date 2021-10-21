extern crate cmake;
use cmake::Config;

fn main()
{
	let dst = Config::new("lib")		// source directory equipped with CMake driven build
					.build();			// run cmake to generate makefiles and build it all eventually
	
	println!("cargo:rustc-link-search=native={}", dst.display());
	println!("cargo:rustc-link-lib=static=tree");				// tells which make targets to run
	println!("cargo:rustc-link-lib=static=wordlist");
}