################################################################################
#
# show_uptime
#
################################################################################

SHOW_UPTIME_VERSION = 0.0.1
SHOW_UPTIME_SOURCE = show_uptime.tar.gz
LIBFOO_SITE_METHOD = file
LIBFOO_SITE = ~/systemarm/armBuildroot/application/

define SHOW_UPTIME_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LIBFOO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/show_uptime_* $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
