JEFA_WEB_VERSION = 1.0
JEFA_WEB_SOURCE = jefa_web.tar.gz
JEFA_WEB_SITE_METHOD = file
JEFA_WEB_SITE = ~/systemarm/ARMBuildroot/website

define SHOW_UPTIME_INSTALL_TARGET_CMDS
  $(INSTALL)-D -m 0755 $(@D)/index.html $(TARGET_DIR)/index.html
endef

$(eval $(generic-package))
