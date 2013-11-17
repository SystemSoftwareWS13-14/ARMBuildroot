################################################################################
#
# show_uptime
#
################################################################################

SHOW_UPTIME_VERSION = 1.0
SHOW_UPTIME_SOURCE = show_uptime.tar.gz
SHOW_UPTIME_SITE_METHOD = file
SHOW_UPTIME_SITE = ~/systemarm/ARMBuildroot/application

define SHOW_UPTIME_BUILD_CMDS
  $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define SHOW_UPTIME_INSTALL_TARGET_CMDS
  $(INSTALL) -D -m 0755 $(@D)/show_uptime_* $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
