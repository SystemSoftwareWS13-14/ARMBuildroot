JEFA_WEB_VERSION = 1.
JEFA_WEB_SOURCE = jefa_web.tar.gz
JEFA_WEB_SITE_METHOD = file
JEFA_WEB_SITE = ~/systemarm/ARMBuildroot/application

define SHOW_UPTIME_BUILD_CMDS
  
endef

define SHOW_UPTIME_INSTALL_TARGET_CMDS
  $(INSTALL) -D -m 0755 $(@D)/* $(TARGET_DIR)/www
endef

$(eval $(generic-package))
