################################################################################
#
# jefa_web
#
################################################################################

JEFA_WEB_VERSION = 1.0
JEFA_WEB_SOURCE = jefa_web.tar.gz
JEFA_WEB_SITE_METHOD = file
JEFA_WEB_SITE = ~/systemarm/ARMBuildroot/website

define JEFA_WEB_INSTALL_TARGET_CMDS
  $(INSTALL) -D -m 0755 $(@D)/index.html $(TARGET_DIR)/www/index.html
  $(INSTALL) -D -d -m 0755 $(@D)/cgi-bin $(TARGET_DIR)/www/cgi-bin
endef

$(eval $(generic-package))
