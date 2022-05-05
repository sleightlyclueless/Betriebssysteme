# Docker vars
FROM gcc:4.9
COPY . /usr/src/app
WORKDIR /usr/src/app

# Add cpp files / programm and compile them to exe via gcc
RUN g++ -std=c++11 -o app main.cpp Storage.cpp Manager.cpp CPUSnapshot.cpp CPUData.cpp

# run
CMD ["./app"]
LABEL Name=p5 Version=0.0.1