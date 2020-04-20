#######################################
#
# poker
#
#######################################

POKER_VERSION = 1.0
POKER_SITE = $(TOPDIR)/../LINSW_LAB_2_Michal_Rogala/poker_src
POKER_SITE_METHOD = local
POKER_DEPENDENCIES = libgpiod

define POKER_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) poker -C $(@D)
endef
define POKER_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/poker $(TARGET_DIR)/usr/bin
endef
POKER_LICENSE = Proprietary

$(eval $(generic-package))
