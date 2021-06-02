# MKIMAGE

[![release](https://github.com/grub4dos/mkimage/actions/workflows/build.yml/badge.svg)](https://github.com/grub4dos/mkimage/actions/workflows/build.yml) ![](https://img.shields.io/github/license/grub4dos/mkimage)

Make a bootable image of GRUB4EFI.

## Usage

mkimage [OPTION...]

- -c, --config=FILE      embed FILE as an early config 
- -d, --directory=DIR     use images and modules under DIR 
- -o, --output=FILE      output a generated image to FILE [default=stdout] 
- -O, --format=FORMAT     generate an image in FORMAT [available formats: i386-efi, x86_64-efi]
- -p, --prefix=DIR      set prefix directory 
- -f, --font=FILE      embed FILE as a font
- -m, --memdisk      embed FILE as a memdisk image
- -v, --verbose        print verbose messages. 
- -?, --help         give this help list 
- --usage         give a short usage message 
- -V, --version        print program version

```
mkimage -p /efi/grub -o BOOTX64.EFI -O x86_64-efi -c menu.lst -f unifont.gz -m grub4dos.mod
```

