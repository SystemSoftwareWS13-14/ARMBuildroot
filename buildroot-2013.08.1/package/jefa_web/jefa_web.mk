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
  mkdir -p $(TARGET_DIR)/www
  cp $(@D)/index.html $(TARGET_DIR)/www
  cp -r $(@D)/cgi-bin $(TARGET_DIR)/www
  cp -r $(@D)/jquery-ui $(TARGET_DIR)/www
endef

$(eval $(generic-package))
