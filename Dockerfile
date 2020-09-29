FROM gcc:latest as agent_build

ENV DEBIAN_FRONTEND="noninteractive"

# Install build-essntial and dependecies
RUN apt-get update \
 && apt-get install -y apt-utils \
 && apt-get upgrade -y \
 && apt-get install -y \
    libboost-dev \
    libboost-program-options-dev \
    build-essential \
    libwebsocketpp-dev \
    libgoogle-glog-dev \
 && apt-get autoremove --purge -y \
 && apt-get autoclean -y \
 && rm -rf /var/cache/apt/* /tmp/*

# Build Agent
ADD ./main.cpp /tmp/main.cpp
WORKDIR /tmp

ENV CFLAGS="-fstack-protector-strong -fpic -fpie -O2 -pthread"
ENV CPPFLAGS="$CFLAGS"
ENV CXXFLAGS="$CFLAGS"
ENV LDFLAGS="-Wl,-O1 -Wl,--hash-style=both"

RUN g++ -L/usr/local/lib -lglog -I/usr/local/include -o agent main.cpp

# Create final container
FROM ubuntu:latest
LABEL maintainer="Leonid Fedotov <me@ileonidze.com>"
RUN groupadd -r anonymous \
 && useradd -r -g anonymous anonymous \
 && apt-get update \
 && apt-get install -y \
    xserver-xorg-video-dummy \
    x11-xserver-utils \
    firefox \
 && apt-get autoremove --purge -y \
 && apt-get autoclean -y \
 && rm -rf /var/cache/apt/* /tmp/* \
 && export DISPLAY=:0.0 \
 && mkdir -p /usr/share/X11/xorg.conf.d
USER anonymous
COPY --from=agent_build /build/agent /bin/agent
ADD ./xorg.conf /usr/share/X11/xorg.conf.d/xorg.conf

# TODO: run X with /usr/share/X11/xorg.conf.d/xorg.conf
ENTRYPOINT ["/bin/agent"]
