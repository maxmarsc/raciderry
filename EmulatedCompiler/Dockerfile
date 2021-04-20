FROM arm32v7/debian:latest
COPY qemu-arm-static /usr/bin/qemu-arm-static
RUN apt-get update
RUN apt-get install -y g++ \
        libjack-jackd2-dev \
        libfreetype6-dev \
        libcurl4-openssl-dev \
        git \
        libasound2-dev \
        libsndfile1-dev \
        unzip \
        wget \
        make

# RUN apt-get install -y juce-tools
# RUN apt-get install -y g++ libgtk-3-dev libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev mesa-common-dev libasound2-dev freeglut3-dev libxcomposite-dev libcurl4-openssl-dev
# RUN add-apt-repository -y ppa:webkit-team/ppa && apt-get install libwebkit2gtk-4.0-37 libwebkit2gtk-4.0-dev

WORKDIR /root


RUN wget https://github.com/juce-framework/JUCE/releases/download/6.0.8/juce-6.0.8-linux.zip
RUN unzip juce-6.0.8-linux.zip

CMD ["make", "CONFIG=Release", "-j8", "-C", "raciderry/Builds/Armv7Makefile"]
