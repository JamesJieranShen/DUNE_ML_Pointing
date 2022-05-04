FROM pytorch/pytorch:latest

# Set up time zone.
ENV TZ=EST
RUN apt-get update && apt-get install -y sudo

RUN sudo ln -snf /usr/share/zoneinfo/$TZ /etc/localtime

RUN pip3 install --no-input notebook matplotlib
RUN useradd -m -s /bin/bash -u 1000 james
RUN usermod -aG james james
RUN usermod -aG sudo james
RUN passwd -d james