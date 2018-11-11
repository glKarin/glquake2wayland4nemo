Name:		glquake2
Version:	0.3.3harmattan1
Release:	3.21+r0.16
Summary:	GL Quake II on wayland for Nemo.

Vendor: karin <beyondk2000@gmail.com>
Packager: karin <beyondk2000@gmail.com>
Group:		Amusements/Games
License:	GPLv2
URL:		https://github.com/glKarin/glquake2wayland4nemo
Source0:	%{name}.tar.gz
BuildRoot:	%(mktemp -ud %{name})

BuildRequires:	pkgconfig(wayland-client)
BuildRequires:	pkgconfig(wayland-egl)
BuildRequires:	pkgconfig(egl)
BuildRequires:	pkgconfig(glesv1_cm)
BuildRequires:	alsa-lib-devel
Requires:	libhybris-libGLESv1
Requires:	libhybris-libEGL
Requires:	libhybris-libwayland-egl
Requires:	wayland
Requires:	fingerterm

%description
GL Quake II + touch on wayland for Nemo. Only support OpenGL hardware renderer.
 A first-person shooter computer game developed by id Software and distributed by Activision.
 To run the game copy pak0.pak from original game to /home/nemo/.quake2/baseq2 folder.
 User executale script is '/home/nemo/.quake2/GLquake2.sh', if the script file is exists, it will execute user script, else execute global default command. See also in shell script: '/usr/share/quake2/GLquake2'.

%define get_arch %(uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc/ -e s/sparc64/sparc/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/alpha/axp/)

%prep
%setup -q -n %{name}


%build
make build_release


%install
rm -rf $RPM_BUILD_ROOT
# shell
install -D -m 755 debian/glquake2.sh $RPM_BUILD_ROOT/usr/share/quake2/GLquake2
# desktop
install -D -m 644 debian/glquake2.desktop $RPM_BUILD_ROOT/usr/share/applications/glquake2.desktop
install -D -m 644 debian/glquake2_80.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/80x80/apps/glquake2_80.png
# binary
install -D -m 755 release%get_arch/quake2 $RPM_BUILD_ROOT/usr/bin/quake2
# lib
install -D -m 755 release%get_arch/game%get_arch.so $RPM_BUILD_ROOT/usr/share/quake2/baseq2/games%get_arch.so
install -D -m 755 release%get_arch/ref_glx.so $RPM_BUILD_ROOT/usr/share/quake2/lib/ref_glx.so
# resource
install -D -m 644 debian/config.cfg $RPM_BUILD_ROOT/usr/share/quake2/baseq2/config.cfg
mkdir -p $RPM_BUILD_ROOT/usr/share/quake2/resc
install -D -m 644 resc/*.png $RPM_BUILD_ROOT/usr/share/quake2/resc


%clean
make clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
/usr/share/quake2/GLquake2
/usr/share/applications/glquake2.desktop
/usr/share/icons/hicolor/80x80/apps/glquake2_80.png
/usr/bin/quake2
/usr/share/quake2/baseq2/games%get_arch.so
/usr/share/quake2/lib/ref_glx.so
/usr/share/quake2/baseq2/config.cfg
/usr/share/quake2/resc



%changelog
* Fri Sep 28 2018 Karin Zhao <beyondk2000@gmail.com> - 0.3.3harmattan1
  * Hardware-Rendering by OpenGLES 1.1.
  * Add On-Screen button and joystick with OpenGL, support multitouch.
	* Using OpenGL FBO to rotate rendering screen.


%post
INSTALL_DIR=/usr/share/quake2
QUAKE2_HOME=/home/nemo/.quake2
BASEQ2=baseq2

if [ "$1" = "1" ]; then
	if [ ! -d ${QUAKE2_HOME} ]; then
		mkdir ${QUAKE2_HOME}
		chown nemo:nemo ${QUAKE2_HOME}
		echo "mkdir ${QUAKE2_HOME}"
		if [ x$? != "x0" ]; then
			echo "[Error]: Create quake2 home directory fail! Exiting..."
			exit 1;
		fi
	fi

	if [ ! -d ${QUAKE2_HOME}/${BASEQ2} ]; then
		mkdir ${QUAKE2_HOME}/${BASEQ2}
		chown nemo:nemo {QUAKE2_HOME}/${BASEQ2}
		echo "mkdir ${QUAKE2_HOME}/${BASEQ2}"
		if [ x$? != "x0" ]; then
			echo "[Error]: Create quake2 base directory fail! Exiting..."
			exit 2;
		fi
	fi

	QLIBS=`ls ${INSTALL_DIR}/lib/`
	for libso in ${QLIBS}; do
		if [ -e ${QUAKE2_HOME}/${libso} ]; then
			rm -f ${QUAKE2_HOME}/${libso}
			echo "rm -f ${QUAKE2_HOME}/${libso}"
		fi
		ln -s ${INSTALL_DIR}/lib/${libso} ${QUAKE2_HOME}
		echo "ln -s ${INSTALL_DIR}/lib/${libso} ${QUAKE2_HOME}"
	done

	QCFGS=`ls ${INSTALL_DIR}/${BASEQ2}`
	for cfg in ${QCFGS}; do
		if [ -e ${QUAKE2_HOME}/${BASEQ2}/${cfg} ]; then
			rm -f ${QUAKE2_HOME}/${BASEQ2}/${cfg}
			echo "rm -f ${QUAKE2_HOME}/${BASEQ2}/${cfg}"
		fi
		if [ "xgamesarm.so" == x${cfg} ]; then
			ln -s ${INSTALL_DIR}/${BASEQ2}/${cfg} ${QUAKE2_HOME}/${BASEQ2}
			echo "ln -s ${INSTALL_DIR}/${BASEQ2}/${cfg} ${QUAKE2_HOME}/${BASEQ2}"
		else
		echo "${QUAKE2_HOME}/${BASEQ2}/${cfg}"
			cp -f ${INSTALL_DIR}/${BASEQ2}/${cfg} ${QUAKE2_HOME}/${BASEQ2}
			chown nemo:nemo ${INSTALL_DIR}/${BASEQ2}/${cfg}
			echo "cp -f ${INSTALL_DIR}/${BASEQ2}/${cfg} ${QUAKE2_HOME}/${BASEQ2}"
		fi
	done

	echo "Quake2 configure done."
fi


%preun
QUAKE2_HOME=/home/nemo/.quake2
REF_SO=ref_*.so
GAME_SO=game*.so
BASEQ2=baseq2

if [ "$1" = "0" ]; then
		if [ -d ${QUAKE2_HOME} ]; then
			Q2LIB=`ls ${QUAKE2_HOME}/${REF_SO} 2> /dev/null || true`
			for libso in ${Q2LIB}; do
				echo "rm -f ${libso}"
				rm -f ${libso}
			done
		fi

		if [ -d ${QUAKE2_HOME}/${BASEQ2} ]; then
			Q2LIB=`ls ${QUAKE2_HOME}/${BASEQ2}/${GAME_SO} 2> /dev/null || true`	
			if [ "x" != x${Q2LIB} ]; then
				echo "rm -f ${Q2LIB}"
				rm -f ${Q2LIB}
			fi
		fi

		echo "Quake2 remove done."
fi
