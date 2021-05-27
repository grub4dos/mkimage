/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2013  Free Software Foundation, Inc.
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

#ifndef GRUB_UTIL_INSTALL_HEADER
#define GRUB_UTIL_INSTALL_HEADER	1

#include <sys/types.h>
#include <stdio.h>

#include <grub/device.h>
#include <grub/disk.h>
#include <grub/emu/hostfile.h>

enum grub_install_options {
  GRUB_INSTALL_OPTIONS_DIRECTORY = 'd',
  GRUB_INSTALL_OPTIONS_VERBOSITY = 'v',
  GRUB_INSTALL_OPTIONS_MODULES = 0x201,
  GRUB_INSTALL_OPTIONS_INSTALL_MODULES,
  GRUB_INSTALL_OPTIONS_INSTALL_THEMES,
  GRUB_INSTALL_OPTIONS_INSTALL_FONTS,
  GRUB_INSTALL_OPTIONS_INSTALL_LOCALES,
  GRUB_INSTALL_OPTIONS_INSTALL_COMPRESS,
  GRUB_INSTALL_OPTIONS_DIRECTORY2,
  GRUB_INSTALL_OPTIONS_LOCALE_DIRECTORY,
  GRUB_INSTALL_OPTIONS_THEMES_DIRECTORY,
  GRUB_INSTALL_OPTIONS_GRUB_MKIMAGE,
  GRUB_INSTALL_OPTIONS_INSTALL_CORE_COMPRESS,
  GRUB_INSTALL_OPTIONS_DTB,
  GRUB_INSTALL_OPTIONS_SBAT,
  GRUB_INSTALL_OPTIONS_DISABLE_SHIM_LOCK
};

typedef enum {
  GRUB_COMPRESSION_AUTO,
  GRUB_COMPRESSION_NONE,
  GRUB_COMPRESSION_XZ,
  GRUB_COMPRESSION_LZMA
} grub_compression_t;

struct grub_install_image_target_desc;

void
grub_install_generate_image (const char *dir, const char *prefix,
			     FILE *out,
			     const char *outname, char *mods[],
			     char *memdisk_path, char **pubkey_paths,
			     size_t npubkeys,
			     char *config_path,
			     const struct grub_install_image_target_desc *image_target,
			     grub_compression_t comp, const char *dtb_file,
			     const char *sbat_path, const int disable_shim_lock);

const struct grub_install_image_target_desc *
grub_install_get_image_target (const char *arg);

char *
grub_install_get_image_targets_string (void);

const char *
grub_util_get_target_dirname (const struct grub_install_image_target_desc *t);

const char *
grub_install_get_default_arm_platform (void);

const char *
grub_install_get_default_x86_platform (void);

int
grub_install_register_efi (grub_device_t efidir_grub_dev,
			   const char *efifile_path,
			   const char *efi_distributor);

void
grub_install_register_ieee1275 (int is_prep, const char *install_device,
				int partno, const char *relpath);

void
grub_install_sgi_setup (const char *install_device,
			const char *imgfile, const char *destname);

int 
grub_install_compress_gzip (const char *src, const char *dest);
int 
grub_install_compress_lzop (const char *src, const char *dest);
int 
grub_install_compress_xz (const char *src, const char *dest);

void
grub_install_get_blocklist (grub_device_t root_dev,
			    const char *core_path, const char *core_img,
			    size_t core_size,
			    void (*callback) (grub_disk_addr_t sector,
					      unsigned offset,
					      unsigned length,
					      void *data),
			    void *hook_data);

void
grub_util_create_envblk_file (const char *name);

void
grub_util_glue_efi (const char *file32, const char *file64, const char *out);

void
grub_util_render_label (const char *label_font,
			const char *label_bgcolor,
			const char *label_color,
			const char *label_string,
			const char *label);

const char *
grub_util_get_target_name (const struct grub_install_image_target_desc *t);

extern char *grub_install_copy_buffer;
#define GRUB_INSTALL_COPY_BUFFER_SIZE 1048576

int
grub_install_is_short_mbrgap_supported (void);

#endif
