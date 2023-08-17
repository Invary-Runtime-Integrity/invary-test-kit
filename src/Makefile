# Invary Test Kit Module
# Copyright (C) 2023 Invary, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# For the complete terms of the GNU General Public License, please see this URL:
# http://www.gnu.org/licenses/gpl-2.0.html
#

# From the kbuild docs
# # linux/Documentation/kbuild/modules.txt
# # linux/Documentation/kbuild/kbuild.txt
# # linux/Documentation/kbuild/makefiles.txt

KMOD_NAME="invary-test-kit"

ifneq ($(KERNELRELEASE),)
include Kbuild
else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) W=1 -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

load: default
	sudo insmod $(KMOD_NAME).ko

package: default
	mkdir -p $(KMOD_NAME)
	cp $(KMOD_NAME).ko $(KMOD_NAME)
	tar cfz $(KMOD_NAME).tar.gz $(KMOD_NAME)/
	mv $(KMOD_NAME).tar.gz /tmp
	rm -rf $(KMOD_NAME)

unload:
	sudo rmmod $(KMOD_NAME) &>/dev/null
endif
