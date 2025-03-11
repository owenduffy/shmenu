PKG=shmenu
VERSION=2.11.0
REL=1
CFLAGS=-I/usr/include/ncurses -DCURSES -DVERSION=\"$(VERSION)\"
LDLIBS=-lncursesw
#BINDIR=/usr/local/bin
-include /usr/share/dpkg/architecture.mk

all: $(PKG)


#install: all
#	install -g users -o owen -m 755 --strip \
#	  menu $(BINDIR)

pkg: $(PKG)-$(VERSION)-$(REL)-$(DEB_BUILD_ARCH).deb

$(PKG)-$(VERSION)-$(REL)-$(DEB_BUILD_ARCH).deb: all
	cp $(PKG) pkg/usr/local/bin
	cd pkg && \
	find . -type f -print0 |grep -zv ^./DEBIAN/ |xargs -0 md5sum |\
		sed 's\  ./\  \g' >./DEBIAN/md5sums
	sed 's/^Version:.*/Version: $(VERSION)/' <control >./pkg/DEBIAN/control
	sed -i 's/^Architecture:.*/Architecture: $(DEB_BUILD_ARCH)/' ./pkg/DEBIAN/control
	sed -i 's/^Depends:.*/Depends: libncursesw5:$(DEB_BUILD_ARCH)|libncursesw6:$(DEB_BUILD_ARCH)/' ./pkg/DEBIAN/control
	dpkg -b pkg $(PKG)-$(VERSION)-$(REL)-$(DEB_BUILD_ARCH).deb
