include $(TOPDIR)/rules.mk

PKG_NAME:=duplicate-ips-monitor
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/duplicate-ips-monitor
	CATEGORY:=Base system
	TITLE:=duplicate-ips-monitor
	DEPENDS:=+libubox +libubus
endef

define Package/duplicate-ips-monitor/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/duplicate-ips-monitor $(1)/usr/bin/
endef

$(eval $(call BuildPackage,duplicate-ips-monitor))
