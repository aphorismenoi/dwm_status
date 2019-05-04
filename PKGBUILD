# Maintainer: aphorismenoi <aphorismenoi@posteo.org>

pkgname=dwm_status
pkgver=0.4
pkgrel=1
pkgdesc='A status monitor for dwm'
arch=('i686' 'x86_64')
url="https://github.com/aphorismenoi/dwm_status"
depends=('libx11' 'libmpdclient')
makedepends=('git')
source=("git+https://github.com/aphorismenoi/${pkgname}.git")

build() {
    cd "${pkgname}"
    make X11INC='/usr/include/X11' X11LIB='/usr/lib/X11'
}

package() {
    cd "${pkgname}"
    make DESTDIR="${pkgdir}" PREFIX='/usr/' install
}
md5sums=('SKIP')
