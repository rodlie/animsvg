FROM rockylinux:9

RUN dnf -y install dnf-plugins-core
RUN dnf -y config-manager --enable crb
RUN dnf -y config-manager --enable devel
RUN dnf -y update
RUN dnf -y group install "Development Tools"
RUN dnf -y install git cmake python3 yasm rpmdevtools ninja-build qt5-devel libmypaint-devel zlib-devel
RUN ln -sf /usr/bin/python3 /usr/bin/python

COPY run.sh /
CMD [ "bash", "./run.sh" ]
