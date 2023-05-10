FROM ubuntu:lunar

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update -y
RUN apt upgrade -y
RUN apt install -y curl build-essential git libtool autoconf automake cmake python3 python-is-python3 ninja-build libfontconfig1-dev libfreetype-dev libavcodec-dev libavformat-dev libavutil-dev libmypaint-dev libqscintilla2-qt5-dev libqt5opengl5-dev libqt5svg5-dev libquazip5-dev libswresample-dev libswscale-dev libunwind-dev qt5-image-formats-plugins libqt5multimedia5-plugins qtbase5-dev-tools qtbase5-dev qtdeclarative5-dev-tools qtdeclarative5-dev qtmultimedia5-dev qttools5-dev-tools

COPY run.sh /
CMD [ "bash", "./run.sh" ]
