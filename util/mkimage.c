/* grub-mkimage.c - make a bootable image */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <grub/types.h>
#include <grub/elf.h>
#include <grub/aout.h>
#include <grub/i18n.h>
#include <grub/kernel.h>
#include <grub/disk.h>
#include <grub/emu/misc.h>
#include <grub/util/misc.h>
#include <grub/misc.h>
#include <grub/offsets.h>
#include <time.h>
#include <multiboot.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <grub/efi/pe32.h>
#include <grub/arm/reloc.h>
#include <grub/arm64/reloc.h>
#include <grub/osdep/hostfile.h>
#include <grub/util/install.h>
#include <grub/util/mkimage.h>

#define ALIGN_ADDR(x) (ALIGN_UP((x), image_target->voidp_sizeof))

#pragma GCC diagnostic ignored "-Wcast-align"

#define TARGET_NO_FIELD 0xffffffff

/* use 2015-01-01T00:00:00+0000 as a stock timestamp */
#define STABLE_EMBEDDING_TIMESTAMP 1420070400

#define EFI32_HEADER_SIZE ALIGN_UP (GRUB_PE32_MSDOS_STUB_SIZE		\
				    + GRUB_PE32_SIGNATURE_SIZE		\
				    + sizeof (struct grub_pe32_coff_header) \
				    + sizeof (struct grub_pe32_optional_header) \
				    + 4 * sizeof (struct grub_pe32_section_table), \
				    GRUB_PE32_FILE_ALIGNMENT)

#define EFI64_HEADER_SIZE ALIGN_UP (GRUB_PE32_MSDOS_STUB_SIZE		\
				    + GRUB_PE32_SIGNATURE_SIZE		\
				    + sizeof (struct grub_pe32_coff_header) \
				    + sizeof (struct grub_pe64_optional_header) \
				    + 4 * sizeof (struct grub_pe32_section_table), \
				    GRUB_PE32_FILE_ALIGNMENT)

static const struct grub_install_image_target_desc image_targets[] =
  {
    {
      .dirname = "i386-multiboot",
      .names = { "i386-multiboot", NULL},
      .voidp_sizeof = 4,
      .bigendian = 0,
      .id = IMAGE_COREBOOT,
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = 1,
      .vaddr_offset = 0,
      .link_addr = GRUB_KERNEL_I386_COREBOOT_LINK_ADDR,
      .elf_target = EM_386,
      .link_align = 4,
      .mod_gap = GRUB_KERNEL_I386_COREBOOT_MOD_GAP,
      .mod_align = GRUB_KERNEL_I386_COREBOOT_MOD_ALIGN
    },
    {
      .dirname = "i386-efi",
      .names = { "i386-efi", NULL },
      .voidp_sizeof = 4,
      .bigendian = 0,
      .id = IMAGE_EFI,
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI32_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_I386,
      .elf_target = EM_386,
    },
    {
      .dirname = "x86_64-efi",
      .names = { "x86_64-efi", NULL },
      .voidp_sizeof = 8,
      .bigendian = 0, 
      .id = IMAGE_EFI, 
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI64_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_X86_64,
      .elf_target = EM_X86_64,
    },
    {
      .dirname = "arm-efi",
      .names = { "arm-efi", NULL },
      .voidp_sizeof = 4,
      .bigendian = 0, 
      .id = IMAGE_EFI, 
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI32_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_ARMTHUMB_MIXED,
      .elf_target = EM_ARM,
    },
    {
      .dirname = "arm64-efi",
      .names = { "arm64-efi", NULL },
      .voidp_sizeof = 8,
      .bigendian = 0,
      .id = IMAGE_EFI,
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI64_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_ARM64,
      .elf_target = EM_AARCH64,
    },
    {
      .dirname = "riscv32-efi",
      .names = { "riscv32-efi", NULL },
      .voidp_sizeof = 4,
      .bigendian = 0,
      .id = IMAGE_EFI,
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI32_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_RISCV32,
      .elf_target = EM_RISCV,
    },
    {
      .dirname = "riscv64-efi",
      .names = { "riscv64-efi", NULL },
      .voidp_sizeof = 8,
      .bigendian = 0,
      .id = IMAGE_EFI,
      .flags = PLATFORM_FLAGS_NONE,
      .total_module_size = TARGET_NO_FIELD,
      .decompressor_compressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_size = TARGET_NO_FIELD,
      .decompressor_uncompressed_addr = TARGET_NO_FIELD,
      .section_align = GRUB_PE32_SECTION_ALIGNMENT,
      .vaddr_offset = EFI64_HEADER_SIZE,
      .pe_target = GRUB_PE32_MACHINE_RISCV64,
      .elf_target = EM_RISCV,
    },
  };

#include <grub/lib/LzmaEnc.h>

static void *SzAlloc(void *p __attribute__ ((unused)), size_t size) { return xmalloc(size); }
static void SzFree(void *p __attribute__ ((unused)), void *address) { free(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static void
compress_kernel_lzma (char *kernel_img, size_t kernel_size,
		      char **core_img, size_t *core_size)
{
  CLzmaEncProps props;
  unsigned char out_props[5];
  size_t out_props_size = 5;

  LzmaEncProps_Init(&props);
  props.dictSize = 1 << 16;
  props.lc = 3;
  props.lp = 0;
  props.pb = 2;
  props.numThreads = 1;

  *core_img = xmalloc (kernel_size);

  *core_size = kernel_size;
  if (LzmaEncode ((unsigned char *) *core_img, core_size,
		  (unsigned char *) kernel_img,
		  kernel_size,
		  &props, out_props, &out_props_size,
		  0, NULL, &g_Alloc, &g_Alloc) != SZ_OK)
    grub_util_error ("%s", _("cannot compress the kernel image"));
}

static void
compress_kernel (const struct grub_install_image_target_desc *image_target, char *kernel_img,
		 size_t kernel_size, char **core_img, size_t *core_size,
		 grub_compression_t comp)
{
  if (image_target->flags & PLATFORM_FLAGS_DECOMPRESSORS
      && (comp == GRUB_COMPRESSION_LZMA))
    {
      compress_kernel_lzma (kernel_img, kernel_size, core_img,
			    core_size);
      return;
    }

 if (image_target->flags & PLATFORM_FLAGS_DECOMPRESSORS
     && (comp != GRUB_COMPRESSION_NONE))
   grub_util_error (_("unknown compression %d"), comp);

  *core_img = xmalloc (kernel_size);
  memcpy (*core_img, kernel_img, kernel_size);
  *core_size = kernel_size;
}

const struct grub_install_image_target_desc *
grub_install_get_image_target (const char *arg)
{
  unsigned i, j;
  for (i = 0; i < ARRAY_SIZE (image_targets); i++)
    for (j = 0; j < ARRAY_SIZE (image_targets[i].names) &&
		    image_targets[i].names[j]; j++)
      if (strcmp (arg, image_targets[i].names[j]) == 0)
	return &image_targets[i];
  return NULL;
}

const char *
grub_util_get_target_dirname (const struct grub_install_image_target_desc *t)
{
  return t->dirname;
}

const char *
grub_util_get_target_name (const struct grub_install_image_target_desc *t)
{
  return t->names[0];
}

char *
grub_install_get_image_targets_string (void)
{
  int format_len = 0;
  char *formats;
  char *ptr;
  unsigned i;
  for (i = 0; i < ARRAY_SIZE (image_targets); i++)
    format_len += strlen (image_targets[i].names[0]) + 2;
  ptr = formats = xmalloc (format_len);
  for (i = 0; i < ARRAY_SIZE (image_targets); i++)
    {
      strcpy (ptr, image_targets[i].names[0]);
      ptr += strlen (image_targets[i].names[0]);
      *ptr++ = ',';
      *ptr++ = ' ';
    }
  ptr[-2] = 0;

  return formats;
}

/*
 * The image_target parameter is used by the grub_host_to_target32() macro.
 */
static struct grub_pe32_section_table *
init_pe_section(const struct grub_install_image_target_desc *image_target,
		struct grub_pe32_section_table *section,
		const char * const name,
		grub_uint32_t *vma, grub_uint32_t vsz, grub_uint32_t valign,
		grub_uint32_t *rda, grub_uint32_t rsz,
		grub_uint32_t characteristics)
{
  size_t len = strlen (name);

  if (len > sizeof (section->name))
    grub_util_error (_("section name %s length is bigger than %lu"),
		     name, (unsigned long) sizeof (section->name));

  memcpy (section->name, name, len);

  section->virtual_address = grub_host_to_target32 (*vma);
  section->virtual_size = grub_host_to_target32 (vsz);
  (*vma) = ALIGN_UP (*vma + vsz, valign);

  section->raw_data_offset = grub_host_to_target32 (*rda);
  section->raw_data_size = grub_host_to_target32 (rsz);
  (*rda) = ALIGN_UP (*rda + rsz, GRUB_PE32_FILE_ALIGNMENT);

  section->characteristics = grub_host_to_target32 (characteristics);

  return section + 1;
}

/*
 * tmp_ is just here so the compiler knows we'll never derefernce a NULL.
 * It should get fully optimized away.
 */
#define PE_OHDR(o32, o64, field) (*(		\
{						\
  __typeof__((o64)->field) tmp_;		\
  __typeof__((o64)->field) *ret_ = &tmp_;	\
  if (o32)					\
    ret_ = (void *)(&((o32)->field));		\
  else if (o64)				\
    ret_ = (void *)(&((o64)->field));		\
  ret_;					\
}))

void
grub_install_generate_image (const char *dir, const char *prefix,
			     FILE *out, const char *outname, char *mods[],
			     char *memdisk_path, char **pubkey_paths,
			     size_t npubkeys, char *config_path,
			     const struct grub_install_image_target_desc *image_target,
			     grub_compression_t comp, const char *dtb_path,
			     const char *sbat_path, const char *font_path)
{
  char *kernel_img, *core_img;
  size_t total_module_size, core_size;
  size_t memdisk_size = 0, config_size = 0;
  size_t prefix_size = 0, dtb_size = 0, sbat_size = 0, font_size = 0;
  char *kernel_path;
  size_t offset;
  char *mod_path;
  size_t j;
  size_t decompress_size = 0;
  struct grub_mkimage_layout layout;

  if (comp == GRUB_COMPRESSION_AUTO)
    comp = image_target->default_compression;

  kernel_path = grub_util_get_path (dir, "kernel.img");

  if (image_target->voidp_sizeof == 8)
    total_module_size = sizeof (struct grub_module_info64);
  else
    total_module_size = sizeof (struct grub_module_info32);

  {
    size_t i;
    for (i = 0; i < npubkeys; i++)
      {
	size_t curs;
	curs = ALIGN_ADDR (grub_util_get_image_size (pubkey_paths[i]));
	grub_util_info ("the size of public key %u is 0x%"
			GRUB_HOST_PRIxLONG_LONG,
			(unsigned) i, (unsigned long long) curs);
	total_module_size += curs + sizeof (struct grub_module_header);
      }
  }

  if (memdisk_path)
    {
      memdisk_size = ALIGN_UP(grub_util_get_image_size (memdisk_path), 512);
      grub_util_info ("the size of memory disk is 0x%" GRUB_HOST_PRIxLONG_LONG,
		      (unsigned long long) memdisk_size);
      total_module_size += memdisk_size + sizeof (struct grub_module_header);
    }

  if (dtb_path)
    {
      dtb_size = ALIGN_ADDR(grub_util_get_image_size (dtb_path));
      total_module_size += dtb_size + sizeof (struct grub_module_header);
    }

  if (sbat_path != NULL && image_target->id != IMAGE_EFI)
    grub_util_error (_(".sbat section can be embedded into EFI images only"));

  if (font_path)
    {
      font_size = ALIGN_ADDR(grub_util_get_image_size (font_path));
      total_module_size += font_size + sizeof (struct grub_module_header);
    }

  if (config_path)
    {
      config_size = ALIGN_ADDR (grub_util_get_image_size (config_path) + 1);
      grub_util_info ("the size of config file is 0x%" GRUB_HOST_PRIxLONG_LONG,
		      (unsigned long long) config_size);
      total_module_size += config_size + sizeof (struct grub_module_header);
    }

  if (prefix)
    {
      prefix_size = ALIGN_ADDR (strlen (prefix) + 1);
      total_module_size += prefix_size + sizeof (struct grub_module_header);
    }

  for (j = 0; mods[j]; j++)
  {
    mod_path = grub_util_get_path (dir, mods[j]);
    total_module_size += (ALIGN_ADDR (grub_util_get_image_size (mod_path))
                          + sizeof (struct grub_module_header));
    free (mod_path);
  }

  grub_util_info ("the total module size is 0x%" GRUB_HOST_PRIxLONG_LONG,
		  (unsigned long long) total_module_size);

  if (image_target->voidp_sizeof == 4)
    kernel_img = grub_mkimage_load_image32 (kernel_path, total_module_size,
					    &layout, image_target);
  else
    kernel_img = grub_mkimage_load_image64 (kernel_path, total_module_size,
					    &layout, image_target);

  if ((image_target->flags & PLATFORM_FLAGS_DECOMPRESSORS)
      && (image_target->total_module_size != TARGET_NO_FIELD))
    *((grub_uint32_t *) (kernel_img + image_target->total_module_size))
      = grub_host_to_target32 (total_module_size);

  if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
    {
      memmove (kernel_img + total_module_size, kernel_img, layout.kernel_size);
      memset (kernel_img, 0, total_module_size);
    }

  if (image_target->voidp_sizeof == 8)
    {
      /* Fill in the grub_module_info structure.  */
      struct grub_module_info64 *modinfo;
      if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
	modinfo = (struct grub_module_info64 *) kernel_img;
      else
	modinfo = (struct grub_module_info64 *) (kernel_img + layout.kernel_size);
      modinfo->magic = grub_host_to_target32 (GRUB_MODULE_MAGIC);
      modinfo->offset = grub_host_to_target_addr (sizeof (struct grub_module_info64));
      modinfo->size = grub_host_to_target_addr (total_module_size);
      if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
	offset = sizeof (struct grub_module_info64);
      else
	offset = layout.kernel_size + sizeof (struct grub_module_info64);
    }
  else
    {
      /* Fill in the grub_module_info structure.  */
      struct grub_module_info32 *modinfo;
      if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
	modinfo = (struct grub_module_info32 *) kernel_img;
      else
	modinfo = (struct grub_module_info32 *) (kernel_img + layout.kernel_size);
      modinfo->magic = grub_host_to_target32 (GRUB_MODULE_MAGIC);
      modinfo->offset = grub_host_to_target_addr (sizeof (struct grub_module_info32));
      modinfo->size = grub_host_to_target_addr (total_module_size);
      if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
	offset = sizeof (struct grub_module_info32);
      else
	offset = layout.kernel_size + sizeof (struct grub_module_info32);
    }

  for (j = 0; mods[j]; j++)
  {
    struct grub_module_header *header;
    size_t mod_size;

    mod_path = grub_util_get_path (dir, mods[j]);
    mod_size = ALIGN_ADDR (grub_util_get_image_size (mod_path));

    header = (struct grub_module_header *) (kernel_img + offset);
    header->type = grub_host_to_target32 (OBJ_TYPE_ELF);
    header->size = grub_host_to_target32 (mod_size + sizeof (*header));
    offset += sizeof (*header);

    grub_util_load_image (mod_path, kernel_img + offset);
    offset += mod_size;
    free (mod_path);
  }

  {
    size_t i;
    for (i = 0; i < npubkeys; i++)
      {
	size_t curs;
	struct grub_module_header *header;

	curs = grub_util_get_image_size (pubkey_paths[i]);

	header = (struct grub_module_header *) (kernel_img + offset);
	header->type = grub_host_to_target32 (OBJ_TYPE_PUBKEY);
	header->size = grub_host_to_target32 (curs + sizeof (*header));
	offset += sizeof (*header);

	grub_util_load_image (pubkey_paths[i], kernel_img + offset);
	offset += ALIGN_ADDR (curs);
      }
  }

  if (memdisk_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      header->type = grub_host_to_target32 (OBJ_TYPE_MEMDISK);
      header->size = grub_host_to_target32 (memdisk_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (memdisk_path, kernel_img + offset);
      offset += memdisk_size;
    }

  if (dtb_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      header->type = grub_host_to_target32 (OBJ_TYPE_DTB);
      header->size = grub_host_to_target32 (dtb_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (dtb_path, kernel_img + offset);
      offset += dtb_size;
    }

  if (font_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      header->type = grub_host_to_target32 (OBJ_TYPE_FONT);
      header->size = grub_host_to_target32 (font_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (font_path, kernel_img + offset);
      offset += font_size;
    }

  if (config_path)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      header->type = grub_host_to_target32 (OBJ_TYPE_CONFIG);
      header->size = grub_host_to_target32 (config_size + sizeof (*header));
      offset += sizeof (*header);

      grub_util_load_image (config_path, kernel_img + offset);
      offset += config_size;
    }

  if (prefix)
    {
      struct grub_module_header *header;

      header = (struct grub_module_header *) (kernel_img + offset);
      header->type = grub_host_to_target32 (OBJ_TYPE_PREFIX);
      header->size = grub_host_to_target32 (prefix_size + sizeof (*header));
      offset += sizeof (*header);

      grub_strcpy (kernel_img + offset, prefix);
      offset += prefix_size;
    }

  grub_util_info ("kernel_img=%p, kernel_size=0x%" GRUB_HOST_PRIxLONG_LONG,
		  kernel_img,
		  (unsigned long long) layout.kernel_size);
  compress_kernel (image_target, kernel_img, layout.kernel_size + total_module_size,
		   &core_img, &core_size, comp);
  free (kernel_img);

  grub_util_info ("the core size is 0x%" GRUB_HOST_PRIxLONG_LONG,
		  (unsigned long long) core_size);

  if (!(image_target->flags & PLATFORM_FLAGS_DECOMPRESSORS) 
      && image_target->total_module_size != TARGET_NO_FIELD)
    *((grub_uint32_t *) (core_img + image_target->total_module_size))
      = grub_host_to_target32 (total_module_size);

  if (image_target->flags & PLATFORM_FLAGS_DECOMPRESSORS)
    {
      char *full_img;
      size_t full_size;
      char *decompress_path, *decompress_img;
      const char *name;

      switch (comp)
	{
	case GRUB_COMPRESSION_XZ:
	  name = "xz_decompress.img";
	  break;
	case GRUB_COMPRESSION_LZMA:
	  name = "lzma_decompress.img";
	  break;
	case GRUB_COMPRESSION_NONE:
	  name = "none_decompress.img";
	  break;
	default:
	  grub_util_error (_("unknown compression %d"), comp);
	}

      decompress_path = grub_util_get_path (dir, name);
      decompress_size = grub_util_get_image_size (decompress_path);
      decompress_img = grub_util_read_image (decompress_path);

      if (image_target->decompressor_compressed_size != TARGET_NO_FIELD)
	*((grub_uint32_t *) (decompress_img
			     + image_target->decompressor_compressed_size))
	  = grub_host_to_target32 (core_size);

      if (image_target->decompressor_uncompressed_size != TARGET_NO_FIELD)
	*((grub_uint32_t *) (decompress_img
			     + image_target->decompressor_uncompressed_size))
	  = grub_host_to_target32 (layout.kernel_size + total_module_size);

      if (image_target->decompressor_uncompressed_addr != TARGET_NO_FIELD)
	{
	  if (image_target->flags & PLATFORM_FLAGS_MODULES_BEFORE_KERNEL)
	    *((grub_uint32_t *) (decompress_img + image_target->decompressor_uncompressed_addr))
	      = grub_host_to_target_addr (image_target->link_addr - total_module_size);
	  else
	    *((grub_uint32_t *) (decompress_img + image_target->decompressor_uncompressed_addr))
	      = grub_host_to_target_addr (image_target->link_addr);
	}
      full_size = core_size + decompress_size;

      full_img = xmalloc (full_size);

      memcpy (full_img, decompress_img, decompress_size);

      memcpy (full_img + decompress_size, core_img, core_size);

      free (core_img);
      core_img = full_img;
      core_size = full_size;
      free (decompress_img);
      free (decompress_path);
    }

  switch (image_target->id)
    {
    case IMAGE_COREBOOT:
	if (image_target->elf_target != EM_ARM && layout.kernel_size + layout.bss_size + GRUB_KERNEL_I386_PC_LINK_ADDR > 0x68000)
	  grub_util_error (_("kernel image is too big (0x%x > 0x%x)"),
			   (unsigned) layout.kernel_size + (unsigned) layout.bss_size
			   + GRUB_KERNEL_I386_PC_LINK_ADDR,
			   0x68000);
	break;
    case IMAGE_EFI:
      break;
    }

  switch (image_target->id)
    {
    case IMAGE_EFI:
      {
	char *pe_img, *pe_sbat, *header;
	struct grub_pe32_section_table *section;
	size_t n_sections = 4;
	size_t scn_size;
	grub_uint32_t vma, raw_data;
	size_t pe_size, header_size;
	struct grub_pe32_coff_header *c;
	static const grub_uint8_t stub[] = GRUB_PE32_MSDOS_STUB;
	struct grub_pe32_optional_header *o32 = NULL;
	struct grub_pe64_optional_header *o64 = NULL;

	if (image_target->voidp_sizeof == 4)
	  header_size = EFI32_HEADER_SIZE;
	else
	  header_size = EFI64_HEADER_SIZE;

	vma = raw_data = header_size;

	if (sbat_path != NULL)
	  {
	    sbat_size = ALIGN_ADDR (grub_util_get_image_size (sbat_path));
	    sbat_size = ALIGN_UP (sbat_size, GRUB_PE32_FILE_ALIGNMENT);
	  }

	pe_size = ALIGN_UP (header_size + core_size, GRUB_PE32_FILE_ALIGNMENT) +
          ALIGN_UP (layout.reloc_size, GRUB_PE32_FILE_ALIGNMENT) + sbat_size;
	header = pe_img = xcalloc (1, pe_size);

	memcpy (pe_img + raw_data, core_img, core_size);

	/* The magic.  */
	memcpy (header, stub, GRUB_PE32_MSDOS_STUB_SIZE);
	memcpy (header + GRUB_PE32_MSDOS_STUB_SIZE, "PE\0\0",
		GRUB_PE32_SIGNATURE_SIZE);

	/* The COFF file header.  */
	c = (struct grub_pe32_coff_header *) (header + GRUB_PE32_MSDOS_STUB_SIZE
					      + GRUB_PE32_SIGNATURE_SIZE);
	c->machine = grub_host_to_target16 (image_target->pe_target);

	if (sbat_path != NULL)
	  n_sections++;

	c->num_sections = grub_host_to_target16 (n_sections);
	c->time = grub_host_to_target32 (STABLE_EMBEDDING_TIMESTAMP);
	c->characteristics = grub_host_to_target16 (GRUB_PE32_EXECUTABLE_IMAGE
						    | GRUB_PE32_LINE_NUMS_STRIPPED
						    | ((image_target->voidp_sizeof == 4)
						       ? GRUB_PE32_32BIT_MACHINE
						       : 0)
						    | GRUB_PE32_LOCAL_SYMS_STRIPPED
						    | GRUB_PE32_DEBUG_STRIPPED);

	/* The PE Optional header.  */
	if (image_target->voidp_sizeof == 4)
	  {
	    c->optional_header_size = grub_host_to_target16 (sizeof (struct grub_pe32_optional_header));

	    o32 = (struct grub_pe32_optional_header *)
              (header + GRUB_PE32_MSDOS_STUB_SIZE + GRUB_PE32_SIGNATURE_SIZE +
               sizeof (struct grub_pe32_coff_header));
	    o32->magic = grub_host_to_target16 (GRUB_PE32_PE32_MAGIC);
	    o32->data_base = grub_host_to_target32 (header_size + layout.exec_size);

	    section = (struct grub_pe32_section_table *)(o32 + 1);
	  }
	else
	  {
	    c->optional_header_size = grub_host_to_target16 (sizeof (struct grub_pe64_optional_header));
	    o64 = (struct grub_pe64_optional_header *)
		  (header + GRUB_PE32_MSDOS_STUB_SIZE + GRUB_PE32_SIGNATURE_SIZE +
                   sizeof (struct grub_pe32_coff_header));
	    o64->magic = grub_host_to_target16 (GRUB_PE32_PE64_MAGIC);

	    section = (struct grub_pe32_section_table *)(o64 + 1);
	  }

	PE_OHDR (o32, o64, header_size) = grub_host_to_target32 (header_size);
	PE_OHDR (o32, o64, entry_addr) = grub_host_to_target32 (layout.start_address);
	PE_OHDR (o32, o64, image_base) = 0;
	PE_OHDR (o32, o64, image_size) = grub_host_to_target32 (pe_size);
	PE_OHDR (o32, o64, section_alignment) = grub_host_to_target32 (image_target->section_align);
	PE_OHDR (o32, o64, file_alignment) = grub_host_to_target32 (GRUB_PE32_FILE_ALIGNMENT);
	PE_OHDR (o32, o64, subsystem) = grub_host_to_target16 (GRUB_PE32_SUBSYSTEM_EFI_APPLICATION);

	/* Do these really matter? */
	PE_OHDR (o32, o64, stack_reserve_size) = grub_host_to_target32 (0x10000);
	PE_OHDR (o32, o64, stack_commit_size) = grub_host_to_target32 (0x10000);
	PE_OHDR (o32, o64, heap_reserve_size) = grub_host_to_target32 (0x10000);
	PE_OHDR (o32, o64, heap_commit_size) = grub_host_to_target32 (0x10000);

	PE_OHDR (o32, o64, num_data_directories) = grub_host_to_target32 (GRUB_PE32_NUM_DATA_DIRECTORIES);

	/* The sections.  */
	PE_OHDR (o32, o64, code_base) = grub_host_to_target32 (vma);
	PE_OHDR (o32, o64, code_size) = grub_host_to_target32 (layout.exec_size);
	section = init_pe_section (image_target, section, ".text",
				   &vma, layout.exec_size,
				   image_target->section_align,
				   &raw_data, layout.exec_size,
				   GRUB_PE32_SCN_CNT_CODE |
				   GRUB_PE32_SCN_MEM_EXECUTE |
				   GRUB_PE32_SCN_MEM_READ);

	scn_size = ALIGN_UP (layout.kernel_size - layout.exec_size, GRUB_PE32_FILE_ALIGNMENT);
	/* ALIGN_UP (sbat_size, GRUB_PE32_FILE_ALIGNMENT) is done earlier. */
	PE_OHDR (o32, o64, data_size) = grub_host_to_target32 (scn_size + sbat_size +
							       ALIGN_UP (total_module_size,
									 GRUB_PE32_FILE_ALIGNMENT));

	section = init_pe_section (image_target, section, ".data",
				   &vma, scn_size, image_target->section_align,
				   &raw_data, scn_size,
				   GRUB_PE32_SCN_CNT_INITIALIZED_DATA |
				   GRUB_PE32_SCN_MEM_READ |
				   GRUB_PE32_SCN_MEM_WRITE);

	scn_size = pe_size - layout.reloc_size - sbat_size - raw_data;
	section = init_pe_section (image_target, section, "mods",
				   &vma, scn_size, image_target->section_align,
				   &raw_data, scn_size,
				   GRUB_PE32_SCN_CNT_INITIALIZED_DATA |
				   GRUB_PE32_SCN_MEM_READ |
				   GRUB_PE32_SCN_MEM_WRITE);

	if (sbat_path != NULL)
	  {
	    pe_sbat = pe_img + raw_data;
	    grub_util_load_image (sbat_path, pe_sbat);

	    section = init_pe_section (image_target, section, ".sbat",
				       &vma, sbat_size,
				       image_target->section_align,
				       &raw_data, sbat_size,
				       GRUB_PE32_SCN_CNT_INITIALIZED_DATA |
				       GRUB_PE32_SCN_MEM_READ);
	  }

	scn_size = layout.reloc_size;
	PE_OHDR (o32, o64, base_relocation_table.rva) = grub_host_to_target32 (vma);
	PE_OHDR (o32, o64, base_relocation_table.size) = grub_host_to_target32 (scn_size);
	memcpy (pe_img + raw_data, layout.reloc_section, scn_size);
	init_pe_section (image_target, section, ".reloc",
			 &vma, scn_size, image_target->section_align,
			 &raw_data, scn_size,
			 GRUB_PE32_SCN_CNT_INITIALIZED_DATA |
			 GRUB_PE32_SCN_MEM_DISCARDABLE |
			 GRUB_PE32_SCN_MEM_READ);

	free (core_img);
	core_img = pe_img;
	core_size = pe_size;
      }
      break;

    case IMAGE_COREBOOT:
      {
	grub_uint64_t target_addr = image_target->link_addr;
	if (image_target->voidp_sizeof == 4)
	  grub_mkimage_generate_elf32 (image_target, &core_img, &core_size,
				       target_addr, &layout);
	else
	  grub_mkimage_generate_elf64 (image_target, &core_img, &core_size,
				       target_addr, &layout);
      }
      break;
    }

  grub_util_write_image (core_img, core_size, out, outname);
  free (core_img);
  free (kernel_path);
  free (layout.reloc_section);
}
